#include "core/usb/report_descriptor.h"
#include "hid/page/button.h"
#include "hid/page/generic_desktop.h"
#include "hid/rdf/descriptor.h"

namespace aibox::usb {

using namespace hid::rdf;
using namespace hid::page;

std::vector<char> ReportDescriptor::GetBytes() const {
    constexpr auto desc = descriptor(
            usage_page<generic_desktop>(),
            usage(generic_desktop::MOUSE),
            collection::application(usage(generic_desktop::POINTER),
                                    collection::physical(usage_page<button>(),
                                                         usage_limits(button(1), button(8)),
                                                         logical_limits<1, int8_t>(0, 1),
                                                         report_count(8),
                                                         report_size(1),
                                                         input::absolute_variable(),
                                                         usage_page<generic_desktop>(),
                                                         usage(generic_desktop::X),
                                                         usage(generic_desktop::Y),
                                                         logical_limits<2, int8_t>(0, 32767),
                                                         report_size(16),
                                                         report_count(2),
                                                         input::absolute_variable(),
                                                         usage(generic_desktop::WHEEL),
                                                         logical_limits<1, int8_t>(-127, 127),
                                                         report_size(8),
                                                         report_count(1),
                                                         input::relative_variable())));
    return std::vector<char>{desc.begin(), desc.end()};
}

uint32_t ReportDescriptor::GetReportLength() const { return 6; }

void ReportDescriptor::WriteToFile(const std::string& path) const {
    FILE* fp = fopen(path.c_str(), "wb");
    const auto& bytes = GetBytes();
    fwrite(bytes.data(), sizeof(char), bytes.size(), fp);
    fclose(fp);
}

}  // namespace aibox::usb