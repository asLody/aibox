#include <cstring>
#include <stdexcept>
#include <linux/usb/ch9.h>
#include <usbg/function/hid.h>
#include <usbg/usbg.h>

#include "base/logging.h"
#include "core/usb/usb_gadget.h"

namespace aibox::usb {

UsbGadget::UsbGadget() {
    int ret = usbg_init("/sys/kernel/config", &u_state);
    if (ret != USBG_SUCCESS) {
        throw std::runtime_error("Error on USB gadget init");
    }
    u_gadget = usbg_get_gadget(u_state, gadget_name);
}

UsbGadget::~UsbGadget() { usbg_cleanup(u_state); }

void UsbGadget::Configure(const HIDGadgetDescriptor& descriptor) {
    PrintDescriptor(descriptor);
    if (!u_gadget) {
        CreateGadget(descriptor);
    } else {
        CheckExistingGadget(descriptor);
    }
}

void UsbGadget::PrintDescriptor(const HIDGadgetDescriptor& descriptor) {
    LOG_INFO("Gadget descriptor:");
    LOG_INFO("bcd_usb: {:#x}", descriptor.bcd_usb);
    LOG_INFO("device_subclass: {}", descriptor.device_subclass);
    LOG_INFO("device_protocol: {}", descriptor.device_protocol);
    LOG_INFO("max_packet_size0: {}", descriptor.max_packet_size0);
    LOG_INFO("vendor_id: {:#x}", descriptor.vendor_id);
    LOG_INFO("product_id: {:#x}", descriptor.product_id);
    LOG_INFO("bcd_device: {:#x}", descriptor.bcd_device);
    LOG_INFO("manufacturer: {}", descriptor.manufacturer);
    LOG_INFO("product: {}", descriptor.product);
    LOG_INFO("serial: {}", descriptor.serial);
    LOG_INFO("hid_protocol: {}", descriptor.hid_protocol);
    LOG_INFO("hid_subclass: {}", descriptor.hid_subclass);
    LOG_INFO("report_length: {}", descriptor.report_length);
}

void UsbGadget::CheckExistingGadget(const HIDGadgetDescriptor& descriptor) {
    const auto identifier = GetConfiguredIdentifier();
    if (identifier.vid != descriptor.vendor_id || identifier.pid != descriptor.product_id) {
        throw std::runtime_error("Gadget already exists with different vendor/product id");
    }
    LOG_INFO("Gadget {} already exists, skip creating", gadget_name);
}

void UsbGadget::RemoveGadgets() {
    usbg_gadget* exist_gadget;
    usbg_for_each_gadget(exist_gadget, u_state) { usbg_rm_gadget(exist_gadget, USBG_RM_RECURSE); }
}

void UsbGadget::CreateGadget(const HIDGadgetDescriptor& descriptor) {
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
            .serial = const_cast<char*>(descriptor.serial.empty() ? default_serial
                                                                  : descriptor.serial.c_str()),
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

HIDDeviceIdentifier UsbGadget::GetConfiguredIdentifier() const {
    if (!u_gadget) {
        return {};
    }
    usbg_gadget_attrs g_attrs{};
    int ret = usbg_get_gadget_attrs(u_gadget, &g_attrs);
    if (ret != USBG_SUCCESS) {
        return {};
    }
    return HIDDeviceIdentifier{g_attrs.idVendor, g_attrs.idProduct};
}

}  // namespace aibox::usb