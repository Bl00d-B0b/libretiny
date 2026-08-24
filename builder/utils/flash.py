# Copyright (c) Kuba Szczodrzyński 2022-06-12.

import re
from os.path import isfile, join
from typing import Dict, NoReturn

import click
from ltchiptool.util.fileio import chext
from platformio.platform.base import PlatformBase
from platformio.platform.board import PlatformBoardConfig
from SCons.Script import DefaultEnvironment, Environment

env: Environment = DefaultEnvironment()


def _parse_range(layout: str) -> tuple:
    """Parse a '0xOFFSET+0xLENGTH' layout string into (start, end)."""
    offset, _, length = layout.partition("+")
    start = int(offset, 16)
    return start, start + int(length, 16)


def _fatal(message: str) -> NoReturn:
    """Report a flash layout configuration error and stop the build."""
    click.secho(message, fg="red")
    exit(1)


def _find_bond_conflict(flash_layout: dict, start: int, end: int, exclude: str) -> str:
    """Name of the partition [start, end) collides with, or None.

    `exclude` names the partition being placed. Used for ble_bonding: the
    SDK erases it unilaterally at BLE bring-up, so it alone must not share
    space. Other overlaps in the table (tuya inside userdata, rdp inside
    tuya) are intentional aliases.
    """
    for name, layout in flash_layout.items():
        if name == exclude:
            continue
        p_start, p_end = _parse_range(layout)
        if start < p_end and p_start < end:
            return name
    return None


def _validate_custom_bond_offset(flash_layout: dict, partitions: Dict[str, int]):
    """Reject a custom ble_bonding offset placed inside another partition.

    Runs against the pre-recompute table - the gap-free recompute would
    otherwise silently shrink whatever partition the bond lands in. Moved
    partitions keep their original length, capped at the next requested
    offset so vacated space is not attributed to the original occupant.
    ble_bonding itself must not cap anything, or a bond placed inside an
    unmoved partition would go undetected.
    """
    requested = partitions["ble_bonding"]
    effective = {}
    ordered = sorted(
        (p for p in partitions.items() if p[0] != "ble_bonding"),
        key=lambda p: p[1],
    )
    for i, (name, start) in enumerate(ordered):
        if name not in flash_layout:
            continue
        orig_start, orig_end = _parse_range(flash_layout[name])
        end = start + (orig_end - orig_start)
        if i + 1 < len(ordered):
            end = min(end, ordered[i + 1][1])
        if end > start:
            effective[name] = f"0x{start:06X}+0x{end - start:X}"
    conflict = _find_bond_conflict(
        effective, requested, requested + 1, exclude="ble_bonding"
    )
    if conflict is not None:
        _fatal(
            f"Custom 'ble_bonding' offset 0x{requested:06X} lies inside "
            f"partition '{conflict}' ({effective[conflict]}); the SDK "
            f"erases the bond sector at BLE bring-up, and the layout "
            f"recompute would silently shrink '{conflict}' to make room"
        )


def _validate_bond_placement(flash_layout: dict):
    """Check a board JSON's ble_bonding alignment, size and overlap."""
    bond_start, bond_end = _parse_range(flash_layout["ble_bonding"])
    if bond_start % 0x1000 or bond_end - bond_start < 0x1000:
        _fatal(
            f"'ble_bonding' (0x{bond_start:06X}-0x{bond_end:06X}) must "
            f"be 4 KB sector-aligned and at least one sector long"
        )
    conflict = _find_bond_conflict(
        flash_layout, bond_start, bond_end, exclude="ble_bonding"
    )
    if conflict is not None:
        _fatal(
            f"Flash layout error: 'ble_bonding' "
            f"(0x{bond_start:06X}-0x{bond_end:06X}) overlaps "
            f"'{conflict}' ({flash_layout[conflict]}); the SDK erases "
            f"this sector at BLE bring-up, so it must not share space "
            f"with any other partition"
        )


