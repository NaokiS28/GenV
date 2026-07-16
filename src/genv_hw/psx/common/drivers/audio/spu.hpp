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

#pragma once

#include <stddef.h>
#include <stdint.h>
#include "common/services/audio/iface_audio.hpp"
#include "registers.hpp"

namespace PSX
{

    /*
     * PlayStation 1 SPU Driver
     * ===========================
     * Status: Broken, untested. Should be easy enough to fix and get working.
     * This driver controls the SPU of the PlayStation 1 for audio playback.
     */

    // TODO: Most of the SPU code itself should probably work but it needs tidiying up and the commented out params need correcting.

    static constexpr uint32_t DUMMY_BLOCK_OFFSET = 0x1000;
    static constexpr uint32_t DUMMY_BLOCK_END    = 0x1010;

    static constexpr int NUM_CHANNELS    = 24;
    static constexpr uint16_t MAX_VOLUME = 0x3fff;

    static constexpr util::Hash GENV_PSX_SOUND_OBJ_TYPENAME = "PSXSoundObject"_h;

    struct VAGHeader
    {
    public:
        uint32_t magic, version, interleave, length, sampleRate;
        uint16_t _reserved[5], channels;
        char name[16];
    };

    class SPUDriver : public Audio::IAudio
    {
        using Channel = int;

    private:
        uint32_t nextSampleID = 1; // start from 1 to reserve 0 for "invalid"

        static inline void setMasterVolume(uint16_t master, uint16_t reverb = 0)
        {
            SPU_MVOLL = master;
            SPU_MVOLR = master;
            SPU_EVOLL = reverb;
            SPU_EVOLR = reverb;
        }

        static inline void setChannelVolume(Channel ch, uint16_t left, uint16_t right)
        {
            if ((ch < 0) || (ch >= NUM_CHANNELS))
                return;

            SPU_CH_VOLL(ch) = left;
            SPU_CH_VOLR(ch) = right;
        }

        Channel getFreeChannel(void) const;
        void stopChannel(Channel ch);
        void resetAllChannels(void);
        size_t upload(uint32_t ramOffset, const void *data, size_t length, bool wait);

        inline Channel spu_play(
            uint16_t left = MAX_VOLUME, uint16_t right = MAX_VOLUME) const
        {
            return spu_play(left, right, getFreeChannel());
        }

        bool initFromVAGHeader(const VAGHeader *header, uint32_t ramOffset);
        Channel spu_play(uint16_t left, uint16_t right, Channel ch) const;

    public:
        bool init() override;
        bool reset() override;
        void shutdown() override;

        bool play(Audio::SoundObject *sObj) override;
        bool stop(Audio::SoundObject *sObj) override;
        bool pause(Audio::SoundObject *sObj) override;
        bool isPlaying(Audio::SoundObject *sObj) override;

        int uploadSample(Audio::SoundObject *sObj) override;

        inline bool pause()
        {
            return false; // return (buffer == nullptr ? false : SUCCEEDED(buffer->Stop()));
        }
    };

} // namespace PSX