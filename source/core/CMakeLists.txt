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
        usb/config.h
        model/model.cpp
        model/model.h
        model/rknn/model_rknn.cpp
        model/rknn/model_rknn.h
        model/yolo/yolo_v6.cpp
        model/yolo/yolo_v6.h
        model/object_detector.cpp
        model/object_detector.h
        model/yolo/yolo.cpp
        model/yolo/yolo.h
)

target_link_libraries(core base hid-rp libusbgx rknn CImg)