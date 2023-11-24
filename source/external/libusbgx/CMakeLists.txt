add_library(libusbgx STATIC
        src/usbg.c
        src/usbg_common.c
        src/usbg_common.c
        src/usbg_error.c
        src/usbg_schemes_none.c
        src/function/ether.c
        src/function/ether.c
        src/function/ffs.c
        src/function/hid.c
        src/function/loopback.c
        src/function/midi.c
        src/function/ms.c
        src/function/phonet.c
        src/function/printer.c
        src/function/serial.c
        src/function/uac2.c
        src/function/uvc.c)

set(USBG_VERSION_HEX 0x000000000)
configure_file(include/usbg/usbg_version.h.in usbg_version.h)

target_compile_definitions(libusbgx PRIVATE _GNU_SOURCE)

target_include_directories(libusbgx PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(libusbgx PUBLIC ${CMAKE_CURRENT_BINARY_DIR})