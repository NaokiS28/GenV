/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * system.hpp - Created on 25-04-2025
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

#include <stdbool.h>

#include "common/services/services.hpp"
#include "common/services/system/system.hpp"
#include "psx/common/drivers/sio0/psx_joy.hpp"
#include "psx/common/drivers/pcdrive/psx_pcdrv.hpp"
#include "psx/common/psx_strings.hpp"
#include "psx/common/system.hpp"
#include "psx_cdrom.hpp"

namespace PSX
{
    using namespace System;

    class PSXSystem : public BasePSXSystem
    {
    protected:
        SystemInfo siPS1 = {
            .type  = SYS_Console,
            .make  = szSony,
            .name  = szPlaystation,
            .flags = SYS_No_Window_Mode};

        Storage::PSX_CDROM *cdDriver   = nullptr; // CD Driver should be pointer to handle PS1/IDE/SCSI drivers
        Storage::PSX_PCDrive *pcDriver = nullptr; // Not always needed?

    public:
        inline PSXSystem(ServiceManager &services)
            : BasePSXSystem(services) {};

        virtual int initVideo() override;
        virtual int initAudio() override;
        virtual int initStorage() override;

        inline virtual const SystemInfo *getSysInfo() const override
        {
            return &siPS1;
        }
    };

} // namespace PSX
