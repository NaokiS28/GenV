/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * errorcodes.h - Created on 01-08-2025
 * 
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <stdint.h>

using ShutdownCode = uint8_t;
enum GENV_Shutdown_Codes : ShutdownCode {
    GENV_SHUTDOWN_GRACEFUL,     // Gracefully shutdown
    GENV_SHUTDOWN_WATCHDOG,     // Watchdog kick caused system to restart
    GENV_SHUTDOWN_FATAL_ERROR,
};

using ErrorCode = uint16_t;

/*
 * GenV Default Error Codes
 * These are the defaults that should be used first unless a game requires a code that is not in this list.
 * If a game must provide it's own error code, then the game MUST use the range of 0x0C00-0x0CFF.
*/
enum GENV_Error_Codes : ErrorCode {
    // -------------------------------
    // 0x00xx General / System
    // -------------------------------
    GENV_NONE                            = 0x0000,
    GENV_ERR_UNKNOWN                     = 0xFFFF,
    GENV_ERR_STARTUP_SELFTEST_FAILED     = 0x0001,
    GENV_ERR_WATCHDOG_RESET              = 0x0002,
    GENV_ERR_FIRMWARE_MISMATCH           = 0x0003,
    GENV_ERR_CONFIG_INVALID              = 0x0004,
    GENV_ERR_CALIBRATION_REQUIRED        = 0x0005,
    GENV_ERR_RESOURCES_UNAVAILABLE       = 0x0006,

    // -------------------------------
    // 0x01xx Inputs & Controls
    // -------------------------------
    GENV_ERR_BTN_STUCK                   = 0x0101,
    GENV_ERR_JOYSTICK_CENTER_FAULT       = 0x0102,
    GENV_ERR_TOUCHSCREEN_DISCONNECTED    = 0x0103,
    GENV_ERR_TOUCHSCREEN_UNCALIBRATED    = 0x0104,
    GENV_ERR_TICKET_SWITCH_STUCK         = 0x0105,
    GENV_ERR_SAFETY_INTERLOCK_OPEN       = 0x0106,

    // -------------------------------
    // 0x02xx Payments (Coin/Bill/Card/MDB)
    // -------------------------------
    GENV_ERR_COIN_MECH_DISCONNECTED      = 0x0201,
    GENV_ERR_COIN_MECH_JAM               = 0x0202,
    GENV_ERR_COIN_MECH_TUBE_EMPTY        = 0x0203,  // coin change tubes
    GENV_ERR_NOTE_ACCEPTOR_DISCONNECTED  = 0x0204,
    GENV_ERR_NOTE_ACCEPTOR_JAM           = 0x0205,
    GENV_ERR_NOTE_ACCEPTOR_STACKER_FULL  = 0x0206,
    GENV_ERR_NOTE_RECYCLER_EMPTY         = 0x0207,  // SWP common
    GENV_ERR_CARD_READER_DISCONNECTED    = 0x0208,
    GENV_ERR_CARD_READER_FAIL            = 0x0209,
    GENV_ERR_MDB_BUS_FAULT               = 0x020A,  // multi-drop bus
    GENV_ERR_PAYMENT_TIMEOUT             = 0x020B,

    // -------------------------------
    // 0x03xx Payout (Hoppers/Dispensers) — SWP-focused
    // -------------------------------
#ifdef GENV_GAME_SWP
    GENV_ERR_PAYOUT_DISABLED             = 0x0300,  // global lockout
    GENV_ERR_HOPPER_EMPTY                = 0x0301,
    GENV_ERR_HOPPER_JAM                  = 0x0302,
    GENV_ERR_HOPPER_SENSOR_FAIL          = 0x0303,
    GENV_ERR_PAYOUT_OVERPAY_DETECTED     = 0x0304,
    GENV_ERR_PAYOUT_UNDERPAY_DETECTED    = 0x0305,
    GENV_ERR_PAYOUT_MOTOR_FAULT          = 0x0306,
    GENV_ERR_TICKET_DISPENSER_JAM        = 0x030A,
    GENV_ERR_TICKET_DISPENSER_EMPTY      = 0x030B,
    GENV_ERR_PRIZE_DOOR_BLOCKED          = 0x030C,  // prize vend door
    GENV_ERR_PRIZE_MOTOR_STALL           = 0x030D,  // claw/spiral/gantry
    GENV_ERR_FLOAT_TOO_LOW               = 0x030E,  // insufficient payout float
    GENV_ERR_PAYOUT_AUDIT_MISMATCH       = 0x030F,  // meter vs. physical
#endif
    // -------------------------------
    // 0x04xx Printers & Media
    // -------------------------------
#ifdef GENV_GAME_PRINTER
    GENV_ERR_PRINTER_DISCONNECTED        = 0x0401,
    GENV_ERR_PRINTER_OUT_OF_PAPER        = 0x0402,
    GENV_ERR_PRINTER_COVER_OPEN          = 0x0403,
    GENV_ERR_PRINTER_OVERHEAT            = 0x0404,
    GENV_ERR_PRINTER_CUTTER_JAM          = 0x0405,
    GENV_ERR_QR_ENCODER_FAIL             = 0x0406,
#endif
    // -------------------------------
    // 0x05xx Communications & I/O
    // -------------------------------
    GENV_ERR_MAIN_IO_BOARD_OFFLINE       = 0x0501,
    GENV_ERR_AUX_IO_BOARD_OFFLINE        = 0x0502,
    GENV_ERR_CAN_BUS_FAULT               = 0x0503,
    GENV_ERR_RS485_COMM_FAIL             = 0x0504,
    GENV_ERR_ETHERNET_LINK_DOWN          = 0x0505,
    GENV_ERR_SERVER_UNREACHABLE          = 0x0506,  // back office / telemetry
    GENV_ERR_TIME_SYNC_INVALID           = 0x0507,  // NTP/GPS mismatch
    GENV_ERR_FIRMWARE_UPDATE_REQUIRED    = 0x0508,
    GENV_ERR_FIRMWARE_UPDATE_FAILED      = 0x0509,

