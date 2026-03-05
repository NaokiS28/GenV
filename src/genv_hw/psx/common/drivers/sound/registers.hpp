/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * registers.hpp - Created on 23-02-2026
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

#include "psx/common/registers.hpp"
#include <cstdint>

#define SPU_CH_VOL_L(N) _MMIO16((IO_BASE | 0xc00) + (16 * (N)))
#define SPU_CH_VOL_R(N) _MMIO16((IO_BASE | 0xc02) + (16 * (N)))
#define SPU_CH_FREQ(N) _MMIO16((IO_BASE | 0xc04) + (16 * (N)))
#define SPU_CH_ADDR(N) _MMIO16((IO_BASE | 0xc06) + (16 * (N)))
#define SPU_CH_ADSR1(N) _MMIO16((IO_BASE | 0xc08) + (16 * (N)))
#define SPU_CH_ADSR2(N) _MMIO16((IO_BASE | 0xc0a) + (16 * (N)))
#define SPU_CH_ADSR_VOL(N) _MMIO16((IO_BASE | 0xc0c) + (16 * (N)))
#define SPU_CH_LOOP_ADDR(N) _MMIO16((IO_BASE | 0xc0e) + (16 * (N)))

#define SPU_MASTER_VOL_L _MMIO16(IO_BASE | 0xd80)
#define SPU_MASTER_VOL_R _MMIO16(IO_BASE | 0xd82)
#define SPU_REVERB_VOL_L _MMIO16(IO_BASE | 0xd84)
#define SPU_REVERB_VOL_R _MMIO16(IO_BASE | 0xd86)
#define SPU_FLAG_ON1 _MMIO16(IO_BASE | 0xd88)
#define SPU_FLAG_ON2 _MMIO16(IO_BASE | 0xd8a)
#define SPU_FLAG_OFF1 _MMIO16(IO_BASE | 0xd8c)
#define SPU_FLAG_OFF2 _MMIO16(IO_BASE | 0xd8e)
#define SPU_FLAG_FM1 _MMIO16(IO_BASE | 0xd90)
#define SPU_FLAG_FM2 _MMIO16(IO_BASE | 0xd92)
#define SPU_FLAG_NOISE1 _MMIO16(IO_BASE | 0xd94)
#define SPU_FLAG_NOISE2 _MMIO16(IO_BASE | 0xd96)
#define SPU_FLAG_REVERB1 _MMIO16(IO_BASE | 0xd98)
#define SPU_FLAG_REVERB2 _MMIO16(IO_BASE | 0xd9a)
#define SPU_FLAG_STATUS1 _MMIO16(IO_BASE | 0xd9c)
#define SPU_FLAG_STATUS2 _MMIO16(IO_BASE | 0xd9e)

#define SPU_REVERB_ADDR _MMIO16(IO_BASE | 0xda2)
#define SPU_IRQ_ADDR _MMIO16(IO_BASE | 0xda4)
#define SPU_ADDR _MMIO16(IO_BASE | 0xda6)
#define SPU_DATA _MMIO16(IO_BASE | 0xda8)
#define SPU_CTRL _MMIO16(IO_BASE | 0xdaa)
#define SPU_DMA_CTRL _MMIO16(IO_BASE | 0xdac)
#define SPU_STAT _MMIO16(IO_BASE | 0xdae)

#define SPU_CDDA_VOL_L _MMIO16(IO_BASE | 0xdb0)
#define SPU_CDDA_VOL_R _MMIO16(IO_BASE | 0xdb2)
#define SPU_EXT_VOL_L _MMIO16(IO_BASE | 0xdb4)
#define SPU_EXT_VOL_R _MMIO16(IO_BASE | 0xdb6)
#define SPU_VOL_STAT_L _MMIO16(IO_BASE | 0xdb8)
#define SPU_VOL_STAT_R _MMIO16(IO_BASE | 0xdba)

