/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sound.hpp - Created on 27-05-2025
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

#include "common/util/hash.hpp"
#include "object.hpp"
#include "file.hpp"
#include "common/formats/riff.hpp"
#include "common/return_codes.hpp"

namespace Audio
{
    static constexpr util::Hash GENV_SOUND_OBJ_TYPENAME = "GenVSoundObject"_h;

    enum
    {
        SO_BITDEPTH_8 = 8,
        SO_BITDEPTH_16 = 16,
        SO_BITDEPTH_32 = 32
    };

    constexpr uint8_t alwaysLoop = UINT8_MAX;

    struct SampleMeta
    {
        const char *name = nullptr; // Sample name, if needed
        uint8_t channels = 0;       // Mono, Stereo
        uint8_t bitDepth = 0;       // The bitdepth of the sound, IE 8-Bits per sample
        uint32_t sampleRate = 0;    // The sample rate in Hz (eg 41,600)
        size_t sampleLength = 0;    // How many samples long the sound is
    };

    // Returns the given sameple's play length in seconds.
    constexpr size_t getSampleTime(SampleMeta meta)
    {
        size_t time = (meta.sampleLength / meta.sampleRate);
        return (time ? time : 1);
    }

    class SoundObject : public ObjectBase
    {
    public:
        SoundObject(util::Hash objectID) : ObjectBase(objectID)
        {
            file = new Files::FileObject(objectID);
            setObjectType(GENV_SOUND_OBJ_TYPENAME);
        }
        SoundObject(util::Hash objectID, const char *filePath) : ObjectBase(objectID)
        {
            file = new Files::FileObject(objectID);
            if (file != nullptr)
                loadSoundFile(filePath);
            setObjectType(GENV_SOUND_OBJ_TYPENAME);
        }
        SoundObject(Files::FileObject *fObj) : ObjectBase(fObj->getObjectID())
        {
            noDeleteFile = true;
            file = fObj;
            setObjectType(GENV_SOUND_OBJ_TYPENAME);
        }
        virtual ~SoundObject()
        {
            if (!noDeleteFile)
            {
                delete file;
            }
        }

        virtual bool play();
        virtual bool stop();
        virtual bool pause();
        virtual bool isPlaying();

        virtual int loadSoundFile(const char *filePath)
        {
            return file->openFile(filePath, false);
        }

        virtual int uploadSample();
        virtual size_t getPlayPosition() { return currentPos; }
        virtual size_t getPlayTime() { return getSampleTime(meta); }

        inline bool restart()
        {
            return (stop() && play());
        }

        inline bool isValid()
        {
            return valid;
        }

        inline void *getSampleData()
        {
            return sampleData;
        }

        inline SampleMeta getMeta()
        {
            return meta;
        }

    private:
        bool noDeleteFile = false;

    protected:
        Files::FileObject *file;
        SampleMeta meta;

        void *sampleData = nullptr;

        bool valid = false;
        bool loop = false;
        uint8_t loopCount = 0;
        size_t loopStartPoint = 0;
        size_t loopEndPoint = 0;
        size_t currentPos = 0;
        size_t startPoint = 0;
        size_t endPoint = 0;
    };

    SoundObject *createSample(util::Hash objectID, const char *filePath);
} // namespace Audio