def env_parse_custom_flash_layout(
    env: Environment,
    platform: PlatformBase,
    board: PlatformBoardConfig,
):
    opts: dict = platform.custom_opts.get("flash", None)
    if not opts:
        return
    flash_layout: dict = board.get("flash")

    # find all default partitions
    partitions: Dict[str, int] = {}
    flash_size = 0
    for name, layout in flash_layout.items():
        offset, _, length = layout.partition("+")
        offset = int(offset, 16)
        length = int(length, 16)
        partitions[name] = offset
        flash_size = max(flash_size, offset + length)

    # set custom offsets
    for name, offset in opts.items():
        offset = int(offset, 0)
        partitions[name] = offset

    if "ble_bonding" in opts:
        _validate_custom_bond_offset(flash_layout, partitions)

    # recalculate partition sizes
    flash_layout = {}
    partitions = sorted(partitions.items(), key=lambda p: p[1])
    for i, (name, offset) in enumerate(partitions):
        end = partitions[i + 1][1] if i + 1 < len(partitions) else flash_size
        length = end - offset
        if length < 0:
            _fatal(
                f"Custom offset 0x{offset:06X} for partition '{name}' lies "
                f"past the end of flash (0x{flash_size:06X})"
            )
        if length == 0:
            if i + 1 < len(partitions):
                _fatal(
                    f"Custom flash offsets collapse partition '{name}' to "
                    f"zero length (offset 0x{offset:06X} is shared with the "
                    f"next partition)"
                )
            _fatal(
                f"Custom offset 0x{offset:06X} for partition '{name}' lies "
                f"at the end of flash (0x{flash_size:06X}); no space remains"
            )
        flash_layout[name] = f"0x{offset:06X}+0x{length:X}"
    board.manifest["flash"] = flash_layout
    env["FLASH_IS_CUSTOM"] = True


def env_add_flash_layout(env: Environment, board: PlatformBoardConfig):
    flash_layout: dict = board.get("flash")
    if flash_layout:
        if "ble_bonding" in flash_layout:
            _validate_bond_placement(flash_layout)
        defines = {}
        flash_size = 0
        fal_items = ""
        # add "root" partition
        fal_items += "FAL_PART_TABLE_ITEM(root,ROOT)"
        # add all partitions
        for name, layout in flash_layout.items():
            name = name.upper()
            offset, _, length = layout.partition("+")
            offset = int(offset, 16)
            length = int(length, 16)
            defines[f"FLASH_{name}_OFFSET"] = f"0x{offset:06X}"
            defines[f"FLASH_{name}_LENGTH"] = f"0x{length:06X}"
            fal_items += f"FAL_PART_TABLE_ITEM({name.lower()}, {name})"
            flash_size = max(flash_size, offset + length)
        defines["FLASH_LENGTH"] = f"0x{flash_size:06X}"
        # for "root" partition
        defines["FLASH_ROOT_OFFSET"] = "0x000000"
        defines["FLASH_ROOT_LENGTH"] = f"0x{flash_size:06X}"
        # add partition table array
        defines["FAL_PART_TABLE"] = "{" + fal_items + "}"
        env.Replace(FLASH_DEFINES=defines)
        env.Replace(**defines)


def env_generate_linker_script(env: Environment, board: PlatformBoardConfig, name: str):
    template_name = chext(name, "template.ld")

    # find the linker script template in LIBPATH
    input = None
    for path in env["LIBPATH"]:
        path = env.subst(path)
        if isfile(join(path, template_name)):
            input = join(path, template_name)
            break
    if not input:
        raise FileNotFoundError(template_name)

    # load the .template.ld script
    with open(input, "r") as f:
        ldscript = f.read()

    def transform(match: re.Match):
        key = match[1]
        if key in env:
            return env[key]
        if key.startswith("BOARD_"):
            key = key[6:].lower()
            return board.get(key)
        raise ValueError(f"Unrecognized template key: {key}")

    ldscript = re.sub(r"\${([A-Z0-9_.]+)}", transform, ldscript)

    # write .ld script
    output = join("${BUILD_DIR}", name)
    with open(env.subst(output), "w") as f:
        f.write(ldscript)

    env.Prepend(LIBPATH=["${BUILD_DIR}"])


env.AddMethod(env_parse_custom_flash_layout, "ParseCustomFlashLayout")
env.AddMethod(env_add_flash_layout, "AddFlashLayout")
env.AddMethod(env_generate_linker_script, "GenerateLinkerScript")
