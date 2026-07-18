/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * system.cpp - Created on 25-04-2025
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

#include "system.hpp"

#include "psx/common/system.hpp"
#include "psx/common/psx_strings.hpp"

namespace PSX
{
    int PSXSystem::initVideo()
    {
        int error = 0;
        //! Review
        // The GPU driver now allocates its own screen(s) inside init() via
        // System::assignScreen, so the system just constructs the driver (injecting
        // itself with *this) and inits it. Screen 0 registration moved into PSXGPU::init().
        gpu   = new GPU::PSXGPU(*this);
        error = ioTest(gpu, PSX_GPU_STR, PSX_CREATE_STR);
        if (!error) ioTest(gpu->init(), PSX_GPU_STR, PSX_INIT_STR);
        //! End
        return error;
    }

    int PSXSystem::initAudio()
    {
        /*
        int error = 0;
        spu       = new Sound::PSXSPU;
        error     = ioTest(spu, PSX_SPU_STR, PSX_CREATE_STR);
        if (!error) ioTest(spu->init(), PSX_SPU_STR, PSX_INIT_STR);
        if (!error) services.setAudio(adminKey, spu);
        */
        return 0;
    }

    int PSXSystem::initStorage()
    {
        BasePSXSystem::initStorage();
        int error = 0; // TODO: How to handle multiple driver failures?
        cdDriver  = new Storage::PSX_CDROM(*this);
        error     = ioTest(cdDriver, PSX_CDROM_DRIVE_STR, PSX_CREATE_STR);
        if (!error) error = ioTest(cdDriver->init(), PSX_CDROM_DRIVE_STR, PSX_INIT_STR);
        if (!error) registerDriver(cdDriver);

#ifndef NDEBUG
        int pcError = 0;
        pcDriver    = new Storage::PSX_PCDrive(*this);
        pcError     = ioTest(pcDriver, PSX_PC_DRIVE_STR, PSX_CREATE_STR);
        if (!pcError) pcError = ioTest(pcDriver->init(), PSX_PC_DRIVE_STR, PSX_INIT_STR);
        if (!pcError) registerDriver(pcDriver);
#endif
        return error;
    }

} // namespace PSX