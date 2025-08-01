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

namespace Apps {
class TextLoader : public LoadScreenApp
{
private:
    const char *appName = "TextLoader(NRC)";
    AppVersion appVer = AppVersion(0, 0, 1);

    const char *loadingText = "NOW LOADING...";

    RectWH textPos;

public:
    TextLoader();

    int init(){ return 0; }
    void update(){}
    void render();
    void reload();
    void shutdown(){}

    const char *name() { return appName; }
    int version() { return appVer.toInt(); }
};
}