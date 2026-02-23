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
#include "common/services/io/player.hpp"
#include "common/services/io/vjoy.hpp"
#include "common/services/system/system.hpp"
#include "common/util/time.hpp"
#include <stdio.h>
#include <string.h>
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

    int currentPage = 0;
    int maxPage     = 0;
    int padCount    = 0;

    char pageStr[256] = {'\0'};

    tm time;

    Input::Player currentPlayer = Input::Player::PLAYER_1;

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
        IO::playerManager()->setMaximumPlayers(8);

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

        bool pageUpdate = false;

        // If player count changes (IO::playerManager() added/removed), update page
        maxPage                = IO::playerManager()->playerCount() - 1;
        static int lastMaxPage = 0;
        if (lastMaxPage != maxPage)
        {
            lastMaxPage = maxPage;
            pageUpdate |= true;
        }

        // If total device count changes (IO::playerManager() added/removed), update page
        padCount                = IO::playerManager()->inputDeviceCount();
        static int lastPadCount = 0;
        if (lastPadCount != padCount)
        {
            lastPadCount = padCount;
            pageUpdate |= true;
        }

        static uint32_t lastInputs = 0;
        uint32_t inputs            = IO::player(IO::Player::PLAYER_1).getDigital();
        // If the input states change, update the page
        if (lastInputs != inputs)
        {
            lastInputs = inputs;
            pageUpdate |= true;
        }

        if (pageUpdate)
        {
            // Player 1 right moves to next page
            if (inputs & static_cast<uint32_t>(VJoy_Input::D_Right))
            {
                if (currentPage < maxPage)
                    currentPage++;
                else
                    currentPage = 0;
            }

            // Player 1 left moves to previous page
            if (inputs & static_cast<uint32_t>(VJoy_Input::D_Left))
            {
                if (currentPage > 0)
                    currentPage--;
                else
                    currentPage = maxPage;
            }

            // If page count changes and the selected page is above or below, fix it
            if (maxPage && currentPage > maxPage) currentPage = maxPage;
            if (currentPage < 0) currentPage = 0;

            auto availablePlayers = IO::playerManager()->getPlayersAvailable();
            auto playerTest       = Input::Player::PLAYER_1;
            int found             = 0;
            bool matched          = false;
            do
            {
                if ((playerTest & availablePlayers) != Input::Player::NONE)
                {
                    if (found == currentPage)
                    {
                        matched = true;
                        break;
                    }
                    found++;
                }
                if (playerTest == Input::Player::ARCADE_CABINET) break;
                if (playerTest == Input::Player::PLAYER_8)
                    playerTest = Input::Player::ARCADE_CABINET;
                else
                    playerTest++;
            } while (true);
            currentPlayer = matched ? playerTest : Input::Player::INVALID;
        }
        System::getTime(time);

        if (currentPlayer == Input::Player::ARCADE_CABINET)
            getServicePanel();
        else
            getControllerData(currentPlayer);
    }

    void render() override
    {
        char textStr[32] = {'\0'};

        Time::getTimeString(time, textStr, 20, true, true);
        gpu->drawText(textStr, timeOrigin.x, timeOrigin.y, 200, 20, Video::Colors::White, Video::TALIGN_CENTER);

        gpu->fillScreen(Video::Colors::Black);
        gpu->drawText("GenV Input Test", txtOrigin.x, txtOrigin.y, 500, 20, Video::Colors::White, Video::TALIGN_CENTER);

        int x = 10;
        int y = 30;

        if (maxPage > 0)
        {
            snprintf(textStr, sizeof(textStr), "Page: %d / %d", currentPage + 1, maxPage + 1);
            gpu->drawText(textStr, txtOrigin.x + 10, y, 200, 20, Video::Colors::White, Video::TALIGN_CENTER);
            y += 20;

            if (currentPlayer != Input::Player::INVALID)
                gpu->drawText(pageStr, x, y, 500, 500, Video::Colors::White, Video::TALIGN_CENTER);
        }
        else
        {
            gpu->drawText("No registered inputs.", x, y, 500, 500, Video::Colors::White, Video::TALIGN_CENTER);
        }
    }

    void reload() override
    {
        txtOrigin  = Coord((gpu->getHorizontalRes() / 2) - 50, 5);
        timeOrigin = Coord(5, gpu->getVerticalRes() - 10);
    }
    void shutdown() override {}

    const AppInfo &info() const override
    {
        return appInfo;
    }

    void getControllerData(Input::Player player)
    {
        auto thisPlayer = IO::player(player);
        pageStr[0]      = {'\0'};

        char temp[32] = {0};

        snprintf(temp, 32, "Player %i:\r\n", static_cast<int>(Input::playerToIndex(player)) + 1);
        strncat(pageStr, temp, 128);
        snprintf(temp, sizeof(temp), "\tAssigned controllers:\r\n");
        strncat(pageStr, temp, strlen(temp));
        auto pDevList = thisPlayer.inputDevices();
        if (pDevList.count)
            for (size_t i = 0; i < pDevList.count; i++)
            {
                if (pDevList.devices[i] == nullptr) continue;
                snprintf(temp, sizeof(temp), "\t%d: %s\r\n", i, pDevList.devices[i]->name);
                strncat(pageStr, temp, strlen(temp));
            }
        else
        {
            snprintf(temp, sizeof(temp), "\tNone assigned.\r\n");
            strncat(pageStr, temp, strlen(temp));
        }

        snprintf(temp, sizeof(temp), "\r\n\tInputs:\r\n");
        strncat(pageStr, temp, strlen(temp));
        uint8_t digitalBanks = (thisPlayer.digitalCount() / 32) + 1;
        if (digitalBanks != 0)
        {
            for (uint8_t d = 0; d < digitalBanks; d++)
            {
                uint32_t inputs = thisPlayer.getDigital();
                snprintf(temp, 32, "\tDigital %i: 0x%08X\r\n", d, inputs);
                strncat(pageStr, temp, 128);
            }
        }

        if (auto aC = thisPlayer.analogCount(); aC != 0)
        {
            for (int a = 0; a < aC; a++)
            {
                int16_t analog = thisPlayer.getAnalog(a);
                snprintf(temp, 32, "\tAnalog %i: %+i\r\n", a, analog);
                strncat(pageStr, temp, 128);
            }
        }

        if (auto rC = thisPlayer.rotaryCount(); rC != 0)
        {
            for (int r = 0; r < rC; r++)
            {
                int16_t rotary = thisPlayer.getRotary(r);
                snprintf(temp, 32, "\tRotary %i: %i\r\n", r, rotary);
                strncat(pageStr, temp, 128);
            }
        }
    }

    void getServicePanel()
    {
        const char *serviceInputList[] = {
            "Test\t: %x\r\n",
            "Tilt\t: %x\r\n",
            "Coin %d:\t%x\r\n",
            "Service %d:\t%x\r\n"};

        enum : uint8_t
        {
            PANEL_TEST,
            PANEL_TILT,
            PANEL_COIN,
            PANEL_SERVICE,
            PANEL_MAX
        };

        pageStr[0] = {'\0'};

        char temp[32] = {0};

        snprintf(temp, 32, "Service Panel:\r\n");
        strncat(pageStr, temp, 128);
        snprintf(temp, sizeof(temp), "\tAssigned controllers:\r\n");
        strncat(pageStr, temp, strlen(temp));

        auto pDevList = IO::playerManager()->getPlayerInputDevices(Input::Player::ARCADE_CABINET);
        if (pDevList.count)
            for (size_t i = 0; i < pDevList.count; i++)
            {
                if (pDevList.devices[i] == nullptr) continue;
                snprintf(temp, sizeof(temp), "\t%d: %s\r\n", i, pDevList.devices[i]->name);
                strncat(pageStr, temp, strlen(temp));
            }
        else
        {
            snprintf(temp, sizeof(temp), "\tNone assigned.\r\n");
            strncat(pageStr, temp, strlen(temp));
        }

        snprintf(temp, sizeof(temp), "\r\n\tInputs:\r\n");
        strncat(pageStr, temp, strlen(temp));
        uint32_t inputs = IO::arcade().getDigital();
        int typeMax     = -1;
        for (uint8_t i = 0; i < PANEL_MAX; i++)
        {
            switch (i)
            {
            case PANEL_TILT:
            case PANEL_TEST:
                snprintf(
                    temp, 32,
                    serviceInputList[i],
                    i == PANEL_TEST ? inputs & asUint32(VJoy_Arcade::Test) : inputs & asUint32(VJoy_Arcade::Tilt));
                break;
            case PANEL_SERVICE:
                typeMax = 8; // Service_1 through Service_8
                for (int idx = 0; idx < typeMax; idx++)
                {
                    int n = idx + 1;
                    snprintf(
                        temp, 32,
                        serviceInputList[i],
                        n, inputs & asUint32(arcadeService(idx)));
                    strncat(pageStr, temp, strlen(temp));
                }
                temp[0] = '\0'; // Don't double-append via the outer strncat
                break;
            case PANEL_COIN:
                typeMax = 8; // Coin_1 through Coin_8
                for (int idx = 0; idx < typeMax; idx++)
                {
                    int n = idx + 1;
                    snprintf(
                        temp, 32,
                        serviceInputList[i],
                        n, inputs & asUint32(arcadeCoin(idx)));
                    strncat(pageStr, temp, strlen(temp));
                }
                temp[0] = '\0'; // Don't double-append via the outer strncat
                break;
            }
            strncat(pageStr, temp, 128);
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
