add_library(core STATIC
        core.cpp
        core.h
        hid/mouse.cpp
        hid/mouse.h
        hid/otg.cpp
        hid/otg.h)

target_link_libraries(core libusbgx rknn)