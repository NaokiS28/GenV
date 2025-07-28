/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * tmss.hpp - Created on 10-06-2025
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
#include "app/app.hpp"
#include "common/util/rect.h"

namespace Apps
{
    constexpr const int iTimeToShow = 4000;
    
    class TMSS : public LoadScreenApp
    {
    private:
        const char *appName = "TMSS(NRC)";
        AppVersion appVer = AppVersion(0, 0, 1);
        uint32_t timer = 0;

        const char *tmssText = "CREATED WITH\nOR RUNNING UNDER\nGEN-V MULTIPLATFORM ENGINE.";

        RectWH textPos;

    public:
        TMSS();

        void update();
        void render();
        void reload();
        void shutdown() {}

        const char *name() { return appName; }
        int version() { return appVer.toInt(); }
    };
}