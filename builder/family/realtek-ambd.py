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
FREERTOS_DIR = join(COMPONENT_DIR, "os", "freertos", "freertos_v10.2.0")
LWIP_DIR = join(COMPONENT_DIR, "common", "network", "lwip", "lwip_v2.0.2")
FREERTOS_PORT_DIR = join(FREERTOS_DIR, "Source", "portable", "GCC", "RTL8721D_HP", "non_secure")

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
    LINKFLAGS=[
        "-mcpu=cortex-m33",
        "-mthumb",
        "-mcmse",
        "-mfloat-abi=hard",
        "-mfpu=fpv5-sp-d16",
        "--specs=nano.specs",
        "-Wl,--gc-sections",
    ],
    CPPPATH=[
        join(SOC_DIR, "cmsis"),
        join(SOC_DIR, "fwlib", "include"),
        join(SOC_DIR, "app", "monitor", "include"),
        join(SOC_DIR, "app", "xmodem"),
        join(SOC_DIR, "swlib", "include"),
        join(SOC_DIR, "swlib", "string"),
        join(SOC_DIR, "misc"),
        join(COMPONENT_DIR, "os", "freertos"),
        # SDK-bundled FreeRTOS 10.2.0 with the vendor RTL8721D_HP port; the
        # library-freertos/-port packages carry no CM33 port, so the SDK copy
        # is the one that builds (same choice the vendor KM4 project makes).
        join(FREERTOS_DIR, "Source", "include"),
        join(FREERTOS_PORT_DIR),
        join("$SDK_DIR", "project", "OpenBeken", "inc", "inc_hp"),
        join(COMPONENT_DIR, "os", "os_dep", "include"),
        join(COMPONENT_DIR, "common", "api"),
        join(COMPONENT_DIR, "common", "api", "wifi"),
        join(COMPONENT_DIR, "common", "api", "network", "include"),
        # rtl8721d_ota.h includes mbedtls/version.h; 2.4.0 is the SDK default
        # (sources join the build later with the SSL/OTA feature work).
        join(COMPONENT_DIR, "common", "network", "ssl", "mbedtls-2.4.0", "include"),
        join(COMPONENT_DIR, "common", "network", "ssl", "ssl_ram_map", "rom"),
        # mbed-style HAL the shared realtek-amb Arduino core builds on
        join(COMPONENT_DIR, "common", "mbed", "api"),
        join(COMPONENT_DIR, "common", "mbed", "hal"),
        join(COMPONENT_DIR, "common", "mbed", "hal_ext"),
        join(COMPONENT_DIR, "common", "mbed", "targets", "hal", "rtl8721d"),
        join(COMPONENT_DIR, "common", "api", "platform"),
        join(COMPONENT_DIR, "common", "network"),
        # SDK-bundled lwIP (v2.0.2, the OpenBeken/vendor default for AmebaD)
        join(LWIP_DIR, "src", "include"),
        join(LWIP_DIR, "port", "realtek"),
        join(LWIP_DIR, "port", "realtek", "freertos"),
        join(COMPONENT_DIR, "common", "drivers", "wlan", "realtek", "include"),
        join(COMPONENT_DIR, "common", "drivers", "wlan", "realtek", "src", "osdep"),
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
        # HTTP/SD-card OTA app code (drags in fatfs); LibreTiny OTA is uf2ota
        "-<misc/rtl8721d_ota.c>",
    ],
    includes=[],
)

# FreeRTOS from the SDK (vendor port + heap_5, per the KM4 project makefile).
queue.AddLibrary(
    name="ambd_freertos",
    base_dir=FREERTOS_DIR,
    srcs=[
        "+<Source/tasks.c>",
        "+<Source/list.c>",
        "+<Source/croutine.c>",
        "+<Source/queue.c>",
        "+<Source/timers.c>",
        "+<Source/event_groups.c>",
        "+<Source/stream_buffer.c>",
        "+<Source/portable/GCC/RTL8721D_HP/non_secure/port.c>",
        "+<Source/portable/GCC/RTL8721D_HP/non_secure/portasm.c>",
        "+<Source/portable/MemMang/heap_5.c>",
    ],
    includes=[],
    options=dict(CFLAGS=["-w"]),
)

# Generate the KM4 img2 linker script (template carries the vendor layout;
# the ROM symbol table and memory layout resolve via LIBPATH INCLUDEs).
env.GenerateLinkerScript(board, board.get("build.ldscript"))

queue.BuildLibraries()
