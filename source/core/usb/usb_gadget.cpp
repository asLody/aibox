#include <cstring>
#include <stdexcept>
#include <linux/usb/ch9.h>
#include <usbg/function/hid.h>
#include <usbg/usbg.h>

#include "base/logging.h"
#include "core/usb/usb_gadget.h"

namespace aibox::usb {

UsbGadget::UsbGadget() = default;

UsbGadget::~UsbGadget() { usbg_cleanup(u_state); }

void UsbGadget::Configure(const HIDDeviceDescriptor& descriptor) {
    int ret = usbg_init("/sys/kernel/config", &u_state);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error on USB gadget init");
    }
    u_gadget = usbg_get_gadget(u_state, gadget_name);
    if (u_gadget == nullptr) {
        CreateGadget(descriptor);
    } else {
        LOG_INFO("Gadget {} already exists, skip creating", gadget_name);
    }
}

void UsbGadget::RemoveGadgets() {
    usbg_gadget* exist_gadget;
    usbg_for_each_gadget(exist_gadget, u_state) { usbg_rm_gadget(exist_gadget, USBG_RM_RECURSE); }
}

void UsbGadget::CreateGadget(const HIDDeviceDescriptor& descriptor) {
    const usbg_gadget_attrs g_attrs = {
            .bcdUSB = descriptor.bcd_usb,
            .bDeviceClass = USB_CLASS_PER_INTERFACE,
            .bDeviceSubClass = descriptor.device_subclass,
            .bDeviceProtocol = descriptor.device_protocol,
            .bMaxPacketSize0 = descriptor.max_packet_size0,
            .idVendor = descriptor.vendor_id,
            .idProduct = descriptor.product_id,
            .bcdDevice = descriptor.bcd_device,
    };
    struct usbg_gadget_strs g_strs = {
            .manufacturer = const_cast<char*>(descriptor.manufacturer.c_str()),
            .product = const_cast<char*>(descriptor.product.c_str()),
            .serial = const_cast<char*>(descriptor.serial.c_str()),
    };
    struct usbg_config_strs c_strs = {
            .configuration = (char*)"config",
    };
    struct usbg_f_hid_attrs f_attrs = {
            .protocol = descriptor.hid_protocol,
            .report_desc =
                    {
                            .desc = (char*)descriptor.report_descriptor_data.data(),
                            .len = (unsigned int)descriptor.report_descriptor_data.size(),
                    },
            .report_length = descriptor.report_length,
            .subclass = descriptor.hid_subclass,
    };
    usbg_function* u_function{};
    usbg_config* u_config{};

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
    LOG_INFO("Gadget {} has been created", gadget_name);
}

}  // namespace aibox::usb