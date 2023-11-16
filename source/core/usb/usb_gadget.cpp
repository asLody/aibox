#include <cstring>
#include <linux/usb/ch9.h>
#include <usbg/function/hid.h>
#include <usbg/usbg.h>

#include "core/usb/usb_gadget.h"

namespace aibox::usb {

UsbGadget::UsbGadget() = default;

UsbGadget::~UsbGadget() { usbg_cleanup(u_state); }

void UsbGadget::Configure() {
    int ret = usbg_init("/sys/kernel/config", &u_state);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error on USB gadget init");
    }
    u_gadget = usbg_get_gadget(u_state, gadget_name);
    if (u_gadget == nullptr) {
        CreateGadget();
    } else {
        printf("Gadget already exists!\n");
    }

    mouse = std::make_unique<Mouse>(config);
    mouse->Open("/dev/hidg0");
}

void UsbGadget::RemoveGadgets() {
    usbg_gadget* exist_gadget;
    usbg_for_each_gadget(exist_gadget, u_state) { usbg_rm_gadget(exist_gadget, USBG_RM_RECURSE); }
}

void UsbGadget::CreateGadget() {
    const usbg_gadget_attrs g_attrs = {
            .bcdUSB = 0x0200,
            .bDeviceClass = USB_CLASS_PER_INTERFACE,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize0 = 64,
            .idVendor = config.vendor_id,
            .idProduct = config.product_id,
            .bcdDevice = config.device_version,
    };
    struct usbg_gadget_strs g_strs = {
            .manufacturer = const_cast<char*>(config.manufacturer.c_str()),
            .product = const_cast<char*>(config.product.c_str()),
            .serial = const_cast<char*>(config.serial.c_str()),
    };
    struct usbg_config_strs c_strs = {
            .configuration = const_cast<char*>(config.configuration.c_str()),
    };
    const auto& report_desc = config.report_descriptor;
    const auto& data = report_desc.GetBytes();

    struct usbg_f_hid_attrs f_attrs = {
            .protocol = 0,
            .report_desc =
                    {
                            .desc = const_cast<char*>(data.data()),
                            .len = static_cast<unsigned int>(data.size()),
                    },
            .report_length = report_desc.GetReportLength(),
            .subclass = 0,
    };
    usbg_function* u_function{};
    usbg_config* u_config{};

    // Start creating our gadget.
    int ret = usbg_create_gadget(u_state, gadget_name, &g_attrs, &g_strs, &u_gadget);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error creating gadget");
    }
    ret = usbg_create_function(u_gadget, USBG_F_HID, "usb0", &f_attrs, &u_function);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error creating function");
    }
    ret = usbg_create_config(u_gadget, 1, nullptr, nullptr, &c_strs, &u_config);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error creating config");
    }
    ret = usbg_add_config_function(u_config, "hid.usb0", u_function);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error adding function to config");
    }
    ret = usbg_enable_gadget(u_gadget, nullptr);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error enabling gadget");
    }
    printf("Active usb gadget success!\n");
}

}  // namespace aibox::usb