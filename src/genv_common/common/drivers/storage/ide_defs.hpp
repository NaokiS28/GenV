/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * ide_defs.hpp - Created on 23-07-2026
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

/*
    This definition file is following the ATA-2 specifcations.
*/

#pragma once

#include <cstdint>
#include <stdint.h>
#include <stddef.h>

namespace IDE
{
    static constexpr size_t ATA_SECTOR_SIZE = 512;

    enum CS0Register : uint8_t
    {
        CS0_REG_DATA          = 0,
        CS0_REG_ERROR         = 1,
        CS0_REG_FEATURES      = 1,
        CS0_REG_SECTOR_COUNT  = 2,
        CS0_REG_SECTOR_NUMBER = 3,
        CS0_REG_CYLINDER_0    = 4,
        CS0_REG_CYLINDER_1    = 5,
        CS0_REG_BLOCK_ADDR_0  = 2,
        CS0_REG_BLOCK_ADDR_1  = 3,
        CS0_REG_BLOCK_ADDR_2  = 4,
        CS0_REG_BLOCK_ADDR_3  = 5,
        CS0_REG_DRIVE_HEAD    = 6,
        CS0_REG_STATUS        = 7,
        CS0_REG_COMMAND       = 7,
    };

    enum CS1Register : uint8_t
    {
        CS1_REG_CONTROL    = 6,
        CS1_REG_ALT_STATUS = 6,
        CS1_REG_ADDRESS    = 7,
    };

    enum CS0ErrorRegister : uint8_t
    {
        CS0_ERROR_NOT_FOUND        = (1 << 0), // Sector (CHS/LBA) was not found
        CS0_ERROR_RECALIBRATE      = (1 << 1), // Drive could not home and calibrate to track 0 - Fatal, disk is not usable
        CS0_ERROR_ABORTED          = (1 << 2), // The previous command was aborted either because it was illegal or because of a drive error
        CS0_ERROR_MEDIA_CHANGE_REQ = (1 << 3), // User pressed the eject button, host must finish up IO requests
        CS0_ERROR_ID_NOT_FOUND     = (1 << 4), // ID of sector not found - either damaged or the sector doesnt exist
        CS0_ERROR_MEDIA_CHANGED    = (1 << 5), // New medium is present, host should reinit medium
        CS0_ERROR_DATA_ERROR       = (1 << 6), // General read error
        CS0_ERROR_BAD_BLOCK        = (1 << 7), // ATA1: Set if the header block is marked as bad. ATA2+: Reserved
    };

    enum CS0HeadRegister : uint8_t
    {
        // Bit 0-3: CHS:Head number/LBA:Byte 3
        CS0_HEAD_DEVICE_bp = 4, // Select Device 0/1
        CS0_LBA_MODE_bp    = 5, // Use LBA address, else CHS
    };

    inline uint8_t __attribute__((always_inline)) headSel(uint8_t number, bool device, bool lba = true)
    {
        return (number & 0xF) | (device << CS0_HEAD_DEVICE_bp) | (lba << CS0_LBA_MODE_bp);
    }

    enum CS0Status : uint8_t
    {
        CS0_STATUS_ERROR        = (1 << 0), // ATA
        CS0_STATUS_CHECK        = (1 << 0), // Check condition ATAPI
        CS0_STATUS_INDEX        = (1 << 1), // Index mark of drive was reached
        CS0_STATUS_CORRECTED    = (1 << 2), // The data was read from the drive but had to be corrected with ECC
        CS0_STATUS_DATA_REQ     = (1 << 3), //
        CS0_STATUS_SEEK         = (1 << 4), //
        CS0_STATUS_DEVICE_FAULT = (1 << 5),
        CS0_STATUS_DEVICE_READY = (1 << 6),
        CS0_STATUS_DEVICE_BUSY  = (1 << 7),
    };

    enum ControlRegister : uint8_t
    {
        CS0_CONTROL_IRQ_EN        = (0 << 0), // Inverted signal - Set = Disabled, Reset = Enabled
        CS0_CONTROL_IRQ_DIS       = (1 << 0), // Inverted signal - Set = Disabled, Reset = Enabled
        CS0_CONTROL_RESET_ASSERT  = (1 << 1),
        CS0_CONTROL_RESET_RELEASE = (0 << 1),
    };

