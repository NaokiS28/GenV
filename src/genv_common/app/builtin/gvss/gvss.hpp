/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * GVSS.hpp - Created on 10-06-2025
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
#include "app/iapp_host.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/texture.hpp"
#include "common/util/rect.h"
#include "common/util/tween.hpp"

namespace Apps
{
    constexpr const int iTimeToShow = 4000;
    constexpr const int iFadeTime = 500;

    class GVSS : public LoadScreenApp
    {
    private:
        static constexpr AppInfo appInfo = makeAppInfo(
            "GVSS",             // name
            "NaokisRC",         // maker
            AppVersion(0, 1, 1) // version
        );
        int timer = -1;

        enum
        {
            GVSS_FadeIn,
            GVSS_Delay,
            GVSS_FadeOut,
            GVSS_Exit
        } GVSSAnimStep = GVSS_FadeIn;

        uint8_t alpha = 0;
        Util::Tween<uint16_t, Util::QuadInEasing> fadeIn;
        Util::Tween<uint16_t, Util::QuadOutEasing> fadeOut;

        Sprites::SpriteObject *logo = nullptr;

        const char *GVSSText = "CREATED WITH\n\rOR RUNNING UNDER\n\rGEN-V MULTIPLATFORM ENGINE.";

        RectWH logoPos;
        RectWH textPos;

    public:
        static LoadScreenApp *createApp(IAppHost *host, Application *app) { return new GVSS(host, app); }
        static constexpr const AppInfo &infoStatic() { return appInfo; }

        GVSS(IAppHost *host, Application *appToLoad);

        int init() override;
        void update() override;
        void render() override;
        void reload() override;

        const AppInfo &info() const override { return appInfo; }
    };
} // namespace Apps