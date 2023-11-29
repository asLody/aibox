#include "core/usb/mouse/output_mouse.h"

namespace aibox::usb {

OutputMouse::OutputMouse() = default;

OutputMouse::~OutputMouse() { Close(); }

}  // namespace aibox::usb