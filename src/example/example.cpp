/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * example.CPP - Created on 26-07-2025
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

#include "app/app.hpp"

using namespace Apps;

class GenV_Demo : public Application
{
private:
    const char *appName = "GenV_Demo (NRC)";
    AppVersion appVer = AppVersion(0, 0, 1);
    Coord txtOrigin;

public:
    GenV_Demo() : Application() {}

    int init() override
    {
        state = APP_STATE_RUN;
        reload();
        return 0;
    }
    void update() override {}
    void render() override
    {
        gpu->fillScreen(Colors::Black);
        gpu->drawText("This is an example string.", 27, txtOrigin.x, txtOrigin.y, 500, 100, Colors::White, Video::TALIGN_CENTER);
    }
    void reload() override
    {
        txtOrigin = Coord((gpu->getHorizontalRes() / 2) - 250,
                          (gpu->getVerticalRes() / 2) - 50);
    }
    void shutdown() override {}

    const char *name() override { return appName; }
    int version() override { return appVer.toInt(); }
};

extern "C++" Apps::Application *genv_register_app()
{
    static GenV_Demo *app = new GenV_Demo;
    return app;
}