#define SPU_REVERB_DAPF1 _MMIO16(IO_BASE | 0xdc0)
#define SPU_REVERB_DAPF2 _MMIO16(IO_BASE | 0xdc2)
#define SPU_REVERB_VIIR _MMIO16(IO_BASE | 0xdc4)
#define SPU_REVERB_VCOMB1 _MMIO16(IO_BASE | 0xdc6)
#define SPU_REVERB_VCOMB2 _MMIO16(IO_BASE | 0xdc8)
#define SPU_REVERB_VCOMB3 _MMIO16(IO_BASE | 0xdca)
#define SPU_REVERB_VCOMB4 _MMIO16(IO_BASE | 0xdcc)
#define SPU_REVERB_VWALL _MMIO16(IO_BASE | 0xdce)
#define SPU_REVERB_VAPF1 _MMIO16(IO_BASE | 0xdd0)
#define SPU_REVERB_VAPF2 _MMIO16(IO_BASE | 0xdd2)
#define SPU_REVERB_MLSAME _MMIO16(IO_BASE | 0xdd4)
#define SPU_REVERB_MRSAME _MMIO16(IO_BASE | 0xdd6)
#define SPU_REVERB_MLCOMB1 _MMIO16(IO_BASE | 0xdd8)
#define SPU_REVERB_MRCOMB1 _MMIO16(IO_BASE | 0xdda)
#define SPU_REVERB_MLCOMB2 _MMIO16(IO_BASE | 0xddc)
#define SPU_REVERB_MRCOMB2 _MMIO16(IO_BASE | 0xdde)
#define SPU_REVERB_DLSAME _MMIO16(IO_BASE | 0xde0)
#define SPU_REVERB_DRSAME _MMIO16(IO_BASE | 0xde2)
#define SPU_REVERB_MLDIFF _MMIO16(IO_BASE | 0xde4)
#define SPU_REVERB_MRDIFF _MMIO16(IO_BASE | 0xde6)
#define SPU_REVERB_MLCOMB3 _MMIO16(IO_BASE | 0xde8)
#define SPU_REVERB_MRCOMB3 _MMIO16(IO_BASE | 0xdea)
#define SPU_REVERB_MLCOMB4 _MMIO16(IO_BASE | 0xdec)
#define SPU_REVERB_MRCOMB4 _MMIO16(IO_BASE | 0xdee)
#define SPU_REVERB_DLDIFF _MMIO16(IO_BASE | 0xdf0)
#define SPU_REVERB_DRDIFF _MMIO16(IO_BASE | 0xdf2)
#define SPU_REVERB_MLAPF1 _MMIO16(IO_BASE | 0xdf4)
#define SPU_REVERB_MRAPF1 _MMIO16(IO_BASE | 0xdf6)
#define SPU_REVERB_MLAPF2 _MMIO16(IO_BASE | 0xdf8)
#define SPU_REVERB_MRAPF2 _MMIO16(IO_BASE | 0xdfa)
#define SPU_REVERB_VLIN _MMIO16(IO_BASE | 0xdfc)
#define SPU_REVERB_VRIN _MMIO16(IO_BASE | 0xdfe)

namespace PSX::SPU
{
    enum StatusFlag : uint16_t
    {
        SPU_STAT_CDDA           = 1 << 0,
        SPU_STAT_EXT            = 1 << 1,
        SPU_STAT_CDDA_REVERB    = 1 << 2,
        SPU_STAT_EXT_REVERB     = 1 << 3,
        SPU_STAT_XFER_BITMASK   = 3 << 4,
        SPU_STAT_XFER_NONE      = 0 << 4,
        SPU_STAT_XFER_WRITE     = 1 << 4,
        SPU_STAT_XFER_DMA_WRITE = 2 << 4,
        SPU_STAT_XFER_DMA_READ  = 3 << 4,
        SPU_STAT_IRQ            = 1 << 6,
        SPU_STAT_DREQ           = 1 << 7,
        SPU_STAT_WRITE_REQ      = 1 << 8,
        SPU_STAT_READ_REQ       = 1 << 9,
        SPU_STAT_BUSY           = 1 << 10,
        SPU_STAT_CAPTURE_BUF    = 1 << 11
    };

    enum ControlFlag : uint16_t
    {
        SPU_CTRL_CDDA           = 1 << 0,
        SPU_CTRL_EXT            = 1 << 1,
        SPU_CTRL_CDDA_REVERB    = 1 << 2,
        SPU_CTRL_EXT_REVERB     = 1 << 3,
        SPU_CTRL_XFER_BITMASK   = 3 << 4,
        SPU_CTRL_XFER_NONE      = 0 << 4,
        SPU_CTRL_XFER_WRITE     = 1 << 4,
        SPU_CTRL_XFER_DMA_WRITE = 2 << 4,
        SPU_CTRL_XFER_DMA_READ  = 3 << 4,
        SPU_CTRL_IRQ_ENABLE     = 1 << 6,
        SPU_CTRL_REVERB         = 1 << 7,
        SPU_CTRL_UNMUTE         = 1 << 14,
        SPU_CTRL_ENABLE         = 1 << 15
    };
} // namespace PSX::SPU