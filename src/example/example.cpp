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

public:
    static Application *createApp()
    {
        return new GenV_Demo;
    }
    static constexpr const AppInfo &infoStatic()
    {
        return appInfo;
    }

    GenV_Demo() : Application() {}

    int init(IAppHost *host) override
    {
        setHost(host);
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
            LOG("clock", "tick");
            lastSeconds = time.tm_sec;
        }
    }

    void render() override
    {
        gpu->fillScreen(Video::Colors::Black);
        gpu->drawText("This is an example string.", txtOrigin.x, txtOrigin.y, 500, 100, Video::Colors::White, Video::TALIGN_CENTER);
    }

    void reload() override
    {
        txtOrigin = Coord((gpu->getHorizontalRes() / 2),
                          (gpu->getVerticalRes() / 2) - 50);
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
