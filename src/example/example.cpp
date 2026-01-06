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

#include "common/services/services.hpp"
#include "common/services/system/system.hpp"
#include "common/util/time.hpp"
#include <stdio.h>
#include <time.h>
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

    Coord txtOrigin;
    Coord timeOrigin;

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

    int init() override
    {
        setAppState(APP_STATE_RUN);
        reload();
        // Textures::TextureObject *textTest[256];
        // for (int i = 0; i < 256; i++)
        //{
        //    textTest[i] = Textures::createDefaultTexture();
        //    textTest[i]->uploadTexture();
        //}
        return 0;
    }

    void update() override
    {
        tm time;
        System::getTime(time);
        static int lastSeconds = 0;
        if (time.tm_sec != lastSeconds)
        {
            // LOG("clock", "tick");
            lastSeconds = time.tm_sec;
        }
    }

    void render() override
    {
        char timeStr[20] = {'\0'};
        char padStr[16] = {'\0'};
        char padNameStr[35 * 8] = {0};
        tm time;
        System::getTime(time);
        Time::getTimeString(time, timeStr, 20, true, true);

        auto padCount = getServiceManager()->inputManager()->deviceCount();
        snprintf(padStr, 16, "Pads: %d", padCount);
        for (size_t i = 0; i < padCount; i++)
        {
            char _name[32];
            snprintf(_name, 32, "%d: %s", i, getServiceManager()->inputManager()->deviceName(i));
            strncat(padNameStr, _name, 32);
            strncat(padNameStr, "\r\n", 2);
        }

        gpu->fillScreen(Video::Colors::Black);
        gpu->drawText("This is an example string.", txtOrigin.x, txtOrigin.y, 500, 100, Video::Colors::White, Video::TALIGN_CENTER);
        gpu->drawText(timeStr, timeOrigin.x, timeOrigin.y, 100, 100, Video::Colors::White, Video::TALIGN_CENTER);
        gpu->drawText(padStr, timeOrigin.x, timeOrigin.y + 10, 500, 100, Video::Colors::White, Video::TALIGN_CENTER);
        gpu->drawText(padNameStr, timeOrigin.x, timeOrigin.y + 20, 500, 100, Video::Colors::White, Video::TALIGN_CENTER);
    }

    void reload() override
    {
        txtOrigin = Coord(
            (gpu->getHorizontalRes() / 2) - 50,
            (gpu->getVerticalRes() / 2));
        timeOrigin = Coord(
            5,
            5);
    }
    void shutdown() override {}

    const AppInfo &info() const override
    {
        return appInfo;
    }
};

extern "C++" int genv_register_apps(Apps::IAppHost *host)
{
    if (!host)
        return -1;
    host->registerApplicationFactory(GenV_Demo::createApp, &GenV_Demo::infoStatic(), APP_SCREEN_TITLE);
    return 0;
}
