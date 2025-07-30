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

#include "object.hpp"
#include "file.hpp"
#include "common/formats/riff.hpp"

namespace Audio
{
    enum : uint8_t
    {
        SO_OKAY,                // Success
        SO_ERROR_OUTOFMEMORY,   // Couldn't allocate memory for file
        SO_ERROR_BADPERMISSION, // Permissions on file denied this action
        SO_ERROR_BADPATH,       // File path was invalid, badly formatted or has invalid characters
        SO_ERROR_FILENOTFOUND,  // File was not found
        SO_ERROR_BADOBJECT,     // DataObject is bad
        SO_ERROR_BADPARAM,      // A parameter provided did not meet the requirements
        SO_ERROR_INCOMPATIBLE,  // File type is incompatible with the target object
        SO_ERROR_BADNAME,       // Filename is invalid
        SO_ERROR_FILEINUSE,     // File is already open and locked for writing
        SO_ERROR_NOLOCK,        // File is not locked or lock could not be set
        SO_ERROR_INVALIDFUNC,   // Function has not been provided on this file provider.
        SO_ERROR_UNKNOWN        // Unknown or unexpected error occured
    };

    enum {
        SO_BITDEPTH_8 = 8,
        SO_BITDEPTH_16 = 16,
        SO_BITDEPTH_32 = 32
    };

    constexpr uint8_t alwaysLoop = UINT8_MAX;

    struct SampleMeta {
        const char *name = nullptr;     // Sample name, if needed
        uint8_t     channels = 0;       // Mono, Stereo
        uint8_t     bitDepth = 0;       // The bitdepth of the sound, IE 8-Bits per sample
        uint32_t    sampleRate = 0;     // The sample rate in Hz (eg 41,600)
        size_t      sampleLength = 0;   // How many samples long the sound is
        uint32_t    id = 0;             // ID for the audio driver to use (set and used by audio driver)
    };

    // Returns the given sameple's play length in seconds.
    constexpr size_t getSampleTime(SampleMeta meta){
        size_t time = (meta.sampleLength / meta.sampleRate);
        return (time ? time : 1);
    }

    class SoundObject : public ObjectBase
    {
    public:
        SoundObject() : ObjectBase() {
            file = new Files::FileObject();
        }
        SoundObject(const char *filePath) : ObjectBase() {
            file = new Files::FileObject();
            if(file != nullptr)
                loadSoundFile(filePath);
        }
        SoundObject(Files::FileObject *fObj) : ObjectBase(){
            noDeleteFile = true;
            file = fObj;
        }
        virtual ~SoundObject() {
            if(!noDeleteFile){
                delete file;
            }
        }

        virtual bool play();
        virtual bool stop();
        virtual bool pause();
        virtual bool isPlaying();


        virtual int loadSoundFile(const char *filePath){ 
            return file->openFile(filePath, false);
        }

        virtual int uploadSample();
        virtual size_t getPlayPosition(){ return currentPos; }
        virtual size_t getPlayTime(){ return getSampleTime(meta); }

        inline bool restart(){
            return (stop() && play());
        }

        inline bool isValid(){
            return valid;
        }

        inline void *getSampleData(){
            return sampleData;
        }

        inline SampleMeta getMeta(){
            return meta;
        }

        inline void setSampleID(uint32_t id){
            meta.id = id;
        }

        inline uint32_t getSampleID(){
            return meta.id;
        }

    private:
        bool noDeleteFile = false;

    protected:
        Files::FileObject *file;
        SampleMeta meta;

        void   *sampleData = nullptr;

        bool    valid = false;
        bool    loop = false;
        uint8_t loopCount = 0;
        size_t  loopStartPoint = 0;
        size_t  loopEndPoint = 0;
        size_t  currentPos = 0;
        size_t  startPoint = 0;
        size_t  endPoint = 0;
    };

    SoundObject *createSample(const char *filePath);
}