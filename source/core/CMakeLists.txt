add_library(core STATIC
        core.cpp
        core.h
        usb/mouse.cpp
        usb/mouse.h
        usb/usb_gadget.cpp
        usb/usb_gadget.h
        usb/report_descriptor.cpp
        usb/report_descriptor.h
        usb/config.cpp
        usb/config.h)

target_link_libraries(core libusbgx rknn)