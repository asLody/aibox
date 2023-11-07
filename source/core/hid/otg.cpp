#include <linux/usb/ch9.h>
#include <usbg/function/hid.h>
#include <usbg/usbg.h>
#include "core/hid/otg.h"

namespace aibox::hid {

OTGDaemon::OTGDaemon() : mouse(std::make_shared<Mouse>()) {}

OTGDaemon::~OTGDaemon() = default;

void OTGDaemon::Start() {
    int ret = usbg_init("/sys/kernel/config", &u_state);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error on USB gadget init");
    }
    const auto& config = mouse->GetConfig();
    usbg_gadget_attrs g_attrs = {
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
    const auto report_desc = mouse->GetReportDescriptor();
    struct usbg_f_hid_attrs f_attrs = {
            .protocol = 0,
            .report_desc =
                    {
                            .desc = const_cast<char*>(report_desc.data()),
                            .len = static_cast<unsigned int>(report_desc.size()),
                    },
            .report_length = 6,
            .subclass = 0,
    };
    usbg_function* u_function{};
    usbg_config* u_config{};

    ret = usbg_create_gadget(u_state, "g2", &g_attrs, &g_strs, &u_gadget);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error creating gadget");
    }
    ret = usbg_create_function(u_gadget, USBG_F_HID, "usb0", &f_attrs, &u_function);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error creating function");
    }
    ret = usbg_create_config(u_gadget, 1, "config.1", nullptr, &c_strs, &u_config);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error creating config");
    }
    ret = usbg_add_config_function(u_config, "hid.usb0", u_function);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error adding function to config");
    }
    ret = usbg_enable_gadget(u_gadget, DEFAULT_UDC);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error enabling gadget");
    }
}

void OTGDaemon::Stop() {
    usbg_disable_gadget(u_gadget);
    usbg_cleanup(u_state);
}

}  // namespace aibox::hid