    // ATA1 only. All of these registers are ACTIVE LOW.
    enum CS0AddressStatus : uint8_t
    {
        CS0_ADDR_STS_DEVICE_0 = (1 << 0),   // Device 0 selected
        CS0_ADDR_STS_DEVICE_1 = (1 << 1),   // Device 1 selected
        CS0_ADDR_STS_HEAD     = (0xF << 2), // Head 0-15 selected
        CS0_ADDR_STS_WRITE    = (1 << 6),   // Write is in progress
    };

    enum CS0DiagnosticResult : uint8_t
    {
        CS0_DIAG_BAD_RESPONSE = 0x00, // 0x00 is an error
        CS0_DIAG_DEVICES_OK   = 0x01,
        CS0_DIAG_DEVICE_1_BAD = 0x02, // Any 4-bit value that is not 0x1 is an error
        CS0_DIAG_DEVICE_2_BAD = 0x80, // If bit 7 is set, device 2 is faulty
    };

    enum ATACommands : uint8_t
    {
        ATA_NOP                     = 0x00,
        ATA_RECALIBRATE             = 0x1F, // Only really used on ATA-1 drives
        ATA_READ_SECTOR             = 0x20,
        ATA_READ_SECTOR_NO_RETRY    = 0x21,
        ATA_READ_LONG               = 0x22,
        ATA_READ_LONG_NO_RETRY      = 0x23,
        ATA_READ_SECTORS_EXT        = 0x24,
        ATA_READ_DMA_EXT            = 0x25,
        ATA_READ_DMA_QUEUED_EXT     = 0x26,
        ATA_WRITE_SECTOR            = 0x30,
        ATA_WRITE_SECTOR_NO_RETRY   = 0x31,
        ATA_WRITE_LONG              = 0x32,
        ATA_WRITE_LONG_NO_RETRY     = 0x33,
        ATA_WRITE_SECTORS_EXT       = 0x34,
        ATA_WRITE_DMA_EXT           = 0x35,
        ATA_WRITE_DMA_QUEUED_EXT    = 0x36,
        ATA_WRITE_SECTORS_VERIFY    = 0x3C,
        ATA_VERIFY_SECTOR           = 0x40,
        ATA_VERIFY_SECTOR_NO_RETRY  = 0x41,
        ATA_FORMAT_TRACK            = 0x50,
        ATA_SEEK                    = 0x70,
        ATA_RUN_DIAGNOSTICS         = 0x90,
        ATA_INIT_DEVICE_PARAMS      = 0x91,
        ATA_DOWNLOAD_FIRMWARE       = 0x92,
        ATA_STANDBY_IMMEDIATE       = 0x94, // Duplicated as IDE has both 0x9x and 0xEx commands
        ATA_IDLE_IMMEDIATE          = 0x95, // Duplicated as IDE has both 0x9x and 0xEx commands
        ATA_STANDBY                 = 0x96, // Duplicated as IDE has both 0x9x and 0xEx commands
        ATA_SET_IDLE_TIMER          = 0x97, // Duplicated as IDE has both 0x9x and 0xEx commands
        ATA_CHECK_POWER_MODE        = 0x98, // Duplicated as IDE has both 0x9x and 0xEx commands
        ATA_SLEEP                   = 0x99, // Duplicated as IDE has both 0x9x and 0xEx commands
        ATA_DEVICE_CONFIG           = 0xB1,
        ATA_ERASE_SECTORS           = 0xc0,
        ATA_READ_MULTIPLE_SECTORS   = 0xC4,
        ATA_WRITE_MULTIPLE_SECTORS  = 0xC5,
        ATA_SET_MULTIPLE_MODE_COUNT = 0xC6,
        ATA_READ_DMA_QUEUED         = 0xC7,
        ATA_READ_DMA_AND_RETRY      = 0xC8,
        ATA_READ_DMA_NO_RETRY       = 0xC9,
        ATA_WRITE_DMA_AND_RETRY     = 0xCA,
        ATA_WRITE_DMA_NO_RETRY      = 0xCB,
        ATA_WRITE_DMA_QUEUED        = 0xCC,
        ATA_ACK_MEDIA_CHANGE        = 0xDB,
        ATA_RUN_POST_BOOT           = 0xDC,
        ATA_RUN_PRE_BOOT            = 0xDD,
        ATA_STANDBY_IMMEDIATE_E     = 0xE0, // Same as the 0x9x version, non-standard but common practice
        ATA_IDLE_IMMEDIATE_E        = 0xE1, // Same as the 0x9x version, non-standard but common practice
        ATA_STANDBY_E               = 0xE2, // Same as the 0x9x version, non-standard but common practice
        ATA_IDLE_E                  = 0xE3, // Same as the 0x9x version, non-standard but common practice
        ATA_READ_FROM_BUFFER        = 0xE4,
        ATA_CHECK_POWER_MODE_E      = 0xE5, // Same as the 0x9x version, non-standard but common practice
        ATA_SLEEP_E                 = 0xE6,
        ATA_REST                    = 0xE7, // ATA-1 command only
        ATA_WRITE_TO_BUFFER         = 0xE8,
        ATA_READ_DRIVE_STATE        = 0xE9, // ATA-1 command only
        ATA_RESTORE_DRIVE_STATE     = 0xE7, // ATA-1 command only
        ATA_IDENTIFY_DEVICE         = 0xEC,
        ATA_MEDIA_EJECT             = 0xED,
        ATA_SET_FEATURES            = 0xEF,
    };

