/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * genv_demo.hpp - Created on 02-05-2026
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
#include "common/util/rect.hpp"
#include "demo_page.hpp"
#include <cstdint>
#include <genv.hpp>

using namespace Apps;

class GenV_Demo : public Application
{
private:
    static constexpr AppInfo appInfo = makeAppInfo(
        "GenV_Demo",        // name
        "NaokisRC",         // maker
        AppVersion(0, 0, 1) // version
    );

    DemoPage *currentPage  = nullptr;
    uint8_t currentMenuPos = 0;

    bool selectIgnore = false;

    Video::RectWH titleBox;
    Video::RectWH sysBox;
    Video::RectWH timeBox;
    tm time;

    Video::RectWH menuBox;

public:
    static Application *createApp(IAppHost *host)
    {
        return new GenV_Demo(host);
    }
    static constexpr const AppInfo &infoStatic()
    {
        return appInfo;
    }

    GenV_Demo(IAppHost *host) : Application(host) {}

    int init() override;
    void update() override;
    void render() override;
    void reload() override;

    void shutdown() override {}

    const AppInfo &info() const override
    {
        return appInfo;
    }
};