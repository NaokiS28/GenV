/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * directsound.hpp - Created on 25-05-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#include <windows.h>
#include <dsound.h>

#include "directsound.hpp"

bool DirectSound::LoadWavFile(const char* filename) {
    HMMIO hmmio = mmioOpenA((LPSTR)filename, nullptr, MMIO_READ | MMIO_ALLOCBUF);
    if (!hmmio) return false;

    MMCKINFO parent, child;
    parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
    if (mmioDescend(hmmio, &parent, nullptr, MMIO_FINDRIFF)) return false;

    child.ckid = mmioFOURCC('f', 'm', 't', ' ');
    if (mmioDescend(hmmio, &child, &parent, MMIO_FINDCHUNK)) return false;

    WAVEFORMATEX wf = {};
    mmioRead(hmmio, (char*)&wf, sizeof(wf));
    mmioAscend(hmmio, &child, 0);

    child.ckid = mmioFOURCC('d', 'a', 't', 'a');
    if (mmioDescend(hmmio, &child, &parent, MMIO_FINDCHUNK)) return false;

    BYTE* wavData = new BYTE[child.cksize];
    mmioRead(hmmio, (char*)wavData, child.cksize);
    mmioClose(hmmio, 0);

    DSBUFFERDESC dsbd = {};
    dsbd.dwSize = sizeof(DSBUFFERDESC);
    dsbd.dwFlags = DSBCAPS_CTRLVOLUME;
    dsbd.dwBufferBytes = child.cksize;
    dsbd.lpwfxFormat = &wf;

    if (FAILED(dsdev->CreateSoundBuffer(&dsbd, &dsbuffer, nullptr)))
        return false;

    VOID* ptr1, *ptr2;
    DWORD len1, len2;
    if (SUCCEEDED(dsbuffer->Lock(0, child.cksize, &ptr1, &len1, &ptr2, &len2, 0))) {
        memcpy(ptr1, wavData, len1);
        if (ptr2) memcpy(ptr2, wavData + len1, len2);
        dsbuffer->Unlock(ptr1, len1, ptr2, len2);
    }

    delete[] wavData;
    return true;
}

DirectSound::DirectSound(WindowObject *hWnd)
{
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

void DirectSound::setWindow(WindowObject *hWnd)
{
    if (hWnd)
        gpuWnd = hWnd;
}

bool DirectSound::reset()
{
}

void DirectSound::shutdown()
{
    if (dsbuffer)
    {
        dsbuffer->Release();
        dsbuffer = nullptr;
    }
    if (dsdev)
    {
        dsdev->Release();
        dsdev = nullptr;
    }
}