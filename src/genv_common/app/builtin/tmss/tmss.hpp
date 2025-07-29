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
#include "common/util/tween.hpp"

namespace Apps
{
    constexpr const int iTimeToShow = 4000;
    constexpr const int iFadeTime = 500;
    
    class TMSS : public LoadScreenApp
    {
    private:
        const char *appName = "TMSS(NRC)";
        AppVersion appVer = AppVersion(0, 0, 1);
        int timer = -1;

        enum {
            TMSS_FadeIn,
            TMSS_Delay,
            TMSS_FadeOut,
            TMSS_Exit
        } tmssAnimStep = TMSS_FadeIn; 

        uint8_t alpha = 0;
        Util::Tween<uint16_t, Util::QuadInEasing> fadeIn;
        Util::Tween<uint16_t, Util::QuadOutEasing> fadeOut;

        const char *tmssText = "CREATED WITH\nOR RUNNING UNDER\nGEN-V MULTIPLATFORM ENGINE.";

        RectWH textPos;

    public:
        TMSS();

        int init();
        void update();
        void render();
        void reload();
        void shutdown() {}

        const char *name() { return appName; }
        int version() { return appVer.toInt(); }
    };
}