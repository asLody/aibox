#include <stdexcept>
#include "base/file_utils.h"
#include "core/usb/input_hid_device.h"

namespace aibox::usb {

constexpr size_t kMaxReportDescriptorSize = 4096;

constexpr u8 operator|(libusb_endpoint_direction dir, libusb_request_recipient recip) {
    return static_cast<u8>(dir) | static_cast<u8>(recip);
}

InputHIDDevice::InputHIDDevice() : vid(0), pid(0) {}

InputHIDDevice::InputHIDDevice(u16 vid, u16 pid) : vid(vid), pid(pid) {}

InputHIDDevice::~InputHIDDevice() {
    if (handle) {
        libusb_release_interface(handle, interface_num);
        libusb_close(handle);
    }
}

void InputHIDDevice::Open() {
    libusb_init(nullptr);
    libusb_device** devs{};
    ssize_t cnt = libusb_get_device_list(nullptr, &devs);
    if (cnt < 0) throw std::runtime_error("Error getting USB device list");

    for (ssize_t i = 0; i < cnt; ++i) {
        libusb_device* dev = devs[i];
        libusb_device_descriptor desc{};
        int r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) throw std::runtime_error("Error getting USB device descriptor");

        if (vid && pid) {
            if (desc.idVendor != vid || desc.idProduct != pid) {
                continue;
            }
        }
        libusb_config_descriptor* config{};
        libusb_get_active_config_descriptor(dev, &config);
        if (!config) continue;

        bool matched = false;
        const libusb_interface_descriptor* interface_desc;
        for (int j = 0; j < config->bNumInterfaces; ++j) {
            const libusb_interface* interface = &config->interface[j];
            for (int k = 0; k < interface->num_altsetting; ++k) {
                interface_desc = &interface->altsetting[k];
                if (MatchInterfaceDescriptor(interface_desc)) {
                    interface_num = interface_desc->bInterfaceNumber;
                    matched = true;
                    break;
                }
            }
        }
        if (!matched) continue;
        OpenUSB(dev, &desc, interface_desc);
        break;
    }
    libusb_free_device_list(devs, 1);
    if (!handle) {
        throw std::runtime_error("USB device not found");
    }
}

size_t InputHIDDevice::ParseReportDescriptorSize(
        const struct libusb_interface_descriptor* interface_desc) {
    size_t result = kMaxReportDescriptorSize;
    const unsigned char* extra = interface_desc->extra;
    int extra_length = interface_desc->extra_length;

    while (extra_length >= 2) {
        if (extra[1] == LIBUSB_DT_HID) {
            if (extra_length < 6) {
                break;
            }
            unsigned char num_descriptors = extra[5];
            if (extra_length < (6 + 3 * num_descriptors)) {
                break;
            }
            for (int i = 0; i < num_descriptors; i++) {
                if (extra[6 + 3 * i] == LIBUSB_DT_REPORT) {
                    result = (u16)extra[6 + 3 * i + 2] << 8 | extra[6 + 3 * i + 1];
                    break;
                }
            }
            break;
        }

        if (extra[0] == 0) {
            break;
        }

        extra_length -= extra[0];
        extra += extra[0];
    }

    return result;
}

bool InputHIDDevice::MatchInterfaceDescriptor(const libusb_interface_descriptor* interface_desc) {
    return interface_desc->bInterfaceClass == LIBUSB_CLASS_HID;
}

void InputHIDDevice::OpenUSB(libusb_device* dev,
                             const libusb_device_descriptor* desc,
                             const libusb_interface_descriptor* interface_desc) {
    std::array<u8, 256> string{};
    int r = libusb_open(dev, &handle);
    if (r < 0) throw std::runtime_error("Error opening USB device");

    if (libusb_kernel_driver_active(handle, interface_num) == 1) {
        r = libusb_detach_kernel_driver(handle, interface_num);
        if (r < 0) throw std::runtime_error("Error detaching kernel driver");
    }
    r = libusb_claim_interface(handle, interface_num);
    if (r < 0) throw std::runtime_error("Error claiming USB device interface");

    transfer = libusb_alloc_transfer(0);
    for (int i = 0; i < interface_desc->bNumEndpoints; ++i) {
        const struct libusb_endpoint_descriptor* ep = &interface_desc->endpoint[i];
        const int is_interrupt =
                (ep->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_INTERRUPT;
        const int is_output =
                (ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_OUT;
        const int is_input =
                (ep->bEndpointAddress & LIBUSB_ENDPOINT_DIR_MASK) == LIBUSB_ENDPOINT_IN;

        if (input_endpoint == 0 && is_interrupt && is_input) {
            input_endpoint = ep->bEndpointAddress;
            input_ep_max_packet_size = ep->wMaxPacketSize;
        }
        if (output_endpoint == 0 && is_interrupt && is_output) {
            output_endpoint = ep->bEndpointAddress;
        }
    }

    descriptor.bcd_usb = desc->bcdDevice;
    descriptor.device_subclass = desc->bDeviceSubClass;
    descriptor.device_protocol = desc->bDeviceProtocol;
    descriptor.max_packet_size0 = desc->bMaxPacketSize0;
    descriptor.vendor_id = desc->idVendor;
    descriptor.product_id = desc->idProduct;
    descriptor.bcd_device = desc->bcdDevice;
    if (desc->iManufacturer > 0) {
        r = libusb_get_string_descriptor_ascii(
                handle, desc->iManufacturer, string.data(), string.size());
        if (r < 0) throw std::runtime_error("Error getting USB device manufacturer");
        descriptor.manufacturer = reinterpret_cast<char*>(string.data());
    }
    if (desc->iProduct > 0) {
        r = libusb_get_string_descriptor_ascii(
                handle, desc->iProduct, string.data(), string.size());
        if (r < 0) throw std::runtime_error("Error getting USB device product");
        descriptor.product = reinterpret_cast<char*>(string.data());
    }
    if (desc->iSerialNumber > 0) {
        r = libusb_get_string_descriptor_ascii(
                handle, desc->iSerialNumber, string.data(), string.size());
        if (r < 0) throw std::runtime_error("Error getting USB device serial");
        descriptor.serial = reinterpret_cast<char*>(string.data());
    }
    descriptor.hid_protocol = interface_desc->bInterfaceProtocol;
    descriptor.hid_subclass = interface_desc->bInterfaceSubClass;

    const u16 report_descriptor_size = ParseReportDescriptorSize(interface_desc);
    descriptor.report_descriptor_data.resize(report_descriptor_size);
    r = libusb_control_transfer(handle,
                                LIBUSB_ENDPOINT_IN | LIBUSB_RECIPIENT_INTERFACE,
                                LIBUSB_REQUEST_GET_DESCRIPTOR,
                                (LIBUSB_DT_REPORT << 8),
                                interface_num,
                                descriptor.report_descriptor_data.data(),
                                descriptor.report_descriptor_data.size(),
                                1500);
    if (r < 0) throw std::runtime_error("Error getting USB device report descriptor");
    hid::ParseResult parse_res =
            hid::ParseReportDescriptor(descriptor.report_descriptor_data.data(),
                                       report_descriptor_size,
                                       &descriptor.report_descriptor);
    if (parse_res != hid::kParseOk)
        throw std::runtime_error("Error parsing USB device report descriptor");

    if (descriptor.report_descriptor->rep_count != 1)
        throw std::runtime_error("USB device report descriptor has more than one report");

    descriptor.report_length = descriptor.report_descriptor->report[0].input_byte_sz;
    InitProtocol();
}

}  // namespace aibox::usb
