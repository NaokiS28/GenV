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

#include <stdint.h>
#include "app/app.hpp"

using namespace Apps;

class GenVTestApp : public ArcadeTestApp
{
private:
    const char *appName = "Test Mode (NRC)";
    AppVersion appVer = AppVersion(0, 0, 1);

public:
    GenVTestApp() : ArcadeTestApp() {}
    int init(void) override;
    void update(void) override;
    void render(void) override;

    const char *name(void) override { return appName; }
    int version(void) override { return appVer.toInt(); }
};