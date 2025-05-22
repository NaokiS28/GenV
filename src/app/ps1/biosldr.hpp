/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * biosldr.hpp - Created on 12-05-2025
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

#pragma once

#include "app/app.hpp"

using namespace Apps;

class PSXLoader : public AppLoader
{
private:
    const char *appName = "PSX Loader";
    const AppVersion appVer = AppVersion(0, 0, 1);

    // AudioObject* bootChime;
    // SpriteObject* makerLogo;
    // SpriteObject* makerText;
    // ModelObject* diamond[3];

public:
    PSXLoader();
    ~PSXLoader(){}

    int version() override { return appVer.toInt(); };               // Return app's version for logging
    const char *name() override { return appName; };          // Return app's name for logging
    
    int init() override { return 0; }
    void update() override {}
    void render() override {}
};