    // Most of the bits in this word only apply to ATA-1 except for the device type
    enum IDConfigBits : uint16_t
    {
        HARD_SECTORED     = (1 << 1),  // ATA-1 ONLY
        SOFT_SECTORED     = (1 << 2),  // ATA-1 ONLY
        NON_MFM_DRIVE     = (1 << 3),  // ATA-1 ONLY
        HEAD_SWITCH_15US  = (1 << 4),  // ATA-1 ONLY
        SPINDLE_CONTROL   = (1 << 5),  // ATA-1 ONLY
        HARD_DISK_DRIVE   = (1 << 6),  // ATA-2
        REMOVABLE_MEDIA   = (1 << 7),  // ATA-2
        DATA_RATE_5MHZ    = (1 << 8),  // ATA-1 ONLY
        DATA_RATE_5_10MHZ = (1 << 9),  // ATA-1 ONLY
        DATA_RATE_10MHZ   = (1 << 10), // ATA-1 ONLY
        MOTOR_TOLERANCE   = (1 << 11), // ATA-1 ONLY
        DATA_CLOCK_OFFSET = (1 << 12), // ATA-1 ONLY
        TRACK_OFFSET      = (1 << 13), // ATA-1 ONLY
        SPEED_TOLERANCE   = (1 << 14), // ATA-1 ONLY
        DRIVE_IS_ATAPI    = (1 << 15),
    };

    enum IDCapabilityBits : uint16_t
    {
        DMA_SUPPORTED           = (1 << 8),
        LBA_SUPPORTED           = (1 << 9),
        IORDY_DEACTIVATABLE     = (1 << 10),
        IORDY_SUPPORTED         = (1 << 11),
        STANDBY_TIMER_SUPPORTED = (1 << 13),
    };

    enum IDParamSupport : uint16_t
    {
        CHS_ID_SUPPORT = (1 << 0),
        PIO_DMA_TIMING = (1 << 1)
    };

    // All of the values in this struct are apparent (ATA-2: Current) values
    struct IDCurrentCHSValues
    {
        uint16_t cylinders;           // Number of cylinders
        uint16_t heads;               // Number of heads
        uint16_t sectorsPerTrack;     // Number of sectors per track
        uint16_t sectorsLo;           // Total number of sectors
        uint16_t sectorsHi;           // Total number of sectors
        uint16_t sectorsPerInterrupt; // Maximum number of sectors that can be transferred per interrupt on read/write multiple commands
    };

