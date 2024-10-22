#ifndef __HID_PAGE_BAR_CODE_SCANNER_H_
#define __HID_PAGE_BAR_CODE_SCANNER_H_

#include "hid/usage.h"

namespace hid::page
{
    enum class bar_code_scanner : usage_id_type;
    template<>
    struct info<bar_code_scanner>
    {
        constexpr static usage_id_type base_id = 0x008c0000;
        constexpr static usage_id_type max_usage = 0x0121 | base_id;
        constexpr static const char* name = "Bar Code Scanner";
    };
    enum class bar_code_scanner : usage_id_type
    {
        BAR_CODE_BADGE_READER = 0x0001 | info<bar_code_scanner>::base_id,
        BAR_CODE_SCANNER = 0x0002 | info<bar_code_scanner>::base_id,
        DUMB_BAR_CODE_SCANNER = 0x0003 | info<bar_code_scanner>::base_id,
        CORDLESS_SCANNER_BASE = 0x0004 | info<bar_code_scanner>::base_id,
        BAR_CODE_SCANNER_CRADLE = 0x0005 | info<bar_code_scanner>::base_id,
        ATTRIBUTE_REPORT = 0x0010 | info<bar_code_scanner>::base_id,
        SETTINGS_REPORT = 0x0011 | info<bar_code_scanner>::base_id,
        SCANNED_DATA_REPORT = 0x0012 | info<bar_code_scanner>::base_id,
        RAW_SCANNED_DATA_REPORT = 0x0013 | info<bar_code_scanner>::base_id,
        TRIGGER_REPORT = 0x0014 | info<bar_code_scanner>::base_id,
        STATUS_REPORT = 0x0015 | info<bar_code_scanner>::base_id,
        UPC_EAN_CONTROL_REPORT = 0x0016 | info<bar_code_scanner>::base_id,
        EAN_2_3_LABEL_CONTROL_REPORT = 0x0017 | info<bar_code_scanner>::base_id,
        CODE_39_CONTROL_REPORT = 0x0018 | info<bar_code_scanner>::base_id,
        INTERLEAVED_2_OF_5_CONTROL_REPORT = 0x0019 | info<bar_code_scanner>::base_id,
        STANDARD_2_OF_5_CONTROL_REPORT = 0x001a | info<bar_code_scanner>::base_id,
        MSI_PLESSEY_CONTROL_REPORT = 0x001b | info<bar_code_scanner>::base_id,
        CODABAR_CONTROL_REPORT = 0x001c | info<bar_code_scanner>::base_id,
        CODE_128_CONTROL_REPORT = 0x001d | info<bar_code_scanner>::base_id,
        MISC_1D_CONTROL_REPORT = 0x001e | info<bar_code_scanner>::base_id,
        _2D_CONTROL_REPORT = 0x001f | info<bar_code_scanner>::base_id,
        AIMING_POINTER_MODE = 0x0030 | info<bar_code_scanner>::base_id,
        BAR_CODE_PRESENT_SENSOR = 0x0031 | info<bar_code_scanner>::base_id,
        CLASS_1A_LASER = 0x0032 | info<bar_code_scanner>::base_id,
        CLASS_2_LASER = 0x0033 | info<bar_code_scanner>::base_id,
        HEATER_PRESENT = 0x0034 | info<bar_code_scanner>::base_id,
        CONTACT_SCANNER = 0x0035 | info<bar_code_scanner>::base_id,
        ELECTRONIC_ARTICLE_SURVEILLANCE_NOTIFICATION = 0x0036 | info<bar_code_scanner>::base_id,
        CONSTANT_ELECTRONIC_ARTICLE_SURVEILLANCE = 0x0037 | info<bar_code_scanner>::base_id,
        ERROR_INDICATION = 0x0038 | info<bar_code_scanner>::base_id,
        FIXED_BEEPER = 0x0039 | info<bar_code_scanner>::base_id,
        GOOD_DECODE_INDICATION = 0x003a | info<bar_code_scanner>::base_id,
        HANDS_FREE_SCANNING = 0x003b | info<bar_code_scanner>::base_id,
        INTRINSICALLY_SAFE = 0x003c | info<bar_code_scanner>::base_id,
        CLASS_1_LASER = 0x003d | info<bar_code_scanner>::base_id,
        LONG_RANGE_SCANNER = 0x003e | info<bar_code_scanner>::base_id,
        MIRROR_SPEED_CONTROL = 0x003f | info<bar_code_scanner>::base_id,
        NOT_ON_FILE_INDICATION = 0x0040 | info<bar_code_scanner>::base_id,
        PROGRAMMABLE_BEEPER = 0x0041 | info<bar_code_scanner>::base_id,
        TRIGGERLESS = 0x0042 | info<bar_code_scanner>::base_id,
        WAND = 0x0043 | info<bar_code_scanner>::base_id,
        WATER_RESISTANT = 0x0044 | info<bar_code_scanner>::base_id,
        MULTI_RANGE_SCANNER = 0x0045 | info<bar_code_scanner>::base_id,
        PROXIMITY_SENSOR = 0x0046 | info<bar_code_scanner>::base_id,
        FRAGMENT_DECODING = 0x004d | info<bar_code_scanner>::base_id,
        SCANNER_READ_CONFIDENCE = 0x004e | info<bar_code_scanner>::base_id,
        DATA_PREFIX = 0x004f | info<bar_code_scanner>::base_id,
        PREFIX_AIMI = 0x0050 | info<bar_code_scanner>::base_id,
        PREFIX_NONE = 0x0051 | info<bar_code_scanner>::base_id,
        PREFIX_PROPRIETARY = 0x0052 | info<bar_code_scanner>::base_id,
        ACTIVE_TIME = 0x0055 | info<bar_code_scanner>::base_id,
        AIMING_LASER_PATTERN = 0x0056 | info<bar_code_scanner>::base_id,
        BAR_CODE_PRESENT = 0x0057 | info<bar_code_scanner>::base_id,
        BEEPER_STATE = 0x0058 | info<bar_code_scanner>::base_id,
        LASER_ON_TIME = 0x0059 | info<bar_code_scanner>::base_id,
        LASER_STATE = 0x005a | info<bar_code_scanner>::base_id,
        LOCKOUT_TIME = 0x005b | info<bar_code_scanner>::base_id,
        MOTOR_STATE = 0x005c | info<bar_code_scanner>::base_id,
        MOTOR_TIMEOUT = 0x005d | info<bar_code_scanner>::base_id,
        POWER_ON_RESET_SCANNER = 0x005e | info<bar_code_scanner>::base_id,
        PREVENT_READ_OF_BARCODES = 0x005f | info<bar_code_scanner>::base_id,
        INITIATE_BARCODE_READ = 0x0060 | info<bar_code_scanner>::base_id,
        TRIGGER_STATE = 0x0061 | info<bar_code_scanner>::base_id,
        TRIGGER_MODE = 0x0062 | info<bar_code_scanner>::base_id,
        TRIGGER_MODE_BLINKING_LASER_ON = 0x0063 | info<bar_code_scanner>::base_id,
        TRIGGER_MODE_CONTINUOUS_LASER_ON = 0x0064 | info<bar_code_scanner>::base_id,
        TRIGGER_MODE_LASER_ON_WHILE_PULLED = 0x0065 | info<bar_code_scanner>::base_id,
        TRIGGER_MODE_LASER_STAYS_ON_AFTER_TRIGGER_RELEASE = 0x0066 | info<bar_code_scanner>::base_id,
        COMMIT_PARAMETERS_TO_NVM = 0x006d | info<bar_code_scanner>::base_id,
        PARAMETER_SCANNING = 0x006e | info<bar_code_scanner>::base_id,
        PARAMETERS_CHANGED = 0x006f | info<bar_code_scanner>::base_id,
        SET_PARAMETER_DEFAULT_VALUES = 0x0070 | info<bar_code_scanner>::base_id,
        SCANNER_IN_CRADLE = 0x0075 | info<bar_code_scanner>::base_id,
        SCANNER_IN_RANGE = 0x0076 | info<bar_code_scanner>::base_id,
        AIM_DURATION = 0x007a | info<bar_code_scanner>::base_id,
        GOOD_READ_LAMP_DURATION = 0x007b | info<bar_code_scanner>::base_id,
        GOOD_READ_LAMP_INTENSITY = 0x007c | info<bar_code_scanner>::base_id,
        GOOD_READ_LED = 0x007d | info<bar_code_scanner>::base_id,
        GOOD_READ_TONE_FREQUENCY = 0x007e | info<bar_code_scanner>::base_id,
        GOOD_READ_TONE_LENGTH = 0x007f | info<bar_code_scanner>::base_id,
        GOOD_READ_TONE_VOLUME = 0x0080 | info<bar_code_scanner>::base_id,
        NO_READ_MESSAGE = 0x0082 | info<bar_code_scanner>::base_id,
        NOT_ON_FILE_VOLUME = 0x0083 | info<bar_code_scanner>::base_id,
        POWERUP_BEEP = 0x0084 | info<bar_code_scanner>::base_id,
        SOUND_ERROR_BEEP = 0x0085 | info<bar_code_scanner>::base_id,
        SOUND_GOOD_READ_BEEP = 0x0086 | info<bar_code_scanner>::base_id,
        SOUND_NOT_ON_FILE_BEEP = 0x0087 | info<bar_code_scanner>::base_id,
        GOOD_READ_WHEN_TO_WRITE = 0x0088 | info<bar_code_scanner>::base_id,
        GRWTI_AFTER_DECODE = 0x0089 | info<bar_code_scanner>::base_id,
        GRWTI_BEEP_LAMP_AFTER_TRANSMIT = 0x008a | info<bar_code_scanner>::base_id,
        GRWTI_NO_BEEP_LAMP_USE_AT_ALL = 0x008b | info<bar_code_scanner>::base_id,
        BOOKLAND_EAN = 0x0091 | info<bar_code_scanner>::base_id,
        CONVERT_EAN_8_TO_13_TYPE = 0x0092 | info<bar_code_scanner>::base_id,
        CONVERT_UPC_A_TO_EAN_13 = 0x0093 | info<bar_code_scanner>::base_id,
        CONVERT_UPC_E_TO_A = 0x0094 | info<bar_code_scanner>::base_id,
        EAN_13 = 0x0095 | info<bar_code_scanner>::base_id,
        EAN_8 = 0x0096 | info<bar_code_scanner>::base_id,
        EAN_99_128_MANDATORY = 0x0097 | info<bar_code_scanner>::base_id,
        EAN_99_P5_128_OPTIONAL = 0x0098 | info<bar_code_scanner>::base_id,
        UPC_EAN = 0x009a | info<bar_code_scanner>::base_id,
        UPC_EAN_COUPON_CODE = 0x009b | info<bar_code_scanner>::base_id,
        UPC_EAN_PERIODICALS = 0x009c | info<bar_code_scanner>::base_id,
        UPC_A = 0x009d | info<bar_code_scanner>::base_id,
        UPC_A_WITH_128_MANDATORY = 0x009e | info<bar_code_scanner>::base_id,
        UPC_A_WITH_128_OPTIONAL = 0x009f | info<bar_code_scanner>::base_id,
        UPC_A_WITH_P5_OPTIONAL = 0x00a0 | info<bar_code_scanner>::base_id,
        UPC_E = 0x00a1 | info<bar_code_scanner>::base_id,
        UPC_E1 = 0x00a2 | info<bar_code_scanner>::base_id,
        PERIODICAL = 0x00a9 | info<bar_code_scanner>::base_id,
        PERIODICAL_AUTO_DISCRIMINATE_2 = 0x00aa | info<bar_code_scanner>::base_id,
        PERIODICAL_ONLY_DECODE_WITH_2 = 0x00ab | info<bar_code_scanner>::base_id,
        PERIODICAL_IGNORE_2 = 0x00ac | info<bar_code_scanner>::base_id,
        PERIODICAL_AUTO_DISCRIMINATE_5 = 0x00ad | info<bar_code_scanner>::base_id,
        PERIODICAL_ONLY_DECODE_WITH_5 = 0x00ae | info<bar_code_scanner>::base_id,
        PERIODICAL_IGNORE_5 = 0x00af | info<bar_code_scanner>::base_id,
        CHECK = 0x00b0 | info<bar_code_scanner>::base_id,
        CHECK_DISABLE_PRICE = 0x00b1 | info<bar_code_scanner>::base_id,
        CHECK_ENABLE_4_DIGIT_PRICE = 0x00b2 | info<bar_code_scanner>::base_id,
        CHECK_ENABLE_5_DIGIT_PRICE = 0x00b3 | info<bar_code_scanner>::base_id,
        CHECK_ENABLE_EUROPEAN_4_DIGIT_PRICE = 0x00b4 | info<bar_code_scanner>::base_id,
        CHECK_ENABLE_EUROPEAN_5_DIGIT_PRICE = 0x00b5 | info<bar_code_scanner>::base_id,
        EAN_TWO_LABEL = 0x00b7 | info<bar_code_scanner>::base_id,
        EAN_THREE_LABEL = 0x00b8 | info<bar_code_scanner>::base_id,
        EAN_8_FLAG_DIGIT_1 = 0x00b9 | info<bar_code_scanner>::base_id,
        EAN_8_FLAG_DIGIT_2 = 0x00ba | info<bar_code_scanner>::base_id,
        EAN_8_FLAG_DIGIT_3 = 0x00bb | info<bar_code_scanner>::base_id,
        EAN_13_FLAG_DIGIT_1 = 0x00bc | info<bar_code_scanner>::base_id,
        EAN_13_FLAG_DIGIT_2 = 0x00bd | info<bar_code_scanner>::base_id,
        EAN_13_FLAG_DIGIT_3 = 0x00be | info<bar_code_scanner>::base_id,
        ADD_EAN_2_3_LABEL_DEFINITION = 0x00bf | info<bar_code_scanner>::base_id,
        CLEAR_ALL_EAN_2_3_LABEL_DEFINITIONS = 0x00c0 | info<bar_code_scanner>::base_id,
        CODABAR = 0x00c3 | info<bar_code_scanner>::base_id,
        CODE_128 = 0x00c4 | info<bar_code_scanner>::base_id,
        CODE_39 = 0x00c7 | info<bar_code_scanner>::base_id,
        CODE_93 = 0x00c8 | info<bar_code_scanner>::base_id,
        FULL_ASCII_CONVERSION = 0x00c9 | info<bar_code_scanner>::base_id,
        INTERLEAVED_2_OF_5 = 0x00ca | info<bar_code_scanner>::base_id,
        ITALIAN_PHARMACY_CODE = 0x00cb | info<bar_code_scanner>::base_id,
        MSI_PLESSEY = 0x00cc | info<bar_code_scanner>::base_id,
        STANDARD_2_OF_5_IATA = 0x00cd | info<bar_code_scanner>::base_id,
        STANDARD_2_OF_5 = 0x00ce | info<bar_code_scanner>::base_id,
        TRANSMIT_START_STOP = 0x00d3 | info<bar_code_scanner>::base_id,
        TRI_OPTIC = 0x00d4 | info<bar_code_scanner>::base_id,
        UCC_EAN_128 = 0x00d5 | info<bar_code_scanner>::base_id,
        CHECK_DIGIT = 0x00d6 | info<bar_code_scanner>::base_id,
        CHECK_DIGIT_DISABLE = 0x00d7 | info<bar_code_scanner>::base_id,
        CHECK_DIGIT_ENABLE_INTERLEAVED_2_OF_5_OPCC = 0x00d8 | info<bar_code_scanner>::base_id,
        CHECK_DIGIT_ENABLE_INTERLEAVED_2_OF_5_USS = 0x00d9 | info<bar_code_scanner>::base_id,
        CHECK_DIGIT_ENABLE_STANDARD_2_OF_5_OPCC = 0x00da | info<bar_code_scanner>::base_id,
        CHECK_DIGIT_ENABLE_STANDARD_2_OF_5_USS = 0x00db | info<bar_code_scanner>::base_id,
        CHECK_DIGIT_ENABLE_ONE_MSI_PLESSEY = 0x00dc | info<bar_code_scanner>::base_id,
        CHECK_DIGIT_ENABLE_TWO_MSI_PLESSEY = 0x00dd | info<bar_code_scanner>::base_id,
        CHECK_DIGIT_CODABAR_ENABLE = 0x00de | info<bar_code_scanner>::base_id,
        CHECK_DIGIT_CODE_39_ENABLE = 0x00df | info<bar_code_scanner>::base_id,
        TRANSMIT_CHECK_DIGIT = 0x00f0 | info<bar_code_scanner>::base_id,
        DISABLE_CHECK_DIGIT_TRANSMIT = 0x00f1 | info<bar_code_scanner>::base_id,
        ENABLE_CHECK_DIGIT_TRANSMIT = 0x00f2 | info<bar_code_scanner>::base_id,
        // "SYMBOLOGY IDENTIFIER" = 0x00fb | info<bar_code_scanner>::base_id,
        // "SYMBOLOGY IDENTIFIER" = 0x00fc | info<bar_code_scanner>::base_id,
        // "SYMBOLOGY IDENTIFIER" = 0x00fd | info<bar_code_scanner>::base_id,
        DECODED_DATA = 0x00fe | info<bar_code_scanner>::base_id,
        DECODE_DATA_CONTINUED = 0x00ff | info<bar_code_scanner>::base_id,
        BAR_SPACE_DATA = 0x0100 | info<bar_code_scanner>::base_id,
        SCANNER_DATA_ACCURACY = 0x0101 | info<bar_code_scanner>::base_id,
        RAW_DATA_POLARITY = 0x0102 | info<bar_code_scanner>::base_id,
        POLARITY_INVERTED_BAR_CODE = 0x0103 | info<bar_code_scanner>::base_id,
        POLARITY_NORMAL_BAR_CODE = 0x0104 | info<bar_code_scanner>::base_id,
        MINIMUM_LENGTH_TO_DECODE = 0x0106 | info<bar_code_scanner>::base_id,
        MAXIMUM_LENGTH_TO_DECODE = 0x0107 | info<bar_code_scanner>::base_id,
        FIRST_DISCRETE_LENGTH_TO_DECODE = 0x0108 | info<bar_code_scanner>::base_id,
        SECOND_DISCRETE_LENGTH_TO_DECODE = 0x0109 | info<bar_code_scanner>::base_id,
        DATA_LENGTH_METHOD = 0x010a | info<bar_code_scanner>::base_id,
        DL_METHOD_READ_ANY = 0x010b | info<bar_code_scanner>::base_id,
        DL_METHOD_CHECK_IN_RANGE = 0x010c | info<bar_code_scanner>::base_id,
        DL_METHOD_CHECK_FOR_DISCRETE = 0x010d | info<bar_code_scanner>::base_id,
        AZTEC_CODE = 0x0110 | info<bar_code_scanner>::base_id,
        BC412 = 0x0111 | info<bar_code_scanner>::base_id,
        CHANNEL_CODE = 0x0112 | info<bar_code_scanner>::base_id,
        CODE_16 = 0x0113 | info<bar_code_scanner>::base_id,
        CODE_32 = 0x0114 | info<bar_code_scanner>::base_id,
        CODE_49 = 0x0115 | info<bar_code_scanner>::base_id,
        CODE_ONE = 0x0116 | info<bar_code_scanner>::base_id,
        COLORCODE = 0x0117 | info<bar_code_scanner>::base_id,
        DATA_MATRIX = 0x0118 | info<bar_code_scanner>::base_id,
        MAXICODE = 0x0119 | info<bar_code_scanner>::base_id,
        MICROPDF = 0x011a | info<bar_code_scanner>::base_id,
        PDF_417 = 0x011b | info<bar_code_scanner>::base_id,
        POSICODE = 0x011c | info<bar_code_scanner>::base_id,
        QR_CODE = 0x011d | info<bar_code_scanner>::base_id,
        SUPERCODE = 0x011e | info<bar_code_scanner>::base_id,
        ULTRACODE = 0x011f | info<bar_code_scanner>::base_id,
        USD_5_SLUG_CODE = 0x0120 | info<bar_code_scanner>::base_id,
        VERICODE = 0x0121 | info<bar_code_scanner>::base_id,
    };
}

#endif // __HID_PAGE_BAR_CODE_SCANNER_H_

