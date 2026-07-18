/*
 * ps1-bare-metal - (C) 2023-2025 spicyjpeg
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#pragma once

#include "common/util/enum_defs.hpp"
#include <stdint.h>

#define DEF(type) static inline type __attribute__((always_inline))

namespace PS1::CDROM
{

    /* CD-ROM drive */

    enum class HSTSFlag : uint8_t
    {
        RA_BITMASK = 3 << 0,
        ADPBUSY    = 1 << 2,
        PRMEMPT    = 1 << 3,
        PRMWRDY    = 1 << 4,
        RSLRRDY    = 1 << 5,
        DRQSTS     = 1 << 6,
        BUSYSTS    = 1 << 7
    };
    ENABLE_BITWISE_OPS(HSTSFlag);

    enum class HINTFlag : uint8_t
    {
        INT0   = 1 << 0,
        INT1   = 1 << 1,
        INT2   = 1 << 2,
        BFEMPT = 1 << 3,
        BFWRDY = 1 << 4
    };
    ENABLE_BITWISE_OPS(HINTFlag);

    constexpr uint8_t CDROM_HINT_INT_BITMASK = 7 << 0;

    enum class HCHPCTLFlag : uint8_t
    {
        SMEN = 1 << 5,
        BFWR = 1 << 6,
        BFRD = 1 << 7
    };
    ENABLE_BITWISE_OPS(HCHPCTLFlag);

    enum class HCLRCTLFlag : uint8_t
    {
        CLRINT_BITMASK = 7 << 0,
        CLRINT0        = 1 << 0,
        CLRINT1        = 1 << 1,
        CLRINT2        = 1 << 2,
        CLRBFEMPT      = 1 << 3,
        CLRBFWRDY      = 1 << 4,
        SMADPCLR       = 1 << 5,
        CLRPRM         = 1 << 6,
        CHPRST         = 1 << 7
    };
    ENABLE_BITWISE_OPS(HCLRCTLFlag);

    enum class CIFlag : uint8_t
    {
        SM       = 1 << 0,
        FS       = 1 << 2,
        BITLNGTH = 1 << 4,
        EMPHASIS = 1 << 6
    };
    ENABLE_BITWISE_OPS(CIFlag);

    enum class ADPCTLFlag : uint8_t
    {
        ADPMUTE = 1 << 0,
        CHNGATV = 1 << 5
    };

    ENABLE_BITWISE_OPS(ADPCTLFlag);

#define CDROM_HSTS _MMIO8(IO_BASE | 0x800)      // All banks
#define CDROM_RESULT _MMIO8(IO_BASE | 0x801)    // All banks
#define CDROM_RDDATA _MMIO8(IO_BASE | 0x802)    // All banks
#define CDROM_HINTMSK_R _MMIO8(IO_BASE | 0x803) // Bank 0
#define CDROM_HINTSTS _MMIO8(IO_BASE | 0x803)   // Bank 1

