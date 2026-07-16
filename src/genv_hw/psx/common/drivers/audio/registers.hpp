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

#define SPU_CH_VOLL(N) _MMIO16((IO_BASE | 0xc00) + (16 * (N)))
#define SPU_CH_VOLR(N) _MMIO16((IO_BASE | 0xc02) + (16 * (N)))
#define SPU_CH_PITCH(N) _MMIO16((IO_BASE | 0xc04) + (16 * (N)))
#define SPU_CH_SSA(N) _MMIO16((IO_BASE | 0xc06) + (16 * (N)))
#define SPU_CH_ADSR1(N) _MMIO16((IO_BASE | 0xc08) + (16 * (N)))
#define SPU_CH_ADSR2(N) _MMIO16((IO_BASE | 0xc0a) + (16 * (N)))
#define SPU_CH_ENVX(N) _MMIO16((IO_BASE | 0xc0c) + (16 * (N)))
#define SPU_CH_LSAX(N) _MMIO16((IO_BASE | 0xc0e) + (16 * (N)))

#define SPU_MVOLL _MMIO16(IO_BASE | 0xd80)
#define SPU_MVOLR _MMIO16(IO_BASE | 0xd82)
#define SPU_EVOLL _MMIO16(IO_BASE | 0xd84)
#define SPU_EVOLR _MMIO16(IO_BASE | 0xd86)
#define SPU_KON0 _MMIO16(IO_BASE | 0xd88)
#define SPU_KON1 _MMIO16(IO_BASE | 0xd8a)
#define SPU_KOFF0 _MMIO16(IO_BASE | 0xd8c)
#define SPU_KOFF1 _MMIO16(IO_BASE | 0xd8e)
#define SPU_PMON0 _MMIO16(IO_BASE | 0xd90)
#define SPU_PMON1 _MMIO16(IO_BASE | 0xd92)
#define SPU_NON0 _MMIO16(IO_BASE | 0xd94)
#define SPU_NON1 _MMIO16(IO_BASE | 0xd96)
#define SPU_EON0 _MMIO16(IO_BASE | 0xd98)
#define SPU_EON1 _MMIO16(IO_BASE | 0xd9a)
#define SPU_ENDX0 _MMIO16(IO_BASE | 0xd9c)
#define SPU_ENDX1 _MMIO16(IO_BASE | 0xd9e)

#define SPU_ESA _MMIO16(IO_BASE | 0xda2)
#define SPU_IRQA _MMIO16(IO_BASE | 0xda4)
#define SPU_TSA _MMIO16(IO_BASE | 0xda6)
#define SPU_DATA _MMIO16(IO_BASE | 0xda8)
#define SPU_CTRL _MMIO16(IO_BASE | 0xdaa)
#define SPU_RAM_CTRL _MMIO16(IO_BASE | 0xdac)
#define SPU_STAT _MMIO16(IO_BASE | 0xdae)

#define SPU_AVOLL _MMIO16(IO_BASE | 0xdb0)
#define SPU_AVOLR _MMIO16(IO_BASE | 0xdb2)
#define SPU_BVOLL _MMIO16(IO_BASE | 0xdb4)
#define SPU_BVOLR _MMIO16(IO_BASE | 0xdb6)
#define SPU_MVOLXL _MMIO16(IO_BASE | 0xdb8)
#define SPU_MVOLXR _MMIO16(IO_BASE | 0xdba)

#define SPU_REVERB_dAPF1 _MMIO16(IO_BASE | 0xdc0)
#define SPU_REVERB_dAPF2 _MMIO16(IO_BASE | 0xdc2)
#define SPU_REVERB_vIIR _MMIO16(IO_BASE | 0xdc4)
#define SPU_REVERB_vCOMB1 _MMIO16(IO_BASE | 0xdc6)
#define SPU_REVERB_vCOMB2 _MMIO16(IO_BASE | 0xdc8)
#define SPU_REVERB_vCOMB3 _MMIO16(IO_BASE | 0xdca)
#define SPU_REVERB_vCOMB4 _MMIO16(IO_BASE | 0xdcc)
#define SPU_REVERB_vWALL _MMIO16(IO_BASE | 0xdce)
#define SPU_REVERB_vAPF1 _MMIO16(IO_BASE | 0xdd0)
#define SPU_REVERB_vAPF2 _MMIO16(IO_BASE | 0xdd2)
#define SPU_REVERB_mLSAME _MMIO16(IO_BASE | 0xdd4)
#define SPU_REVERB_mRSAME _MMIO16(IO_BASE | 0xdd6)
#define SPU_REVERB_mLCOMB1 _MMIO16(IO_BASE | 0xdd8)
#define SPU_REVERB_mRCOMB1 _MMIO16(IO_BASE | 0xdda)
#define SPU_REVERB_mLCOMB2 _MMIO16(IO_BASE | 0xddc)
#define SPU_REVERB_mRCOMB2 _MMIO16(IO_BASE | 0xdde)
#define SPU_REVERB_dLSAME _MMIO16(IO_BASE | 0xde0)
#define SPU_REVERB_dRSAME _MMIO16(IO_BASE | 0xde2)
#define SPU_REVERB_mLDIFF _MMIO16(IO_BASE | 0xde4)
#define SPU_REVERB_mRDIFF _MMIO16(IO_BASE | 0xde6)
#define SPU_REVERB_mLCOMB3 _MMIO16(IO_BASE | 0xde8)
#define SPU_REVERB_mRCOMB3 _MMIO16(IO_BASE | 0xdea)
#define SPU_REVERB_mLCOMB4 _MMIO16(IO_BASE | 0xdec)
#define SPU_REVERB_mRCOMB4 _MMIO16(IO_BASE | 0xdee)
#define SPU_REVERB_dLDIFF _MMIO16(IO_BASE | 0xdf0)
#define SPU_REVERB_dRDIFF _MMIO16(IO_BASE | 0xdf2)
#define SPU_REVERB_mLAPF1 _MMIO16(IO_BASE | 0xdf4)
#define SPU_REVERB_mRAPF1 _MMIO16(IO_BASE | 0xdf6)
#define SPU_REVERB_mLAPF2 _MMIO16(IO_BASE | 0xdf8)
#define SPU_REVERB_mRAPF2 _MMIO16(IO_BASE | 0xdfa)
#define SPU_REVERB_vLIN _MMIO16(IO_BASE | 0xdfc)
#define SPU_REVERB_vRIN _MMIO16(IO_BASE | 0xdfe)

#define SPU_CH_VOLXL(N) _MMIO16((IO_BASE | 0xe00) + (4 * (N)))
#define SPU_CH_VOLXR(N) _MMIO16((IO_BASE | 0xe02) + (4 * (N)))

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