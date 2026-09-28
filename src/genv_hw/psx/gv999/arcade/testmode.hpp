/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * testmode.hpp - Created on 02-08-2025
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

using namespace Apps;

class GVTestApp : public ArcadeTestApp
{
private:
    static constexpr AppInfo appInfo = makeAppInfo(
        "Konami GV Test Mode", // name
        "NaokisRC",            // maker
        AppVersion(0, 0, 1)    // version
    );

public:
    static ArcadeTestApp *createArcadeApp(IAppHost *host) { return new GVTestApp(host); }
    static Application *createApp(IAppHost *host) { return new GVTestApp(host); }
    static constexpr const AppInfo &infoStatic() { return appInfo; }

    GVTestApp(IAppHost *host);

    static inline ArcadeTestApp *create(IAppHost *host)
    {
        return new GVTestApp(host);
    }

    int init() override;
    void update(void) override;
    void render(void) override;

    const AppInfo &info() const override { return appInfo; }
};