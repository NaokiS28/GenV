/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * directsound.hpp - Created on 25-05-2025
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

#include <windows.h>
#include <dsound.h>

#include "directsound.hpp"

LPDIRECTSOUNDBUFFER DirectSound::getSoundBuffer(Audio::SoundObject *sObj)
{
    auto it = dsbufferMap.find(sObj->getSampleID());
    if (it == dsbufferMap.end())
        return nullptr;

    LPDIRECTSOUNDBUFFER buffer = it->second;
    return buffer;
}

bool DirectSound::play(Audio::SoundObject *sObj)
{
    auto buffer = getSoundBuffer(sObj);
    if (buffer != nullptr)
        return (
            SUCCEEDED(buffer->SetCurrentPosition(0)) &&
            SUCCEEDED(buffer->Play(0, 0, 0)));
    return false;
}

bool DirectSound::stop(Audio::SoundObject *sObj)
{
    auto buffer = getSoundBuffer(sObj);
    if (buffer != nullptr)
        return (pause(buffer) && SUCCEEDED(buffer->SetCurrentPosition(0)));
    return false;
}

bool DirectSound::pause(Audio::SoundObject *sObj)
{
    auto buffer = getSoundBuffer(sObj);
    return ((buffer != nullptr) ? SUCCEEDED(buffer->Stop()) : false);
}

bool DirectSound::isPlaying(Audio::SoundObject *sObj)
{
    auto buffer = getSoundBuffer(sObj);
    if (buffer != nullptr)
    {
        DWORD status;
        if (SUCCEEDED(buffer->GetStatus(&status)))
            return (status & DSBSTATUS_PLAYING);
    }
    return false;
}

int DirectSound::uploadSample(Audio::SoundObject *sObj)
{
    if (sObj == nullptr)
        return Audio::IA_ERROR_BADOBJECT;

    SampleMeta meta = sObj->getMeta();

    WAVEFORMATEX wf;
    ZeroMemory(&wf, sizeof(wf));

    wf.nChannels = meta.channels;
    wf.wFormatTag = WAVE_FORMAT_PCM;
    wf.wBitsPerSample = meta.bitDepth;
    wf.nSamplesPerSec = meta.sampleRate;
    wf.nBlockAlign = (wf.wBitsPerSample * wf.nChannels) / 8;
    wf.nAvgBytesPerSec = (wf.nSamplesPerSec * wf.nBlockAlign);

    DSBUFFERDESC dsbd = {};
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = meta.sampleLength;
    dsbd.lpwfxFormat = &wf;

    LPDIRECTSOUNDBUFFER sndBuff;
    if (FAILED(dsdev->CreateSoundBuffer(&dsbd, &sndBuff, nullptr)))
        return Audio::IA_ERROR_OUTOFMEMORY;

    VOID *ptr1, *ptr2;
    DWORD len1, len2;
    if (SUCCEEDED(sndBuff->Lock(0, meta.sampleLength, &ptr1, &len1, &ptr2, &len2, 0)))
    {
        memcpy(ptr1, sObj->getSampleData(), len1);
        if (ptr2)
            memcpy(ptr2, sObj->getSampleData() + len1, len2);
        sndBuff->Unlock(ptr1, len1, ptr2, len2);
    }
    else
    {
        return Audio::IA_ERROR_NOLOCK;
    }

    uint32_t id = nextSampleID++;
    dsbufferMap[id] = sndBuff;
    sObj->setSampleID(id);

    return Audio::IA_OKAY;
}

DirectSound::DirectSound(WindowObject *wObj)
{
    this->gpuWnd = wObj;
}

DirectSound::~DirectSound()
{
    shutdown();
}

bool DirectSound::init()
{
    if (FAILED(DirectSoundCreate8(nullptr, &dsdev, nullptr)))
        return false;

    if (FAILED(dsdev->SetCooperativeLevel(gpuWnd->hWnd, DSSCL_PRIORITY)))
        return false;

    return true;
}

bool DirectSound::reset()
{
    return true;
}

void DirectSound::shutdown()
{
    for (auto buffer : dsbufferMap)
    {
        buffer.second->Release();
    }
    dsbufferMap.clear();

    if (dsdev)
    {
        dsdev->Release();
        dsdev = nullptr;
    }
}