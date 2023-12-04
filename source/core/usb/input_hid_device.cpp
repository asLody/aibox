#include <stdexcept>
#include "base/file_utils.h"
#include "core/usb/input_hid_device.h"

namespace aibox::usb {

constexpr size_t kMaxReportDescriptorSize = 4096;

constexpr u8 operator|(libusb_endpoint_direction dir, libusb_request_recipient recip) {
    return static_cast<u8>(dir) | static_cast<u8>(recip);
}

InputHIDDevice::InputHIDDevice(HIDDeviceIdentifier identifier) : identifier(identifier) {}

InputHIDDevice::~InputHIDDevice() {
    StopReadInput();
    if (usb_transfer) {
        libusb_free_transfer(usb_transfer);
    }
    if (usb_handle) {
        libusb_release_interface(usb_handle, interface_num);
        libusb_close(usb_handle);
    }
}

void InputHIDDevice::Open() {
    int r = libusb_init(&usb_ctx);
    if (r < 0) throw std::runtime_error("Error initializing USB context");
    libusb_device** devs{};
    ssize_t cnt = libusb_get_device_list(usb_ctx, &devs);
    if (cnt < 0) throw std::runtime_error("Error getting USB device list");

    for (ssize_t i = 0; i < cnt; ++i) {
        libusb_device* dev = devs[i];
        libusb_device_descriptor desc{};
        r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) throw std::runtime_error("Error getting USB device descriptor");

        if (identifier.vid && identifier.pid) {
            if (desc.idVendor != identifier.vid || desc.idProduct != identifier.pid) {
                continue;
            }
        }
        libusb_config_descriptor* config{};
        r = libusb_get_active_config_descriptor(dev, &config);
        if (r < 0) {
            libusb_get_config_descriptor(dev, 0, &config);
        }
        if (!config) continue;

        bool matched = false;
        const libusb_interface_descriptor* interface_desc{};
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
            if (matched) {
                break;
            }
        }
        if (!matched) continue;
        OpenUSB(dev, &desc, interface_desc);
        break;
    }
    libusb_free_device_list(devs, 1);
    if (!usb_handle) {
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
    int r = libusb_open(dev, &usb_handle);
    if (r < 0) throw std::runtime_error("Error opening USB device");

    libusb_attach_kernel_driver(usb_handle, interface_num);
    if (libusb_kernel_driver_active(usb_handle, interface_num) == 1) {
        r = libusb_detach_kernel_driver(usb_handle, interface_num);
        if (r < 0) throw std::runtime_error("Error detaching kernel driver");
    }
    r = libusb_claim_interface(usb_handle, interface_num);
    if (r < 0) throw std::runtime_error("Error claiming USB device interface");

    usb_transfer = libusb_alloc_transfer(0);
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

    descriptor.bcd_usb = desc->bcdUSB;
    descriptor.device_subclass = desc->bDeviceSubClass;
    descriptor.device_protocol = desc->bDeviceProtocol;
    descriptor.max_packet_size0 = desc->bMaxPacketSize0;
    descriptor.vendor_id = desc->idVendor;
    descriptor.product_id = desc->idProduct;
    descriptor.bcd_device = desc->bcdDevice;
    if (desc->iManufacturer > 0) {
        r = libusb_get_string_descriptor_ascii(
                usb_handle, desc->iManufacturer, string.data(), string.size());
        if (r < 0) throw std::runtime_error("Error getting USB device manufacturer");
        descriptor.manufacturer = reinterpret_cast<char*>(string.data());
    }
    if (desc->iProduct > 0) {
        r = libusb_get_string_descriptor_ascii(
                usb_handle, desc->iProduct, string.data(), string.size());
        if (r < 0) throw std::runtime_error("Error getting USB device product");
        descriptor.product = reinterpret_cast<char*>(string.data());
    }
    if (desc->iSerialNumber > 0) {
        r = libusb_get_string_descriptor_ascii(
                usb_handle, desc->iSerialNumber, string.data(), string.size());
        if (r < 0) throw std::runtime_error("Error getting USB device serial");
        descriptor.serial = reinterpret_cast<char*>(string.data());
    }
    descriptor.hid_protocol = interface_desc->bInterfaceProtocol;
    descriptor.hid_subclass = interface_desc->bInterfaceSubClass;

    const u16 report_descriptor_size = ParseReportDescriptorSize(interface_desc);
    descriptor.report_descriptor_data.resize(report_descriptor_size);
    r = libusb_control_transfer(usb_handle,
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

    u32 report_length = 0;
    for (int i = 0; i < descriptor.report_descriptor->rep_count; ++i) {
        if (descriptor.report_descriptor->report[i].input_byte_sz > report_length) {
            report_length = descriptor.report_descriptor->report[i].input_byte_sz;
        }
    }
    if (report_length == 0) {
        throw std::runtime_error("USB device report descriptor has no input reports");
    }
    descriptor.report_length = report_length;
    InitProtocol();
}

void InputHIDDevice::ReadCallback(libusb_transfer* transfer) {
    switch (transfer->status) {
        case LIBUSB_TRANSFER_COMPLETED: {
            HandleInput(transfer->buffer, transfer->actual_length);
            break;
        }
        case LIBUSB_TRANSFER_ERROR:
        case LIBUSB_TRANSFER_CANCELLED:
        case LIBUSB_TRANSFER_NO_DEVICE: {
            is_running = false;
            break;
        }
        default:
            break;
    }
    if (is_running) {
        int r = libusb_submit_transfer(transfer);
        if (r < 0) {
            is_running = false;
        }
    }
}

void InputHIDDevice::StartReadInput() {
    if (is_running) {
        throw std::runtime_error("USB device is already running");
    }
    is_running = true;
    thread = std::thread([&]() {
        const auto read_callback = [](libusb_transfer* transfer) {
            auto device = static_cast<InputHIDDevice*>(transfer->user_data);
            device->ReadCallback(transfer);
        };
        std::vector<u8> buffer(input_ep_max_packet_size);
        libusb_fill_interrupt_transfer(usb_transfer,
                                       usb_handle,
                                       input_endpoint,
                                       buffer.data(),
                                       static_cast<int>(buffer.size()),
                                       read_callback,
                                       this,
                                       5000);
        int r = libusb_submit_transfer(usb_transfer);
        if (r < 0) throw std::runtime_error("Error submitting USB transfer");
        while (is_running) {
            r = libusb_handle_events(usb_ctx);
            if (r < 0) {
                if (r == LIBUSB_ERROR_BUSY || r == LIBUSB_ERROR_TIMEOUT ||
                    r == LIBUSB_ERROR_OVERFLOW || r == LIBUSB_ERROR_INTERRUPTED) {
                    // Just skip these errors
                    continue;
                }
                is_running = false;
                break;
            }
        }
        libusb_cancel_transfer(usb_transfer);
    });
}

void InputHIDDevice::StopReadInput() {
    if (is_running) {
        is_running = false;
        libusb_cancel_transfer(usb_transfer);
        thread.join();
    }
}

}  // namespace aibox::usb
