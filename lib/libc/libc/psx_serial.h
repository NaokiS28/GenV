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
 *
 * This code is based on psyqo's malloc implementation, available here:
 * https://github.com/grumpycoders/pcsx-redux/blob/main/src/mips/psyqo/src/alloc.c
 */

#include <stdint.h>

#define _ADDR8(addr)  ((volatile uint8_t *) (addr))
#define _ADDR16(addr) ((volatile uint16_t *) (addr))
#define _ADDR32(addr) ((volatile uint32_t *) (addr))
#define _MMIO8(addr)  (*_ADDR8(addr))
#define _MMIO16(addr) (*_ADDR16(addr))
#define _MMIO32(addr) (*_ADDR32(addr))

typedef enum {
	DEV0_BASE  = 0xbf000000,
	EXP1_BASE  = 0xbf000000,
	CACHE_BASE = 0x9f800000, // Cannot be accessed from KSEG1
	IO_BASE    = 0xbf801000,
	EXP2_BASE  = 0xbf802000,
	EXP3_BASE  = 0xbfa00000,
	DEV2_BASE  = 0xbfc00000,
	CPU_BASE   = 0xfffe0000
} BaseAddress;

/* Constants */

#define F_CPU      33868800
#define F_GPU_NTSC 53693175
#define F_GPU_PAL  53203425
/* Serial interfaces */

typedef enum {
	SIO_STAT_TX_NOT_FULL   = 1 << 0,
	SIO_STAT_RX_NOT_EMPTY  = 1 << 1,
	SIO_STAT_TX_EMPTY      = 1 << 2,
	SIO_STAT_RX_PARITY_ERR = 1 << 3,
	SIO_STAT_RX_OVERRUN    = 1 << 4, // SIO1 only
	SIO_STAT_RX_STOP_ERR   = 1 << 5, // SIO1 only
	SIO_STAT_RX_INVERT     = 1 << 6, // SIO1 only
	SIO_STAT_DSR           = 1 << 7, // DSR is /ACK on SIO0
	SIO_STAT_CTS           = 1 << 8, // SIO1 only
	SIO_STAT_IRQ           = 1 << 9
} SIOStatusFlag;

typedef enum {
	SIO_MODE_BAUD_BITMASK   = 3 << 0,
	SIO_MODE_BAUD_DIV1      = 1 << 0,
	SIO_MODE_BAUD_DIV16     = 2 << 0,
	SIO_MODE_BAUD_DIV64     = 3 << 0,
	SIO_MODE_DATA_BITMASK   = 3 << 2,
	SIO_MODE_DATA_5         = 0 << 2,
	SIO_MODE_DATA_6         = 1 << 2,
	SIO_MODE_DATA_7         = 2 << 2,
	SIO_MODE_DATA_8         = 3 << 2,
	SIO_MODE_PARITY_BITMASK = 3 << 4,
	SIO_MODE_PARITY_NONE    = 0 << 4,
	SIO_MODE_PARITY_EVEN    = 1 << 4,
	SIO_MODE_PARITY_ODD     = 3 << 4,
	SIO_MODE_STOP_BITMASK   = 3 << 6, // SIO1 only
	SIO_MODE_STOP_1         = 1 << 6, // SIO1 only
	SIO_MODE_STOP_1_5       = 2 << 6, // SIO1 only
	SIO_MODE_STOP_2         = 3 << 6, // SIO1 only
	SIO_MODE_SCK_INVERT     = 1 << 8  // SIO0 only
} SIOModeFlag;

typedef enum {
	SIO_CTRL_TX_ENABLE      = 1 <<  0,
	SIO_CTRL_DTR            = 1 <<  1, // DTR is /CS on SIO0
	SIO_CTRL_RX_ENABLE      = 1 <<  2,
	SIO_CTRL_TX_INVERT      = 1 <<  3, // SIO1 only
	SIO_CTRL_ACKNOWLEDGE    = 1 <<  4,
	SIO_CTRL_RTS            = 1 <<  5, // SIO1 only
	SIO_CTRL_RESET          = 1 <<  6,
	SIO_CTRL_TX_IRQ_ENABLE  = 1 << 10,
	SIO_CTRL_RX_IRQ_ENABLE  = 1 << 11,
	SIO_CTRL_DSR_IRQ_ENABLE = 1 << 12, // DSR is /ACK on SIO0
	SIO_CTRL_CS_PORT_1      = 0 << 13, // SIO0 only
	SIO_CTRL_CS_PORT_2      = 1 << 13  // SIO0 only
} SIOControlFlag;

// SIO_DATA is a 32-bit register, but some emulators do not implement it
// correctly and break if it's read more than 8 bits at a time.
#define SIO_DATA(N) _MMIO8 ((IO_BASE | 0x040) + (16 * (N)))
#define SIO_STAT(N) _MMIO16((IO_BASE | 0x044) + (16 * (N)))
#define SIO_MODE(N) _MMIO16((IO_BASE | 0x048) + (16 * (N)))
#define SIO_CTRL(N) _MMIO16((IO_BASE | 0x04a) + (16 * (N)))
#define SIO_BAUD(N) _MMIO16((IO_BASE | 0x04e) + (16 * (N)))