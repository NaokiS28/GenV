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
#include "app/iapp_host.hpp"
#include "common/util/rect.h"

namespace Apps
{
    class TextLoader : public LoadScreenApp
    {
    private:
        static constexpr AppInfo appInfo = makeAppInfo(
            "TextLoader (NRC)", // name
            "NaokiS",           // maker
            AppVersion(0, 0, 1) // version
        );

        const char *loadingText = "NOW LOADING...";

        RectWH textPos;

    public:
        static LoadScreenApp *createApp() { return new TextLoader; }
        static LoadScreenApp *createApp(Application *app) { return new TextLoader(app); }
        static constexpr const AppInfo &infoStatic() { return appInfo; }

        TextLoader();
        TextLoader(Application *appToLoad);

        void render();
        void reload();

        const AppInfo &info() const override { return appInfo; }
    };
}