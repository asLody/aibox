#include <stdint.h>
#include <stdlib.h>
#include "core/usb/hid-parser/parser.h"
#include "core/usb/hid-parser/units.h"

namespace hid {

// Extracts |value_out| from |report| as the closest unit type to |attr.unit|. The unit type
// can be gotten from the |GetUnitTypeFromUnit| function. This is the recommended
// extraction function for the users of this library.
bool ExtractAsUnitType(const uint8_t* report,
                       size_t report_len,
                       const hid::Attributes& attr,
                       double* value_out);
// Inserts |value_in| into |report|. This function assumes that |value_in| is in the
// unit type closest to |attr.unit|.
bool InsertAsUnitType(uint8_t* report,
                      size_t report_len,
                      const hid::Attributes& attr,
                      double value_in);

// Extracts |value_out| from |report| and ensures that is in the units
// specified by |attr|.
bool ExtractAsUnit(const uint8_t* report,
                   size_t report_len,
                   const hid::Attributes& attr,
                   double* value_out);
// Inserts |value_in| into |report| after it has been translated into
// the logical units described by |attr|.
bool InsertAsUnit(uint8_t* report, size_t report_len, const hid::Attributes& attr, double value_in);

// Extracts |value_out| from |report| and converts it into the units
// specified by |unit_out|.
bool ExtractWithUnit(const uint8_t* report,
                     size_t report_len,
                     const hid::Attributes& attr,
                     const Unit& unit_out,
                     double* value_out);
// Given |value_in| with units |unit_in|, it converts it to |attr.unit| and
// then inserts it into |report|.
bool InsertWithUnit(uint8_t* report,
                    size_t report_len,
                    const hid::Attributes& attr,
                    const Unit& unit_in,
                    double value_in);

// Helper functions that extracts the raw byte data from a report. This is only
// recommended for users that know what they are doing and are willing to
// use raw data or do their own conversion between logical and physical values.
bool ExtractUint(const uint8_t* report,
                 size_t report_len,
                 const hid::Attributes& attr,
                 uint8_t* value_out);
bool ExtractUint(const uint8_t* report,
                 size_t report_len,
                 const hid::Attributes& attr,
                 uint16_t* value_out);
bool ExtractUint(const uint8_t* report,
                 size_t report_len,
                 const hid::Attributes& attr,
                 uint32_t* value_out);

bool InsertUint(uint8_t* report, size_t report_len, const hid::Attributes& attr, uint32_t value_in);

}  // namespace hid
