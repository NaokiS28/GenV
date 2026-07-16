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

#define _ADDR8(addr) ((volatile uint8_t *)(addr))
#define _ADDR16(addr) ((volatile uint16_t *)(addr))
#define _ADDR32(addr) ((volatile uint32_t *)(addr))
#define _MMIO8(addr) (*_ADDR8(addr))
#define _MMIO16(addr) (*_ADDR16(addr))
#define _MMIO32(addr) (*_ADDR32(addr))

/* Constants */

#define F_CPU 33868800
#define F_GPU_NTSC 53693175
#define F_GPU_PAL 53203425

typedef enum
{
    DEV0_BASE  = 0xbf000000,
    EXP1_BASE  = 0xbf000000,
    CACHE_BASE = 0x9f800000, // Cannot be accessed from KSEG1
    IO_BASE    = 0xbf801000,
    EXP2_BASE  = 0xbf802000,
    EXP3_BASE  = 0xbfa00000,
    DEV2_BASE  = 0xbfc00000,
    CPU_BASE   = 0xfffe0000
} BaseAddress;

/* Serial interfaces */

typedef enum
{
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

typedef enum
{
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

typedef enum
{
    SIO_CTRL_TX_ENABLE      = 1 << 0,
    SIO_CTRL_DTR            = 1 << 1, // DTR is /CS on SIO0
    SIO_CTRL_RX_ENABLE      = 1 << 2,
    SIO_CTRL_TX_INVERT      = 1 << 3, // SIO1 only
    SIO_CTRL_ACKNOWLEDGE    = 1 << 4,
    SIO_CTRL_RTS            = 1 << 5, // SIO1 only
    SIO_CTRL_RESET          = 1 << 6,
    SIO_CTRL_TX_IRQ_ENABLE  = 1 << 10,
    SIO_CTRL_RX_IRQ_ENABLE  = 1 << 11,
    SIO_CTRL_DSR_IRQ_ENABLE = 1 << 12, // DSR is /ACK on SIO0
    SIO_CTRL_CS_PORT_1      = 0 << 13, // SIO0 only
    SIO_CTRL_CS_PORT_2      = 1 << 13  // SIO0 only
} SIOControlFlag;

// SIO_DATA is a 32-bit register, but some emulators do not implement it
// correctly and break if it's read more than 8 bits at a time.
#define SIO_DATA(N) _MMIO8((IO_BASE | 0x040) + (16 * (N)))
#define SIO_STAT(N) _MMIO16((IO_BASE | 0x044) + (16 * (N)))
#define SIO_MODE(N) _MMIO16((IO_BASE | 0x048) + (16 * (N)))
#define SIO_CTRL(N) _MMIO16((IO_BASE | 0x04a) + (16 * (N)))
#define SIO_BAUD(N) _MMIO16((IO_BASE | 0x04e) + (16 * (N)))

/* Bus interface */

typedef enum
{
    BIU_CTRL_WRITE_DELAY_BITMASK = 15 << 0,
    BIU_CTRL_READ_DELAY_BITMASK  = 15 << 4,
    BIU_CTRL_RECOVERY            = 1 << 8,
    BIU_CTRL_HOLD                = 1 << 9,
    BIU_CTRL_FLOAT               = 1 << 10,
    BIU_CTRL_PRESTROBE           = 1 << 11,
    BIU_CTRL_WIDTH_8             = 0 << 12,
    BIU_CTRL_WIDTH_16            = 1 << 12,
    BIU_CTRL_AUTO_INCR           = 1 << 13,
    BIU_CTRL_SIZE_BITMASK        = 31 << 16,
    BIU_CTRL_DMA_DELAY_BITMASK   = 15 << 24,
    BIU_CTRL_ADDR_ERROR          = 1 << 28,
    BIU_CTRL_DMA_DELAY           = 1 << 29,
    BIU_CTRL_DMA32               = 1 << 30,
    BIU_CTRL_WAIT                = 1 << 31
} BIUControlFlag;

#define BIU_DEV0_ADDR _MMIO32(IO_BASE | 0x000) // PIO/573
#define BIU_EXP2_ADDR _MMIO32(IO_BASE | 0x004) // PIO/debug
#define BIU_DEV0_CTRL _MMIO32(IO_BASE | 0x008) // PIO/573
#define BIU_EXP3_CTRL _MMIO32(IO_BASE | 0x00c) // PIO/debug
#define BIU_DEV2_CTRL _MMIO32(IO_BASE | 0x010) // BIOS ROM
#define BIU_DEV4_CTRL _MMIO32(IO_BASE | 0x014) // SPU
#define BIU_DEV5_CTRL _MMIO32(IO_BASE | 0x018) // CD-ROM
#define BIU_EXP2_CTRL _MMIO32(IO_BASE | 0x01c) // PIO/debug
#define BIU_COM_DELAY _MMIO32(IO_BASE | 0x020)

/* DRAM controller */

typedef enum
{
    DRAM_CTRL_UNKNOWN     = 1 << 3,
    DRAM_CTRL_FETCH_DELAY = 1 << 7,
    DRAM_CTRL_SIZE_MUL1   = 0 << 9,
    DRAM_CTRL_SIZE_MUL4   = 1 << 9,
    DRAM_CTRL_COUNT_1     = 0 << 10, // 1 DRAM bank (single RAS)
    DRAM_CTRL_COUNT_2     = 1 << 10, // 2 DRAM banks (dual RAS)
    DRAM_CTRL_SIZE_1MB    = 0 << 11, // 1MB chips (4MB with MUL4)
    DRAM_CTRL_SIZE_2MB    = 1 << 11  // 2MB chips (8MB with MUL4)
} DRAMControlFlag;

#define DRAM_CTRL _MMIO32(IO_BASE | 0x060)

/* IRQ controller */

typedef enum
{
    IRQ_VSYNC  = 0,
    IRQ_GPU    = 1,
    IRQ_CDROM  = 2,
    IRQ_DMA    = 3,
    IRQ_TIMER0 = 4,
    IRQ_TIMER1 = 5,
    IRQ_TIMER2 = 6,
    IRQ_SIO0   = 7,
    IRQ_SIO1   = 8,
    IRQ_SPU    = 9,
    IRQ_GUN    = 10,
    IRQ_PIO    = 10,
    IRQ_MAX,
    IRQ_INVALID = IRQ_MAX
} IRQChannel;

#define IRQ_STAT _MMIO16(IO_BASE | 0x070)
#define IRQ_MASK _MMIO16(IO_BASE | 0x074)

/* DMA */

typedef enum
{
    DMA_MDEC_IN  = 0,
    DMA_MDEC_OUT = 1,
    DMA_GPU      = 2,
    DMA_CDROM    = 3,
    DMA_SPU      = 4,
    DMA_PIO      = 5,
    DMA_OTC      = 6
} DMAChannel;

typedef enum
{
    DMA_CHCR_READ             = 0 << 0,
    DMA_CHCR_WRITE            = 1 << 0,
    DMA_CHCR_REVERSE          = 1 << 1,
    DMA_CHCR_CHOPPING         = 1 << 8,
    DMA_CHCR_MODE_BITMASK     = 3 << 9,
    DMA_CHCR_MODE_BURST       = 0 << 9,
    DMA_CHCR_MODE_SLICE       = 1 << 9,
    DMA_CHCR_MODE_LIST        = 2 << 9,
    DMA_CHCR_DMA_TIME_BITMASK = 7 << 16,
    DMA_CHCR_CPU_TIME_BITMASK = 7 << 20,
    DMA_CHCR_ENABLE           = 1 << 24,
    DMA_CHCR_TRIGGER          = 1 << 28,
    DMA_CHCR_PAUSE            = 1 << 29 // Burst mode only
} DMACHCRFlag;

#define DMA_DPCR_CH_PRIORITY_BITMASK(N) (7 << (4 * (N)))
#define DMA_DPCR_CH_PRIORITY(N, priority) ((priority & 7) << (4 * (N)))
#define DMA_DPCR_CH_ENABLE(N) ((1 << 3) << (4 * (N)))

typedef enum
{
    DMA_DICR_CH_MODE_BITMASK   = 0x7f << 0,
    DMA_DICR_BUS_ERROR         = 1 << 15,
    DMA_DICR_CH_ENABLE_BITMASK = 0x7f << 16,
    DMA_DICR_IRQ_ENABLE        = 1 << 23,
    DMA_DICR_CH_STAT_BITMASK   = 0x7f << 24,
    DMA_DICR_IRQ               = 1 << 31
} DMADICRFlag;

#define DMA_DICR_CH_MODE(N) (1 << ((N) + 0))
#define DMA_DICR_CH_ENABLE(N) (1 << ((N) + 16))
#define DMA_DICR_CH_STAT(N) (1 << ((N) + 24))

#define DMA_MADR(N) _MMIO32((IO_BASE | 0x080) + (16 * (N)))
#define DMA_BCR(N) _MMIO32((IO_BASE | 0x084) + (16 * (N)))
#define DMA_CHCR(N) _MMIO32((IO_BASE | 0x088) + (16 * (N)))

#define DMA_DPCR _MMIO32(IO_BASE | 0x0f0)
#define DMA_DICR _MMIO32(IO_BASE | 0x0f4)

/* Timers */

/* GPU */

typedef enum
{
    GP1_STAT_PAGE_X_BITMASK      = 15 << 0, // GP0_CMD_TEXPAGE
    GP1_STAT_PAGE_Y0             = 1 << 4,  // GP0_CMD_TEXPAGE
    GP1_STAT_BLEND_BITMASK       = 3 << 5,  // GP0_CMD_TEXPAGE
    GP1_STAT_BLEND_SEMITRANS     = 0 << 5,  // GP0_CMD_TEXPAGE
    GP1_STAT_BLEND_ADD           = 1 << 5,  // GP0_CMD_TEXPAGE
    GP1_STAT_BLEND_SUBTRACT      = 2 << 5,  // GP0_CMD_TEXPAGE
    GP1_STAT_BLEND_DIV4_ADD      = 3 << 5,  // GP0_CMD_TEXPAGE
    GP1_STAT_COLOR_BITMASK       = 3 << 7,  // GP0_CMD_TEXPAGE
    GP1_STAT_COLOR_4BPP          = 0 << 7,  // GP0_CMD_TEXPAGE
    GP1_STAT_COLOR_8BPP          = 1 << 7,  // GP0_CMD_TEXPAGE
    GP1_STAT_COLOR_16BPP         = 2 << 7,  // GP0_CMD_TEXPAGE
    GP1_STAT_DITHER              = 1 << 9,  // GP0_CMD_TEXPAGE
    GP1_STAT_UNLOCK_FB           = 1 << 10, // GP0_CMD_TEXPAGE
    GP1_STAT_SET_MASK            = 1 << 11, // GP0_CMD_FB_MASK
    GP1_STAT_USE_MASK            = 1 << 12, // GP0_CMD_FB_MASK
    GP1_STAT_DISP_FIELD_BITMASK  = 1 << 13,
    GP1_STAT_DISP_FIELD_EVEN     = 0 << 13,
    GP1_STAT_DISP_FIELD_ODD      = 1 << 13,
    GP1_STAT_PAGE_Y1             = 1 << 15, // GP0_CMD_TEXPAGE
    GP1_STAT_FB_HRES_BITMASK     = 7 << 16, // GP1_CMD_FB_MODE
    GP1_STAT_FB_VRES_BITMASK     = 1 << 19, // GP1_CMD_FB_MODE
    GP1_STAT_FB_VRES_256         = 0 << 19, // GP1_CMD_FB_MODE
    GP1_STAT_FB_VRES_512         = 1 << 19, // GP1_CMD_FB_MODE
    GP1_STAT_FB_MODE_BITMASK     = 1 << 20, // GP1_CMD_FB_MODE
    GP1_STAT_FB_MODE_NTSC        = 0 << 20, // GP1_CMD_FB_MODE
    GP1_STAT_FB_MODE_PAL         = 1 << 20, // GP1_CMD_FB_MODE
    GP1_STAT_FB_COLOR_BITMASK    = 1 << 21, // GP1_CMD_FB_MODE
    GP1_STAT_FB_COLOR_16BPP      = 0 << 21, // GP1_CMD_FB_MODE
    GP1_STAT_FB_COLOR_24BPP      = 1 << 21, // GP1_CMD_FB_MODE
    GP1_STAT_FB_INTERLACE        = 1 << 22, // GP1_CMD_FB_MODE
    GP1_STAT_DISP_BLANK          = 1 << 23, // GP1_CMD_DISP_BLANK
    GP1_STAT_IRQ                 = 1 << 24,
    GP1_STAT_DREQ                = 1 << 25,
    GP1_STAT_CMD_READY           = 1 << 26,
    GP1_STAT_READ_READY          = 1 << 27,
    GP1_STAT_WRITE_READY         = 1 << 28,
    GP1_STAT_DREQ_MODE_BITMASK   = 3 << 29, // GP1_CMD_DREQ_MODE
    GP1_STAT_DREQ_MODE_NONE      = 0 << 29, // GP1_CMD_DREQ_MODE
    GP1_STAT_DREQ_MODE_FIFO      = 1 << 29, // GP1_CMD_DREQ_MODE
    GP1_STAT_DREQ_MODE_GP0_WRITE = 2 << 29, // GP1_CMD_DREQ_MODE
    GP1_STAT_DREQ_MODE_GP0_READ  = 3 << 29, // GP1_CMD_DREQ_MODE
    GP1_STAT_DRAW_FIELD_BITMASK  = 1 << 31,
    GP1_STAT_DRAW_FIELD_EVEN     = 0 << 31,
    GP1_STAT_DRAW_FIELD_ODD      = 1 << 31
} GP1StatusFlag;

#define GPU_GP0 _MMIO32(IO_BASE | 0x810)
#define GPU_GP1 _MMIO32(IO_BASE | 0x814)

/* MDEC */

typedef enum
{
    MDEC_CMD_LENGTH_BITMASK     = 0xffff << 0, // MDEC_CMD_OP_DECODE
    MDEC_CMD_USE_CHROMA         = 1 << 0,      // MDEC_CMD_OP_SET_QUANT_TABLE
    MDEC_CMD_SIGNED             = 1 << 25,     // MDEC_CMD_OP_DECODE
    MDEC_CMD_16BPP_MASK         = 1 << 26,     // MDEC_CMD_OP_DECODE
    MDEC_CMD_FORMAT_BITMASK     = 3 << 27,     // MDEC_CMD_OP_DECODE
    MDEC_CMD_FORMAT_4BPP        = 0 << 27,     // MDEC_CMD_OP_DECODE
    MDEC_CMD_FORMAT_8BPP        = 1 << 27,     // MDEC_CMD_OP_DECODE
    MDEC_CMD_FORMAT_24BPP       = 2 << 27,     // MDEC_CMD_OP_DECODE
    MDEC_CMD_FORMAT_16BPP       = 3 << 27,     // MDEC_CMD_OP_DECODE
    MDEC_CMD_OP_BITMASK         = 7 << 29,
    MDEC_CMD_OP_NOP             = 0 << 29,
    MDEC_CMD_OP_DECODE          = 1 << 29,
    MDEC_CMD_OP_SET_QUANT_TABLE = 2 << 29,
    MDEC_CMD_OP_SET_IDCT_TABLE  = 3 << 29
} MDECCommandFlag;

typedef enum
{
    MDEC_STAT_LENGTH_BITMASK = 0xffff << 0,
    MDEC_STAT_BLOCK_BITMASK  = 7 << 16,
    MDEC_STAT_BLOCK_Y0       = 0 << 16,
    MDEC_STAT_BLOCK_Y1       = 1 << 16,
    MDEC_STAT_BLOCK_Y2       = 2 << 16,
    MDEC_STAT_BLOCK_Y3       = 3 << 16,
    MDEC_STAT_BLOCK_CR       = 4 << 16,
    MDEC_STAT_BLOCK_CB       = 5 << 16,
    MDEC_STAT_16BPP_MASK     = 1 << 23,
    MDEC_STAT_SIGNED         = 1 << 24,
    MDEC_STAT_FORMAT_BITMASK = 3 << 25,
    MDEC_STAT_FORMAT_4BPP    = 0 << 25,
    MDEC_STAT_FORMAT_8BPP    = 1 << 25,
    MDEC_STAT_FORMAT_24BPP   = 2 << 25,
    MDEC_STAT_FORMAT_16BPP   = 3 << 25,
    MDEC_STAT_DREQ_OUT       = 1 << 27,
    MDEC_STAT_DREQ_IN        = 1 << 28,
    MDEC_STAT_BUSY           = 1 << 29,
    MDEC_STAT_DATA_FULL      = 1 << 30,
    MDEC_STAT_DATA_EMPTY     = 1 << 31
} MDECStatusFlag;

typedef enum
{
    MDEC_CTRL_DMA_OUT = 1 << 29,
    MDEC_CTRL_DMA_IN  = 1 << 30,
    MDEC_CTRL_RESET   = 1 << 31
} MDECControlFlag;

#define MDEC0 _MMIO32(IO_BASE | 0x820)
#define MDEC1 _MMIO32(IO_BASE | 0x824)

/* CW33300 CPU configuration */

typedef enum
{
    CPU_BCC_LOCK           = 1 << 0,
    CPU_BCC_INV            = 1 << 1,
    CPU_BCC_TAG            = 1 << 2,
    CPU_BCC_RAM            = 1 << 3,
    CPU_BCC_DBLKSZ_BITMASK = 3 << 4,
    CPU_BCC_DBLKSZ_2       = 0 << 4,
    CPU_BCC_DBLKSZ_4       = 1 << 4,
    CPU_BCC_DS             = 1 << 7,
    CPU_BCC_IBLKSZ_BITMASK = 3 << 8,
    CPU_BCC_IBLKSZ_2       = 0 << 8,
    CPU_BCC_IBLKSZ_4       = 1 << 8,
    CPU_BCC_IS0            = 1 << 10,
    CPU_BCC_IS1            = 1 << 11,
    CPU_BCC_INTP           = 1 << 12,
    CPU_BCC_RDPRI          = 1 << 13,
    CPU_BCC_NOPAD          = 1 << 14,
    CPU_BCC_BGNT           = 1 << 15,
    CPU_BCC_LDSCH          = 1 << 16,
    CPU_BCC_NOSTR          = 1 << 17
} CPUBCCFlag;

#define CPU_BCC _MMIO32(CPU_BASE | 0x130)