    // -------------------------------
    // 0x06xx Security & Cabinet
    // -------------------------------
    GENV_ERR_MAIN_DOOR_OPEN              = 0x0601,
    GENV_ERR_CASHBOX_DOOR_OPEN           = 0x0602,
    GENV_ERR_HOPPER_DOOR_OPEN            = 0x0603,
    GENV_ERR_TILT_DETECTED               = 0x0604,
    GENV_ERR_VIBRATION_TAMPER            = 0x0605,
    GENV_ERR_COINBOX_REMOVED             = 0x0606,
    GENV_ERR_STACKER_REMOVED             = 0x0607,

    // -------------------------------
    // 0x07xx Environment & Power
    // -------------------------------
#ifdef GENV_HW_MONITORING
    GENV_ERR_OVER_TEMPERATURE            = 0x0701,
    GENV_ERR_UNDER_TEMPERATURE           = 0x0702,
    GENV_ERR_FAN_FAILURE                 = 0x0703,
    GENV_ERR_PSU_12V_OUT_OF_RANGE        = 0x0704,
    GENV_ERR_PSU_5V_OUT_OF_RANGE         = 0x0705,
    GENV_ERR_PSU_3V3_OUT_OF_RANGE        = 0x0706,
    GENV_ERR_CABINET_LIGHTING_FAULT      = 0x0707,
#endif

    // -------------------------------
    // 0x08xx Storage & NVRAM
    // -------------------------------
    GENV_ERR_NVRAM_CORRUPT               = 0x0801,
    GENV_ERR_NVRAM_BATTERY_LOW           = 0x0802,
    GENV_ERR_SD_MISSING                  = 0x0803,
    GENV_ERR_SD_READ_WRITE_FAIL          = 0x0804,
    GENV_ERR_CF_MISSING                  = 0x0810,
    GENV_ERR_CF_READ_WRITE_FAIL          = 0x0811,
    GENV_ERR_CD_MISSING                  = 0x0820,
    GENV_ERR_CD_READ_WRITE_FAIL          = 0x0821,
    GENV_ERR_HDD_MISSING                 = 0x0830,
    GENV_ERR_HDD_READ_WRITE_FAIL         = 0x0831,
    GENV_ERR_FLASH_MISSING               = 0x0840,
    GENV_ERR_FLASH_READ_WRITE_FAIL       = 0x0841,
    GENV_ERR_LOG_STORAGE_FULL            = 0x08A0,

    // -------------------------------
    // 0x09xx Game/App Logic
    // -------------------------------
    GENV_ERR_LICENSE_EXPIRED             = 0x0901,
    GENV_ERR_CREDIT_METER_MISMATCH       = 0x0902,
    GENV_ERR_SESSION_STUCK               = 0x0903,
    GENV_ERR_RULES_CONFIG_INVALID        = 0x0904,  // SWP rule-set invalid
    GENV_ERR_MAX_PAYOUT_LIMIT_REACHED    = 0x0905,  // SWP compliance
    GENV_ERR_GAME_ASSET_MISSING          = 0x0906,

    // -------------------------------
    // 0x0Cxx Game specific
    // -------------------------------
    GENV_ERR_GAME_SPECIFIC              = 0x0C00,
};