#define CDROM_ADDRESS _MMIO8(IO_BASE | 0x800)   // All banks
#define CDROM_COMMAND _MMIO8(IO_BASE | 0x801)   // Bank 0
#define CDROM_PARAMETER _MMIO8(IO_BASE | 0x802) // Bank 0
#define CDROM_HCHPCTL _MMIO8(IO_BASE | 0x803)   // Bank 0
#define CDROM_WRDATA _MMIO8(IO_BASE | 0x801)    // Bank 1
#define CDROM_HINTMSK_W _MMIO8(IO_BASE | 0x802) // Bank 1
#define CDROM_HCLRCTL _MMIO8(IO_BASE | 0x803)   // Bank 1
#define CDROM_CI _MMIO8(IO_BASE | 0x801)        // Bank 2
#define CDROM_ATV0 _MMIO8(IO_BASE | 0x802)      // Bank 2
#define CDROM_ATV1 _MMIO8(IO_BASE | 0x803)      // Bank 2
#define CDROM_ATV2 _MMIO8(IO_BASE | 0x801)      // Bank 3
#define CDROM_ATV3 _MMIO8(IO_BASE | 0x802)      // Bank 3
#define CDROM_ADPCTL _MMIO8(IO_BASE | 0x803)    // Bank 3

    struct __attribute__((packed)) XAHeader
    {
        uint8_t file, channel, submode, codingInfo;
    };

    enum class XASubmode : uint8_t
    {
        END_OF_RECORD = 1 << 0,
        TYPE_VIDEO    = 1 << 1,
        TYPE_AUDIO    = 1 << 2,
        TYPE_DATA     = 1 << 3,
        TRIGGER       = 1 << 4,
        FORM2         = 1 << 5,
        REAL_TIME     = 1 << 6,
        END_OF_FILE   = 1 << 7
    };

    ENABLE_BITWISE_OPS(XASubmode);

    enum class XACodingInfo : uint8_t
    {
        STEREO              = 1 << 0,
        SAMPLE_RATE_BITMASK = 1 << 2,
        SAMPLE_RATE_18900   = 0 << 2,
        SAMPLE_RATE_37800   = 1 << 2,
        BITS_BITMASK        = 1 << 4,
        BITS_4              = 0 << 4,
        BITS_8              = 1 << 4,
        EMPHASIS            = 1 << 6
    };

    ENABLE_BITWISE_OPS(XACodingInfo);

    /* CD-ROM drive data types */

    struct __attribute__((packed)) MSF
    {
        uint8_t minute, second, frame;
    };

    struct __attribute__((packed)) GetlocLResult
    {
        MSF absoluteMSF;
        uint8_t mode;
        XAHeader header;
    };

    struct __attribute__((packed)) GetlocPResult
    {
        uint8_t track, index;
        MSF relativeMSF, absoluteMSF;
    };

    struct __attribute__((packed)) GetIDResult
    {
        uint8_t status, flag, type, atip;
        char license[4];
    };

    struct __attribute__((packed)) ReportPacket
    {
        uint8_t status, track, index;
        MSF msf;
        uint16_t peak;
    };

    DEF(uint8_t)
    encodeBCD(uint8_t value)
    {
        // output = units + tens * 16
        //        = units + tens * 10 + tens * 6
        //        = value             + tens * 6
        return value + (value / 10) * 6;
    }

    DEF(uint8_t)
    decodeBCD(uint8_t value)
    {
        // output = low + high * 10
        //        = low + high * 16 - high * 6
        //        = value           - high * 6
        return value - (value >> 4) * 6;
    }

    DEF(void)
    convertLBAToMSF(MSF *msf, unsigned int lba)
    {
        lba += 150; // Skip lead-in area (LBA 0 is always at 00:02:00)

        msf->minute = encodeBCD(lba / (75 * 60));
        msf->second = encodeBCD((lba / 75) % 60);
        msf->frame  = encodeBCD(lba % 75);
    }

    DEF(unsigned int)
    convertMSFToLBA(const MSF *msf)
    {
        return 0 + decodeBCD(msf->minute) * (75 * 60) + decodeBCD(msf->second) * 75 + decodeBCD(msf->frame) - 150;
    }

    /* CD-ROM drive command and status definitions */

    enum Command : uint8_t
    {
        CMD_NOP        = 0x01,
        CMD_SETLOC     = 0x02,
        CMD_PLAY       = 0x03,
        CMD_FORWARD    = 0x04,
        CMD_BACKWARD   = 0x05,
        CMD_READ_N     = 0x06,
        CMD_STANDBY    = 0x07,
        CMD_STOP       = 0x08,
        CMD_PAUSE      = 0x09,
        CMD_INIT       = 0x0a,
        CMD_MUTE       = 0x0b,
        CMD_DEMUTE     = 0x0c,
        CMD_SETFILTER  = 0x0d,
        CMD_SETMODE    = 0x0e,
        CMD_GETPARAM   = 0x0f,
        CMD_GETLOC_L   = 0x10,
        CMD_GETLOC_P   = 0x11,
        CMD_SETSESSION = 0x12,
        CMD_GET_TN     = 0x13,
        CMD_GET_TD     = 0x14,
        CMD_SEEK_L     = 0x15,
        CMD_SEEK_P     = 0x16,
        CMD_TEST       = 0x19,
        CMD_GET_ID     = 0x1a,
        CMD_READ_S     = 0x1b,
        CMD_RESET      = 0x1c,
        CMD_GET_Q      = 0x1d, // Versions 0xc1 and later only
        CMD_READ_TOC   = 0x1e, // Versions 0xc1 and later only
        CMD_UNLOCK0    = 0x50, // Versions 0xc1 and later only
        CMD_UNLOCK1    = 0x51, // Versions 0xc1 and later only
        CMD_UNLOCK2    = 0x52, // Versions 0xc1 and later only
        CMD_UNLOCK3    = 0x53, // Versions 0xc1 and later only
        CMD_UNLOCK4    = 0x54, // Versions 0xc1 and later only
        CMD_UNLOCK5    = 0x55, // Versions 0xc1 and later only
        CMD_UNLOCK6    = 0x56, // Versions 0xc1 and later only
        CMD_LOCK       = 0x57  // Versions 0xc1 and later only
    };

    enum TestCommand : uint8_t
    {
        TEST_READ_ID              = 0x04,
        TEST_GET_ID_COUNTERS      = 0x05,
        TEST_GET_VERSION          = 0x20,
        TEST_GET_SWITCHES         = 0x21,
        TEST_GET_REGION           = 0x22, // Versions 0xc1 and later only
        TEST_GET_SERVO_TYPE       = 0x23, // Versions 0xc1 and later only
        TEST_GET_DSP_TYPE         = 0x24, // Versions 0xc1 and later only
        TEST_GET_DECODER_TYPE     = 0x25, // Versions 0xc1 and later only
        TEST_DSP_CMD              = 0x50,
        TEST_DSP_CMD_RESP         = 0x51, // Versions 0xc2 and later only
        TEST_MCU_PEEK             = 0x60,
        TEST_DECODER_GET_REG      = 0x71, // Versions 0xc1 and later only
        TEST_DECODER_SET_REG      = 0x72, // Versions 0xc1 and later only
        TEST_DECODER_GET_SRAM_PTR = 0x75, // Versions 0xc1 and later only
        TEST_DECODER_SET_SRAM_PTR = 0x76  // Versions 0xc1 and later only
    };

    enum IRQType : uint8_t
    {
        IRQ_NONE        = 0,
        IRQ_DATA_READY  = 1,
        IRQ_COMPLETE    = 2,
        IRQ_ACKNOWLEDGE = 3,
        IRQ_DATA_END    = 4,
        IRQ_ERROR       = 5
    };

    enum CommandStatusFlag : uint8_t
    {
        STATUS_ERROR      = 1 << 0,
        STATUS_SPINDLE_ON = 1 << 1,
        STATUS_SEEK_ERROR = 1 << 2,
        STATUS_ID_ERROR   = 1 << 3,
        STATUS_LID_OPEN   = 1 << 4,
        STATUS_READING    = 1 << 5,
        STATUS_SEEKING    = 1 << 6,
        STATUS_PLAYING    = 1 << 7
    };

    enum CommandErrorFlag : uint8_t
    {
        ERROR_SEEK_FAILED         = 1 << 2,
        ERROR_LID_OPENED          = 1 << 3,
        ERROR_INVALID_PARAM_VALUE = 1 << 4,
        ERROR_INVALID_PARAM_COUNT = 1 << 5,
        ERROR_INVALID_COMMAND     = 1 << 6,
        ERROR_NO_DISC             = 1 << 7
    };

    enum ModeFlag : uint8_t
    {
        MODE_CDDA         = 1 << 0,
        MODE_AUTO_PAUSE   = 1 << 1,
        MODE_CDDA_REPORT  = 1 << 2,
        MODE_XA_FILTER    = 1 << 3,
        MODE_SIZE_BITMASK = 3 << 4,
        MODE_SIZE_2048    = 0 << 4,
        MODE_SIZE_2340    = 2 << 4,
        MODE_XA_ADPCM     = 1 << 6,
        MODE_SPEED_1X     = 0 << 7,
        MODE_SPEED_2X     = 1 << 7
    };
    ENABLE_BITWISE_OPS(ModeFlag);

} // namespace PS1::CDROM

#undef DEF
