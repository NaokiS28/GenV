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

#include "common/services/io/iface_input.hpp"
#include "common/services/io/inputman.hpp"
#include "common/services/services.hpp"
#include "common/services/system/system.hpp"
#include "common/util/time.hpp"
#include <cstdint>
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

    Input::VPad *pads;

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
        pads = &services.getInputs()->vpad;

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
        tm time;
        System::getTime(time);
        Time::getTimeString(time, timeStr, 20, true, true);

        auto padCount = getServiceManager()->getInputs()->deviceCount();
        snprintf(padStr, 16, "Pads: %d", padCount);

        int x = timeOrigin.x;
        int y = timeOrigin.y;

        gpu->fillScreen(Video::Colors::Black);
        gpu->drawText("This is an example string.", txtOrigin.x, txtOrigin.y, 500, 20, Video::Colors::White, Video::TALIGN_CENTER);
        gpu->drawText(timeStr, x, y += 10, 200, 20, Video::Colors::White, Video::TALIGN_CENTER);
        gpu->drawText(padStr, x, y += 10, 200, 20, Video::Colors::White, Video::TALIGN_CENTER);

        for (size_t i = 0; i < padCount; i++)
        {
            char line[64] = {0};
            int player = getServiceManager()->getInputs()->devicePlayer(i);
            if (player > 0 && player < 10)
                snprintf(line, 64, "%d: %s, assigned to player %d", (int)i, getServiceManager()->getInputs()->deviceName(i), player);
            else
                snprintf(line, 64, "%d: %s, not assigned to player.", (int)i, getServiceManager()->getInputs()->deviceName(i));
            gpu->drawText(line, x, y += 10, 500, 20, Video::Colors::White, Video::TALIGN_CENTER);
        }

        printControllerData(x, y);
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

    void printControllerData(int x, int y)
    {
        static uint8_t digitalCount[8] = {0};
        static uint8_t analogCount[8] = {0};
        static uint8_t rotaryCount[8] = {0};

        if (pads->devicesChanged())
        {
            for (int i = 0; i < 8; i++)
            {
                auto player = Input::playerIndexToFlag(i);
                if (!pads->isPlayerAvailable(player)) continue;
                digitalCount[i] = pads->getPlayerDigitalCount(player);
                analogCount[i] = pads->getPlayerAnalogCount(player);
                rotaryCount[i] = pads->getPlayerRotaryCount(player);
            }
        }

        for (int i = 0; i < 8; i++)
        {
            auto p = Input::playerIndexToFlag(i);
            if (pads->isPlayerAvailable(p))
            {
                char text[32] = {0};
                snprintf(text, 32, "Player: %i:", i + 1);
                gpu->drawText(text, x, y += 10, 500, 20, Video::Colors::White, Video::TALIGN_CENTER);

                uint8_t digitalBanks = (digitalCount[i] / 32) + 1;
                if (digitalCount[i] != 0)
                {
                    for (uint8_t d = 0; d < digitalBanks; d++)
                    {
                        uint32_t inputs = 0;
                        pads->getPlayerDigitalInputs(inputs, p);
                        snprintf(text, 32, "Digital %i: 0x%08X", d, inputs);
                        gpu->drawText(text, x + 10, y += 10, 500, 20, Video::Colors::White, Video::TALIGN_CENTER);
                    }
                }

                if (analogCount[i] != 0)
                {
                    for (int a = 0; a < analogCount[i]; a++)
                    {
                        int16_t analog = 0;
                        pads->getPlayerAnalogInputs(analog, p, a);
                        snprintf(text, 32, "Analog %i: %i", a, analog);
                        gpu->drawText(text, x + 10, y += 10, 500, 20, Video::Colors::White, Video::TALIGN_CENTER);
                    }
                }

                if (rotaryCount[i] != 0)
                {
                    for (int r = 0; r < rotaryCount[i]; r++)
                    {
                        int16_t rotary = 0;
                        pads->getPlayerRotaryInputs(rotary, p, r);
                        snprintf(text, 32, "Rotary %i: %i", r, rotary);
                        gpu->drawText(text, x + 10, y += 10, 500, 20, Video::Colors::White, Video::TALIGN_CENTER);
                    }
                }
            }
        }
    }
};

extern "C++" int genv_register_apps(Apps::IAppHost *host)
{
    if (!host)
        return -1;
    host->registerApplicationFactory(GenV_Demo::createApp, &GenV_Demo::infoStatic(), APP_SCREEN_TITLE);
    return 0;
}