#ifndef GENV_NO_ERROR_STRINGS
#include <string_view>

constexpr const char *GENV_ErrorCode_String(ErrorCode code) noexcept {
    switch (code) {
        // 0x00xx General / System
        case GENV_NONE:                           return "";
        case GENV_ERR_UNKNOWN:                    return "Unknown error";
        case GENV_ERR_STARTUP_SELFTEST_FAILED:    return "Self-test failed (0x%x)";
        case GENV_ERR_WATCHDOG_RESET:             return "Watchdog reset";
        case GENV_ERR_FIRMWARE_MISMATCH:          return "Firmware mismatch (%u, %u)";
        case GENV_ERR_CONFIG_INVALID:             return "Invalid config";
        case GENV_ERR_CALIBRATION_REQUIRED:       return "Calibration required";
        case GENV_ERR_RESOURCES_UNAVAILABLE:      return "Resources unavailable";

        // 0x01xx Inputs & Controls
        case GENV_ERR_BTN_STUCK:                  return "Button stuck (%u, %u)";
        case GENV_ERR_JOYSTICK_CENTER_FAULT:      return "Joystick center fault (%u)";
        case GENV_ERR_TOUCHSCREEN_DISCONNECTED:   return "Touchscreen disconnected";
        case GENV_ERR_TOUCHSCREEN_UNCALIBRATED:   return "Touchscreen uncalibrated";
        case GENV_ERR_TICKET_SWITCH_STUCK:        return "Ticket switch stuck (%u)";
        case GENV_ERR_SAFETY_INTERLOCK_OPEN:      return "Safety interlock open (%u)";

        // 0x02xx Payments (Coin/Bill/Card/MDB)
        case GENV_ERR_COIN_MECH_DISCONNECTED:     return "Coin mech disconnected (%u)";
        case GENV_ERR_COIN_MECH_JAM:              return "Coin mech jam (%u)";
        case GENV_ERR_COIN_MECH_TUBE_EMPTY:       return "Coin tube empty (%u)";
        case GENV_ERR_NOTE_ACCEPTOR_DISCONNECTED: return "Note acceptor disconnected";
        case GENV_ERR_NOTE_ACCEPTOR_JAM:          return "Note acceptor jam";
        case GENV_ERR_NOTE_ACCEPTOR_STACKER_FULL: return "Stacker full";
        case GENV_ERR_NOTE_RECYCLER_EMPTY:        return "Note recycler empty";
        case GENV_ERR_CARD_READER_DISCONNECTED:   return "Card reader disconnected (%u)";
        case GENV_ERR_CARD_READER_FAIL:           return "Card reader failure (%u)";
        case GENV_ERR_MDB_BUS_FAULT:              return "MDB bus fault (0x%X)";
        case GENV_ERR_PAYMENT_TIMEOUT:            return "Payment timeout";
    
    #ifdef GENV_GAME_SWP
        // 0x03xx Payout (Hoppers/Dispensers) — SWP
        case GENV_ERR_PAYOUT_DISABLED:            return "Payout disabled";
        case GENV_ERR_HOPPER_EMPTY:               return "Hopper %u empty";
        case GENV_ERR_HOPPER_JAM:                 return "Hopper %u jam";
        case GENV_ERR_HOPPER_SENSOR_FAIL:         return "Hopper %u sensor fail";
        case GENV_ERR_PAYOUT_OVERPAY_DETECTED:    return "Overpay detected (%u)";
        case GENV_ERR_PAYOUT_UNDERPAY_DETECTED:   return "Underpay detected (%u)";
        case GENV_ERR_PAYOUT_MOTOR_FAULT:         return "Payout motor fault";
        case GENV_ERR_TICKET_DISPENSER_JAM:       return "Ticket dispenser %u jam ";
        case GENV_ERR_TICKET_DISPENSER_EMPTY:     return "Ticket dispenser %u empty";
        case GENV_ERR_PRIZE_DOOR_BLOCKED:         return "Prize door blocked";
        case GENV_ERR_PRIZE_MOTOR_STALL:          return "Prize motor stall";
        case GENV_ERR_FLOAT_TOO_LOW:              return "Float too low";
        case GENV_ERR_PAYOUT_AUDIT_MISMATCH:      return "Payout audit mismatch";
    #endif

    #ifdef GENV_GAME_PRINTER
        // 0x04xx Printers & Media
        case GENV_ERR_PRINTER_DISCONNECTED:       return "Printer disconnected";
        case GENV_ERR_PRINTER_OUT_OF_PAPER:       return "Printer out of paper";
        case GENV_ERR_PRINTER_COVER_OPEN:         return "Printer cover open";
        case GENV_ERR_PRINTER_OVERHEAT:           return "Printer overheat";
        case GENV_ERR_PRINTER_CUTTER_JAM:         return "Printer cutter jam";
        case GENV_ERR_QR_ENCODER_FAIL:            return "QR encoder failure";
    #endif

        // 0x05xx Communications & I/O
        case GENV_ERR_MAIN_IO_BOARD_OFFLINE:      return "Main I/O board missing";
        case GENV_ERR_AUX_IO_BOARD_OFFLINE:       return "Aux I/O board missing";
        case GENV_ERR_CAN_BUS_FAULT:              return "CAN bus fault";
        case GENV_ERR_RS485_COMM_FAIL:            return "RS-485 comm failure";
        case GENV_ERR_ETHERNET_LINK_DOWN:         return "Ethernet link down";
        case GENV_ERR_SERVER_UNREACHABLE:         return "Server unreachable";
        case GENV_ERR_TIME_SYNC_INVALID:          return "Time sync invalid";
        case GENV_ERR_FIRMWARE_UPDATE_REQUIRED:   return "Firmware update required (V: %f)";
        case GENV_ERR_FIRMWARE_UPDATE_FAILED:     return "Firmware update failed";

        // 0x06xx Security & Cabinet
        case GENV_ERR_MAIN_DOOR_OPEN:             return "Main door open";
        case GENV_ERR_CASHBOX_DOOR_OPEN:          return "Cashbox door open";
        case GENV_ERR_HOPPER_DOOR_OPEN:           return "Hopper door open";
        case GENV_ERR_TILT_DETECTED:              return "Tilt detected";
        case GENV_ERR_VIBRATION_TAMPER:           return "Vibration tamper";
        case GENV_ERR_COINBOX_REMOVED:            return "Coin box removed";
        case GENV_ERR_STACKER_REMOVED:            return "Stacker removed";

        // 0x07xx Environment & Power
#ifdef GENV_HW_MONITORING
        case GENV_ERR_OVER_TEMPERATURE:           return "Over temperature (%u)";
        case GENV_ERR_FAN_FAILURE:                return "Fan failure (%u)";
        case GENV_ERR_PSU_12V_OUT_OF_RANGE:       return "PSU 12V out of range";
        case GENV_ERR_PSU_5V_OUT_OF_RANGE:        return "PSU 5V out of range";
        case GENV_ERR_PSU_3V3_OUT_OF_RANGE:       return "PSU 3.3V out of range";
        case GENV_ERR_CABINET_LIGHTING_FAULT:     return "Cabinet lighting fault (0x%X)";
#endif

        // 0x08xx Storage & NVRAM
        case GENV_ERR_NVRAM_CORRUPT:              return "NVRAM corrupt";
        case GENV_ERR_NVRAM_BATTERY_LOW:          return "NVRAM battery low";
        case GENV_ERR_SD_MISSING:                 return "SD%u card missing";
        case GENV_ERR_CD_MISSING:                 return "CD%u missing";
        case GENV_ERR_CF_MISSING:                 return "CF%u card missing";
        case GENV_ERR_HDD_MISSING:                return "HDD%u missing";
        case GENV_ERR_FLASH_MISSING:              return "Flash device missing (%u)";
        case GENV_ERR_SD_READ_WRITE_FAIL:         return "SD%u read/write fail (0x%X)";
        case GENV_ERR_CF_READ_WRITE_FAIL:         return "CF%u read/write fail (0x%X)";
        case GENV_ERR_CD_READ_WRITE_FAIL:         return "CD%u read/write fail (0x%X)";
        case GENV_ERR_HDD_READ_WRITE_FAIL:        return "HDD%u read/write fail (0x%X)";
        case GENV_ERR_FLASH_READ_WRITE_FAIL:      return "Flash device %u read/write fail (0x%X)";
        case GENV_ERR_LOG_STORAGE_FULL:           return "Log storage full";

        // 0x09xx Game/App Logic
        case GENV_ERR_LICENSE_EXPIRED:            return "License expired";
        case GENV_ERR_CREDIT_METER_MISMATCH:      return "Credit meter mismatch (E:%u, R:%u)";
        case GENV_ERR_SESSION_STUCK:              return "Session stuck";
        case GENV_ERR_RULES_CONFIG_INVALID:       return "Rules config invalid";
        case GENV_ERR_MAX_PAYOUT_LIMIT_REACHED:   return "Max payout limit reached";
        case GENV_ERR_GAME_ASSET_MISSING:         return "Game asset missing (%s)";
    }
    return "Unknown error";
}
#endif