    struct IdentifyReply
    {
        IDConfigBits config;           // 0: Mostly obseleted in ATA-2 - Tells host the physical drive config
        uint16_t cylinders;            // 1: Number of logical cylinders
        uint16_t reserved1;            // 2:
        uint16_t heads;                // 3: Number of logical heads
        uint16_t bytesPerTrack;        // 4: Obselete from ATA2
        uint16_t bytesPerSector;       // 5: Obselete from ATA2
        uint16_t sectorsPerTrack;      // 6: Number of logical sectors per logical track
        uint16_t vendor1[3];           // 7-9: Vendor specific
        char serial[10 * 2];           // 10-19: Device's serial number, 20 character ASCII string
        uint16_t bufferType;           // 20: Obselete from ATA2
        uint16_t bufferSize;           // 21: Obselete from ATA2
        uint16_t numECCBytes;          // 22:
        char fwVer[4 * 2];             // 23-26: Device's firmware revision, 8 character ASCII string
        char model[20 * 2];            // 27-46: Device's model name, 40 character ASCII string
        uint16_t sectorsPerInterupt;   // 47: Maximum number of sectors that can be transferred per interrupt on read/write multiple commands
        uint16_t reserved2;            // 48:
        IDCapabilityBits capabilities; // 49: Device's reported capabilities (DMA compatible e.t.c.)
        uint16_t reserved3;            // 50:
        uint16_t PIOTiming;            // 51: 8-15: Timing mode for PIO data transfers
        uint16_t DMATiming;            // 52: 8-15: Timing mode for DMA data transfers
        IDParamSupport paramSupport;   // 53: B0: ID words 54-58 apply, B1: ID words 64-70 apply
        IDCurrentCHSValues currentCHS; // 54-59: Apparent (ATA-2: Current) CHS values
        union
        {
            uint16_t lo;
            uint16_t hi;
            uint32_t u32;
        } LBAsectors;                     // 60-61: Total number of sectors in LBA mode
        uint16_t DMAMode;                 // 62: B7-0: Supported modes for single DMA, B15-8: Current (active) mode for single DMA
        uint16_t multipleDMAMode;         // 63: B7-0: Supported modes for multiple DMA, B15-8: Current (active) mode for multiple DMA
        uint16_t PIOMode;                 // 64: B7-0: Supported modes for advanced PIO
        uint16_t minMultiwordDMATiming;   // 65: Minimum cycle time for multi-word DMA in ns
        uint16_t MultiwordDMATiming;      // 66: Recomended cycle time for multi-word DMA in ns
        uint16_t PIOCycleTimingNoFlow;    // 67: Minimum cycle time for PIO without IORDY in ns
        uint16_t PIOCycleTimingIORDYFlow; // 68: Minimum cycle time for PIO with IORDY in ns
        uint16_t reserved4[59];           // 69-127:
        uint16_t vendor2[32];             // 128-159:
        uint16_t reserved5[96];           // 160-255:
    };

    enum StandbyTimer : uint8_t
    {
        STANDBY_DEACTIVATE = 0x00,
        STANDBY_5S         = 0x01,
        STANDBY_PLUS_30M   = 0xF0,
        STANDBY_VENDOR     = 0xFD,
        STANDBY_RESERVED   = 0xFE,
        STANDBY_21M_15S    = 0xFF,
    };

    enum FeatureOpcode : uint8_t
    {
        ENABLE_8BIT               = 0x01,
        ENABLE_WRITE_CACHE        = 0x02,
        SET_MODE_SECTOR_COUNT     = 0x03,  // ATA-2 and up
        WRITE_SAME_AREA           = 0x022, // ATA-1 ONLY
        DISABLE_RETRIES           = 0x33,
        VENDOR_ECC_LENGTH         = 0x44,
        PLACE_NUM_CACHE_SEG       = 0x54,
        DISABLE_READ_AHEAD        = 0x55,
        KEEP_PARAMS               = 0x66,
        DISABLE_ECC               = 0x77,
        DISABLE_8BIT              = 0x81,
        DISABLE_WRITE_CACHE       = 0x82,
        ENABLE_ECC                = 0x88,
        ENABLE_RETRIES            = 0x99,
        ENABLE_READ_AHEAD         = 0xAA,
        USE_SECTOR_COUNT_FOR_READ = 0xAB,
        ALLOW_REST_MODE           = 0xAC, // ATA-1 ONLY
        ECC_4_BYTES               = 0xBB,
        RESET_LOADS_DEFAULTS      = 0xCC,
        WRITE_TO_ENTIRE_MEDIUM    = 0xDD, // ATA-1 ONLY
    };

    enum MultipleMode : uint8_t
    {
        PIO_MODE        = (0 << 0),
        PIO_NO_IORDY    = (1 << 0),
        PIO_FLOW_CONTRL = (1 << 3),
        SINGLE_DMA      = (1 << 4),
        MULTIPLE_DMA    = (1 << 5),
    };

    struct Sector
    {
        uint8_t data[512];
        uint8_t operator[](uint16_t idx)
        {
            idx &= 0x1FF;
            return data[idx];
        }
    };

    union LBA
    {
        uint8_t u8[8];
        uint16_t u16[4];
        uint32_t u32[2];
        uint64_t u64;
    };

} // namespace IDE