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

#include <stdint.h>

#define DEF(type) static inline type __attribute__((always_inline))

namespace PSX::CDROM
{

    /* CD-ROM drive */

    enum HSTSFlag : uint8_t
    {
        HSTS_RA_BITMASK = 3 << 0,
        HSTS_ADPBUSY    = 1 << 2,
        HSTS_PRMEMPT    = 1 << 3,
        HSTS_PRMWRDY    = 1 << 4,
        HSTS_RSLRRDY    = 1 << 5,
        HSTS_DRQSTS     = 1 << 6,
        HSTS_BUSYSTS    = 1 << 7
    };

    enum HINTFlag : uint8_t
    {
        HINT_INT_BITMASK = 7 << 0,
        HINT_INT0        = 1 << 0,
        HINT_INT1        = 1 << 1,
        HINT_INT2        = 1 << 2,
        HINT_BFEMPT      = 1 << 3,
        HINT_BFWRDY      = 1 << 4
    };

    enum HCHPCTLFlag : uint8_t
    {
        HCHPCTL_SMEN = 1 << 5,
        HCHPCTL_BFWR = 1 << 6,
        HCHPCTL_BFRD = 1 << 7
    };

    enum HCLRCTLFlag : uint8_t
    {
        HCLRCTL_CLRINT_BITMASK = 7 << 0,
        HCLRCTL_CLRINT0        = 1 << 0,
        HCLRCTL_CLRINT1        = 1 << 1,
        HCLRCTL_CLRINT2        = 1 << 2,
        HCLRCTL_CLRBFEMPT      = 1 << 3,
        HCLRCTL_CLRBFWRDY      = 1 << 4,
        HCLRCTL_SMADPCLR       = 1 << 5,
        HCLRCTL_CLRPRM         = 1 << 6,
        HCLRCTL_CHPRST         = 1 << 7
    };

    enum CIFlag : uint8_t
    {
        CI_SM       = 1 << 0,
        CI_FS       = 1 << 2,
        CI_BITLNGTH = 1 << 4,
        CI_EMPHASIS = 1 << 6
    };

    enum ADPCTLFlag : uint8_t
    {
        ADPCTL_ADPMUTE = 1 << 0,
        ADPCTL_CHNGATV = 1 << 5
    };

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

    enum XASubmode : uint8_t
    {
        XA_SUB_END_OF_RECORD = 1 << 0,
        XA_SUB_TYPE_VIDEO    = 1 << 1,
        XA_SUB_TYPE_AUDIO    = 1 << 2,
        XA_SUB_TYPE_DATA     = 1 << 3,
        XA_SUB_TRIGGER       = 1 << 4,
        XA_SUB_FORM2         = 1 << 5,
        XA_SUB_REAL_TIME     = 1 << 6,
        XA_SUB_END_OF_FILE   = 1 << 7
    };

    enum XACodingInfo : uint8_t
    {
        XA_CI_STEREO              = 1 << 0,
        XA_CI_SAMPLE_RATE_BITMASK = 1 << 2,
        XA_CI_SAMPLE_RATE_18900   = 0 << 2,
        XA_CI_SAMPLE_RATE_37800   = 1 << 2,
        XA_CI_BITS_BITMASK        = 1 << 4,
        XA_CI_BITS_4              = 0 << 4,
        XA_CI_BITS_8              = 1 << 4,
        XA_CI_EMPHASIS            = 1 << 6
    };

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

    enum class Command : uint8_t
    { // Command codes are identities, not bitmasks — enum class is intentional
        NOP        = 0x01,
        SETLOC     = 0x02,
        PLAY       = 0x03,
        FORWARD    = 0x04,
        BACKWARD   = 0x05,
        READ_N     = 0x06,
        STANDBY    = 0x07,
        STOP       = 0x08,
        PAUSE      = 0x09,
        INIT       = 0x0a,
        MUTE       = 0x0b,
        DEMUTE     = 0x0c,
        SETFILTER  = 0x0d,
        SETMODE    = 0x0e,
        GETPARAM   = 0x0f,
        GETLOC_L   = 0x10,
        GETLOC_P   = 0x11,
        SETSESSION = 0x12,
        GET_TN     = 0x13,
        GET_TD     = 0x14,
        SEEK_L     = 0x15,
        SEEK_P     = 0x16,
        TEST       = 0x19,
        GET_ID     = 0x1a,
        READ_S     = 0x1b,
        RESET      = 0x1c,
        GET_Q      = 0x1d, // Versions 0xc1 and later only
        READ_TOC   = 0x1e, // Versions 0xc1 and later only
        UNLOCK0    = 0x50, // Versions 0xc1 and later only
        UNLOCK1    = 0x51, // Versions 0xc1 and later only
        UNLOCK2    = 0x52, // Versions 0xc1 and later only
        UNLOCK3    = 0x53, // Versions 0xc1 and later only
        UNLOCK4    = 0x54, // Versions 0xc1 and later only
        UNLOCK5    = 0x55, // Versions 0xc1 and later only
        UNLOCK6    = 0x56, // Versions 0xc1 and later only
        LOCK       = 0x57  // Versions 0xc1 and later only
    };

    enum class TestCommand : uint8_t
    { // Command codes are identities, not bitmasks — enum class is intentional
        READ_ID              = 0x04,
        GET_ID_COUNTERS      = 0x05,
        GET_VERSION          = 0x20,
        GET_SWITCHES         = 0x21,
        GET_REGION           = 0x22, // Versions 0xc1 and later only
        GET_SERVO_TYPE       = 0x23, // Versions 0xc1 and later only
        GET_DSP_TYPE         = 0x24, // Versions 0xc1 and later only
        GET_DECODER_TYPE     = 0x25, // Versions 0xc1 and later only
        DSP_CMD              = 0x50,
        DSP_CMD_RESP         = 0x51, // Versions 0xc2 and later only
        MCU_PEEK             = 0x60,
        DECODER_GET_REG      = 0x71, // Versions 0xc1 and later only
        DECODER_SET_REG      = 0x72, // Versions 0xc1 and later only
        DECODER_GET_SRAM_PTR = 0x75, // Versions 0xc1 and later only
        DECODER_SET_SRAM_PTR = 0x76  // Versions 0xc1 and later only
    };

    enum class IRQType : uint8_t
    { // IRQ types are identities, not bitmasks — enum class is intentional
        NONE        = 0,
        DATA_READY  = 1,
        COMPLETE    = 2,
        ACKNOWLEDGE = 3,
        DATA_END    = 4,
        ERROR       = 5
    };

    enum CommandStatusFlag : uint8_t
    {
        CDSTAT_ERROR      = 1 << 0,
        CDSTAT_SPINDLE_ON = 1 << 1,
        CDSTAT_SEEK_ERROR = 1 << 2,
        CDSTAT_ID_ERROR   = 1 << 3,
        CDSTAT_LID_OPEN   = 1 << 4,
        CDSTAT_READING    = 1 << 5,
        CDSTAT_SEEKING    = 1 << 6,
        CDSTAT_PLAYING    = 1 << 7
    };

    enum CommandErrorFlag : uint8_t
    {
        CDERR_SEEK_FAILED         = 1 << 2,
        CDERR_LID_OPENED          = 1 << 3,
        CDERR_INVALID_PARAM_VALUE = 1 << 4,
        CDERR_INVALID_PARAM_COUNT = 1 << 5,
        CDERR_INVALID_COMMAND     = 1 << 6,
        CDERR_NO_DISC             = 1 << 7
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

} // namespace PSX::CDROM

#undef DEF
