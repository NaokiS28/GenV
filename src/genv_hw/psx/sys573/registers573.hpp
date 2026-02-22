/*
 * ps1-bare-metal - (C) 2023 spicyjpeg
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

#include "psx/common/registers.hpp"
#include "common/util/enum_defs.hpp"

/* System 573 base hardware */
namespace System573
{
    enum class MiscOutput : uint16_t
    {
        NONE        = 0 << 0,
        ADC_DI      = 1 << 0,
        ADC_CS      = 1 << 1,
        ADC_CLK     = 1 << 2,
        COIN_COUNT1 = 1 << 3,
        COIN_COUNT2 = 1 << 4,
        AMP_ENABLE  = 1 << 5,
        CDDA_ENABLE = 1 << 6,
        SPU_ENABLE  = 1 << 7,
        JVS_RESET   = 1 << 8
    };

    ENABLE_BITWISE_OPS(MiscOutput);

    enum class MiscInput : uint16_t
    {
        NONE       = 0 << 0,
        ADC_DO     = 1 << 0,
        ADC_SARS   = 1 << 1,
        CART_SDA   = 1 << 2,
        JVS_SENSE  = 1 << 3,
        JVS_IRDY   = 1 << 4,
        JVS_DRDY   = 1 << 5,
        CART_IRDY  = 1 << 6,
        CART_DRDY  = 1 << 7,
        COIN1      = 1 << 8,
        COIN2      = 1 << 9,
        PCMCIA_CD1 = 1 << 10,
        PCMCIA_CD2 = 1 << 11,
        SERVICE    = 1 << 12
    };

    ENABLE_BITWISE_OPS(MiscInput);

    typedef enum
    {
        FLASH   = 0,
        PCMCIA1 = 16,
        PCMCIA2 = 32
    } FlashBank;

} // namespace System573

#define SYS573_MISC_OUT _MMIO16(DEV0_BASE | 0x400000)
#define SYS573_DIP_CART _MMIO16(DEV0_BASE | 0x400004)
#define SYS573_MISC_IN _MMIO16(DEV0_BASE | 0x400006)
#define SYS573_JAMMA_MAIN _MMIO16(DEV0_BASE | 0x400008)
#define SYS573_JVS_RX_DATA _MMIO16(DEV0_BASE | 0x40000a)
#define SYS573_JAMMA_EXT1 _MMIO16(DEV0_BASE | 0x40000c)
#define SYS573_JAMMA_EXT2 _MMIO16(DEV0_BASE | 0x40000e)
#define SYS573_BANK_CTRL _MMIO16(DEV0_BASE | 0x500000)
#define SYS573_JVS_IRDY_ACK _MMIO16(DEV0_BASE | 0x520000)
#define SYS573_IDE_RESET _MMIO16(DEV0_BASE | 0x560000)
#define SYS573_WATCHDOG _MMIO16(DEV0_BASE | 0x5c0000)
#define SYS573_EXT_OUT _MMIO16(DEV0_BASE | 0x600000)
#define SYS573_JVS_TX_DATA _MMIO16(DEV0_BASE | 0x680000)
#define SYS573_CART_OUT _MMIO16(DEV0_BASE | 0x6a0000)

#define SYS573_FLASH_BASE _ADDR16(DEV0_BASE | 0x000000)
#define SYS573_IDE_CS0_BASE _ADDR16(DEV0_BASE | 0x480000)
#define SYS573_IDE_CS1_BASE _ADDR16(DEV0_BASE | 0x4c0000)
#define SYS573_IO_BASE _ADDR16(DEV0_BASE | 0x640000)

/* System 573 RTC */

typedef enum
{
    SYS573_RTC_CTRL_CAL_BITMASK  = 31 << 0,
    SYS573_RTC_CTRL_CAL_POSITIVE = 0 << 5,
    SYS573_RTC_CTRL_CAL_NEGATIVE = 1 << 5,
    SYS573_RTC_CTRL_READ         = 1 << 6,
    SYS573_RTC_CTRL_WRITE        = 1 << 7
} Sys573RTCControlFlag;

typedef enum
{
    SYS573_RTC_SECOND_UNITS_BITMASK = 15 << 0,
    SYS573_RTC_SECOND_TENS_BITMASK  = 7 << 4,
    SYS573_RTC_SECOND_STOP          = 1 << 7
} Sys573RTCSecondFlag;

typedef enum
{
    SYS573_RTC_WEEKDAY_UNITS_BITMASK  = 7 << 0,
    SYS573_RTC_WEEKDAY_CENTURY        = 1 << 4,
    SYS573_RTC_WEEKDAY_CENTURY_ENABLE = 1 << 5,
    SYS573_RTC_WEEKDAY_FREQUENCY_TEST = 1 << 6
} Sys573RTCWeekdayFlag;

typedef enum
{
    SYS573_RTC_DAY_UNITS_BITMASK   = 15 << 0,
    SYS573_RTC_DAY_TENS_BITMASK    = 3 << 4,
    SYS573_RTC_DAY_LOW_BATTERY     = 1 << 6,
    SYS573_RTC_DAY_BATTERY_MONITOR = 1 << 7
} Sys573RTCDayFlag;

#define SYS573_RTC_BASE (DEV0_BASE | 0x620000)
#define SYS573_RTC_SIZE ((8 * 1024) - 0x000F)
#define SYS573_RTC_CLOCK_BASE _A8_to_A16(SYS573_RTC_BASE, 0x1FF8)

#define SYS573_RTC_SRAM _ADDR16(SYS573_RTC_BASE)
#define SYS573_RTC_CTRL _MMIO16(_A8_to_A16(SYS573_RTC_CLOCK_BASE, 0x00))
#define SYS573_RTC_SECOND _MMIO16(_A8_to_A16(SYS573_RTC_CLOCK_BASE, 0x01))
#define SYS573_RTC_MINUTE _MMIO16(_A8_to_A16(SYS573_RTC_CLOCK_BASE, 0x02))
#define SYS573_RTC_HOUR _MMIO16(_A8_to_A16(SYS573_RTC_CLOCK_BASE, 0x03))
#define SYS573_RTC_WEEKDAY _MMIO16(_A8_to_A16(SYS573_RTC_CLOCK_BASE, 0x04))
#define SYS573_RTC_DAY _MMIO16(_A8_to_A16(SYS573_RTC_CLOCK_BASE, 0x05))
#define SYS573_RTC_MONTH _MMIO16(_A8_to_A16(SYS573_RTC_CLOCK_BASE, 0x06))
#define SYS573_RTC_YEAR _MMIO16(_A8_to_A16(SYS573_RTC_CLOCK_BASE, 0x07))