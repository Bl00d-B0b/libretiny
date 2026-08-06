# Copyright (c) Bl00d-B0b 2026-08-06.

from os.path import join

from platformio.platform.base import PlatformBase
from platformio.platform.board import PlatformBoardConfig
from SCons.Script import DefaultEnvironment, Environment

env: Environment = DefaultEnvironment()
platform: PlatformBase = env.PioPlatform()
board: PlatformBoardConfig = env.BoardConfig()
queue = env.AddLibraryQueue("realtek-ambd")
env.ConfigureFamily()

COMPONENT_DIR = join("$SDK_DIR", "component")
SOC_DIR = join(COMPONENT_DIR, "soc", "realtek", "amebad")

# KM4 (Cortex-M33) application build. KM0 image and both boot images ship
# precompiled in the framework package; only the KM4 image2 is compiled here.
queue.AppendPublic(
    CCFLAGS=[
        "-march=armv8-m.main+dsp",
        "-mthumb",
        "-mcmse",
        "-mfloat-abi=hard",
        "-mfpu=fpv5-sp-d16",
    ],
    CPPDEFINES=[
        ("CONFIG_PLATFORM_8721D", 1),
        ("ARM_CORE_CM4", 1),
        ("CONFIG_BUILD_RAM", 1),
    ],
    CPPPATH=[
        join(SOC_DIR, "cmsis"),
        join(SOC_DIR, "fwlib", "include"),
        join(SOC_DIR, "app", "monitor", "include"),
        join(SOC_DIR, "swlib", "include"),
        join(SOC_DIR, "misc"),
        join(COMPONENT_DIR, "os", "freertos"),
        join(COMPONENT_DIR, "os", "os_dep", "include"),
        join(COMPONENT_DIR, "common", "api"),
        join(COMPONENT_DIR, "common", "api", "wifi"),
        join(COMPONENT_DIR, "common", "api", "network", "include"),
    ],
)

# Sources: fwlib + app glue compile with the core; wlan/BT link from the
# prebuilt SDK archives (lib_wlan.a etc.) shipped in the framework package.
queue.AddLibrary(
    name="ambd_fwlib",
    base_dir=SOC_DIR,
    srcs=[
        "+<fwlib/ram_hp/*.c>",
        "+<misc/*.c>",
    ],
    includes=[],
)

env.BuildLibraries()
