#include <linux/usb/ch9.h>
#include <usbg/usbg.h>
#include <usbg/function/hid.h>
#include "core/hid/otg.h"

namespace aibox::hid {

OTGDaemon::OTGDaemon() : mouse(std::make_shared<Mouse>()) {}

void OTGDaemon::Start() {
    int res = usbg_init("/sys/kernel/config", &u_state);
    if (res != USBG_SUCCESS) {
        throw std::runtime_error("Error on USB gadget init");
    }
    const auto &config = mouse->GetConfig();
    usbg_gadget_attrs g_attrs = {
            .bDeviceClass = USB_CLASS_PER_INTERFACE,
            .bDeviceSubClass = 0x00,
            .bDeviceProtocol = 0x00,
            .bMaxPacketSize0 = 64,
            .idVendor = config.vendor_id,
            .idProduct = config.product_id,
            .bcdDevice = config.device_version,
    };
}

void OTGDaemon::Stop() {

}

}