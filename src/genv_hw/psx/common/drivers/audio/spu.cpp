/*
 * 573in1 - Copyright (C) 2022-2024 spicyjpeg
 *
 * 573in1 is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * 573in1 is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * 573in1. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdint.h>
#include "spu.hpp"
#include "common/util/templates.hpp"
#include "registers.hpp"
#include "psx/common/system/sys.h"

#define SPU_RAM_SIZE 0x80000
#define SPU_RAM_ALLOC_OFFSET 0x01000
#define SPU_RAM_ADDR_UNIT 8

#define SPU_NUM_CHANNELS 24
#define SPU_MAX_VOLUME 0x3fff

namespace PSX
{
    using namespace SPU;

    /* Basic API */

    static constexpr int _DMA_CHUNK_SIZE     = 4;
    static constexpr int _DMA_MAX_CHUNK_SIZE = 16;
    static constexpr int _DMA_TIMEOUT        = 100000;
    static constexpr int _STATUS_TIMEOUT     = 10000;

    static bool _waitForStatus(uint16_t mask, uint16_t value)
    {
        for (int timeout = _STATUS_TIMEOUT; timeout > 0; timeout -= 10)
        {
            if ((SPU_STAT & mask) == value)
                return true;

            psx_delayMicrosecondsBusy(10);
        }

        return false;
    }

    void _waitForSPUDMADone(void)
    {
        while (DMA_CHCR(DMA_SPU) & DMA_CHCR_ENABLE)
            __asm__ volatile("");

        // A delay is required here in order to allow the SPU to flush its transfer
        // FIFO to SPU RAM. This takes around 30 us when the FIFO is full.
        psx_delayMicrosecondsBusy(35);
    }

    bool SPUDriver::init(void)
    {
        BIU_DEV4_CTRL = 0                    //
                        | (1 << 0)           // Write delay
                        | (14 << 4)          // Read delay
                        | BIU_CTRL_RECOVERY  //
                        | BIU_CTRL_WIDTH_16  //
                        | BIU_CTRL_AUTO_INCR //
                        | (9 << 16)          // Number of address lines
                        | (0 << 24)          // DMA read/write delay
                        | BIU_CTRL_DMA_DELAY;

        SPU_CTRL = 0;
        _waitForStatus(0x3f, 0);

        SPU_MVOLL = 0;
        SPU_MVOLR = 0;
        SPU_EVOLL = 0;
        SPU_EVOLR = 0;

        SPU_ESA      = (SPU_RAM_SIZE - 16) / SPU_RAM_ADDR_UNIT;
        SPU_RAM_CTRL = 4;

        SPU_CTRL = SPU_CTRL_ENABLE;
        _waitForStatus(0x3f, 0);

        // Place a dummy (silent) looping block at the beginning of SPU RAM.
        DMA_DPCR |= DMA_DPCR_CH_ENABLE(DMA_SPU);
        DMA_CHCR(DMA_SPU) = 0;

        SPU_DATA = 0x0500;
        for (int i = 7; i > 0; i--)
            SPU_DATA = 0;

        SPU_CTRL = SPU_CTRL_XFER_WRITE | SPU_CTRL_ENABLE;
        _waitForStatus(((uint16_t)SPU_CTRL_XFER_BITMASK | (uint16_t)SPU_STAT_BUSY), SPU_CTRL_XFER_WRITE);
        psx_delayMicrosecondsBusy(100);

        SPU_CTRL = SPU_CTRL_UNMUTE | SPU_CTRL_ENABLE;
        resetAllChannels();
        return true;
    }

    SPUDriver::Channel SPUDriver::getFreeChannel(void) const
    {
#if 0
	// The status flag gets set when a channel stops or loops for the first
	// time rather than when it actually goes silent (so it will be set early
	// for e.g. short looping samples with a long release envelope, or samples
	// looping indefinitely).
	uint32_t flags = (SPU_FLAG_STATUS1 | (SPU_FLAG_STATUS2 << 16)) & 0xffffff;

	for (Channel ch = 0; flags; ch++, flags >>= 1) {
		if (flags & 1)
			return ch;
	}
#else
        for (Channel ch = NUM_CHANNELS - 1; ch >= 0; ch--)
        {
            if (!SPU_CH_ENVX(ch))
                return ch;
        }
#endif

        return -1;
    }

    void SPUDriver::stopChannel(SPUDriver::Channel ch)
    {
        if ((ch < 0) || (ch >= NUM_CHANNELS))
            return;

        SPU_CH_VOLL(ch)  = 0;
        SPU_CH_VOLR(ch)  = 0;
        SPU_CH_PITCH(ch) = 1 << 12;
        SPU_CH_SSA(ch)   = DUMMY_BLOCK_OFFSET / 8;

        if (ch < 16)
        {
            SPU_KOFF0 = 1 << ch;
            SPU_KON0  = 1 << ch;
        }
        else
        {
            SPU_KOFF1 = 1 << (ch - 16);
            SPU_KON1  = 1 << (ch - 16);
        }
    }

    void SPUDriver::resetAllChannels(void)
    {
        // Reset all channels and point them to the beginning of SPU RAM. This is
        // not strictly required but useful when using the SPU's interrupt feature,
        // as "stopped" channels will actually keep reading samples from memory and
        // may accidentally trigger an IRQ.
        // NOTE: doing this is technically invalid as the first 4 KB of SPU RAM
        // contain PCM capture buffers rather than ADPCM samples, but it does not
        // matter as we're also setting the pitch to zero.
        for (int i = 0; i < SPU_NUM_CHANNELS; i++)
        {
            SPU_CH_VOLL(i)  = 0;
            SPU_CH_VOLR(i)  = 0;
            SPU_CH_PITCH(i) = 0;
            SPU_CH_SSA(i)   = 0;
            SPU_CH_ADSR1(i) = 0;
            SPU_CH_ADSR2(i) = 0;
            SPU_CH_LSAX(i)  = 0;
        }

        SPU_PMON0 = 0;
        SPU_PMON1 = 0;
        SPU_NON0  = 0;
        SPU_NON1  = 0;
        SPU_EON0  = 0;
        SPU_EON1  = 0;

        // Key on all channels to force the current playback address to be updated,
        // then key them off again.
        SPU_KON0 = 0xffff;
        SPU_KON1 = 0x00ff;
        psx_delayMicrosecondsBusy(1);
        SPU_KOFF0 = 0xffff;
        SPU_KOFF1 = 0x00ff;
    }

    size_t SPUDriver::upload(uint32_t offset, const void *data, size_t length, bool wait)
    {
        _waitForSPUDMADone();
        assert(!((uint32_t)data % 4));
        assert(!(offset % SPU_RAM_ADDR_UNIT));

        size_t chunkSize, numChunks;
        length = (length + _DMA_CHUNK_SIZE - 1) / _DMA_CHUNK_SIZE;

        if (length < _DMA_MAX_CHUNK_SIZE)
        {
            chunkSize = length;
            numChunks = 1;
        }
        else
        {
            chunkSize = _DMA_MAX_CHUNK_SIZE;
            numChunks = length / _DMA_MAX_CHUNK_SIZE;

            assert(!(length % _DMA_MAX_CHUNK_SIZE));
        }

        _waitForStatus(SPU_CTRL_XFER_BITMASK, SPU_STAT_XFER_NONE);

        auto ctrlReg = static_cast<SPU::ControlFlag>(SPU_CTRL) & ~SPU_CTRL_XFER_BITMASK;
        SPU_TSA      = offset / SPU_RAM_ADDR_UNIT;
        SPU_CTRL     = ctrlReg;
        _waitForStatus(SPU_CTRL_XFER_BITMASK, SPU_STAT_XFER_DMA_WRITE);

        DMA_MADR(DMA_SPU) = reinterpret_cast<uint32_t>(data);
        DMA_BCR(DMA_SPU)  = chunkSize | (numChunks << 16);
        DMA_CHCR(DMA_SPU) = 0 | DMA_CHCR_WRITE | DMA_CHCR_MODE_SLICE | DMA_CHCR_ENABLE;

        if (wait)
            psx_waitForDMATransfer(DMA_SPU, _DMA_TIMEOUT);

        return length * _DMA_CHUNK_SIZE * 4;
    }

    bool SPUDriver::initFromVAGHeader(const VAGHeader *header, uint32_t ramOffset)
    {
        if (header->magic != util::concat4('V', 'A', 'G', 'p'))
            return false;

        // offset     = ramOffset / 8;
        // sampleRate = (__builtin_bswap32(header->sampleRate) << 12) / 44100;
        // length     = __builtin_bswap32(header->length);

        return true;
    }

    SPUDriver::Channel SPUDriver::spu_play(uint16_t left, uint16_t right, Channel ch) const
    {
        if ((ch < 0) || (ch >= NUM_CHANNELS))
            return -1;
        // if (!offset)
        //    return -1;

        SPU_CH_VOLL(ch) = left;
        SPU_CH_VOLR(ch) = right;
        // SPU_CH_PITCH(ch) = sampleRate;
        // SPU_CH_SSA(ch)   = offset;
        SPU_CH_ADSR1(ch) = 0x00ff;
        SPU_CH_ADSR2(ch) = 0x0000;

        if (ch < 16)
            SPU_KON0 = 1 << ch;
        else
            SPU_KON1 = 1 << (ch - 16);

        return ch;
    }

    bool SPUDriver::play(Audio::SoundObject *sObj)
    {

        return false;
    }

    bool SPUDriver::stop(Audio::SoundObject *sObj)
    {

        return false;
    }

    bool SPUDriver::pause(Audio::SoundObject *sObj)
    {

        return false;
    }

    bool SPUDriver::isPlaying(Audio::SoundObject *sObj)
    {

        return false;
    }

    int SPUDriver::uploadSample(Audio::SoundObject *sObj)
    {

        return GV_OK;
    }

    bool SPUDriver::reset()
    {
        return true;
    }

    void SPUDriver::shutdown()
    {
    }

} // namespace PSX