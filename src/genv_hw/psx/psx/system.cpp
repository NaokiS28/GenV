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

namespace PS1
{
    int PS1System::initVideo()
    {
        int error = 0;
        gpu       = new GPU::PS1GPU(*this);
        error     = ioTest(gpu, PS1_GPU_STR, PS1_CREATE_STR);
        if (!error) ioTest(gpu->init(), PS1_GPU_STR, PS1_INIT_STR);
        return error;
    }

    int PS1System::initAudio()
    {
        /*
        int error = 0;
        spu       = new Sound::PS1SPU;
        error     = ioTest(spu, PS1_SPU_STR, PS1_CREATE_STR);
        if (!error) ioTest(spu->init(), PS1_SPU_STR, PS1_INIT_STR);
        if (!error) services.setAudio(adminKey, spu);
        */
        return 0;
    }

    int PS1System::initStorage()
    {
        BasePS1System::initStorage();
        int error = 0; // TODO: How to handle multiple driver failures?
        cdDriver  = new Storage::PS1_CDROM(*this);
        error     = ioTest(cdDriver, PS1_CDROM_DRIVE_STR, PS1_CREATE_STR);
        if (!error) registerDriver(cdDriver);

#ifndef NDEBUG
        int pcError = 0;
        pcDriver    = new Storage::PS1_PCDrive(*this);
        pcError     = ioTest(pcDriver, PS1_PC_DRIVE_STR, PS1_CREATE_STR);
        if (!pcError) registerDriver(pcDriver);
#endif
        return error;
    }

} // namespace PS1