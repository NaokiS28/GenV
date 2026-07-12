/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * genv_demo.cpp - Created on 02-05-2026
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

#include "genv_demo.hpp"
#include "app/app.hpp"
#include "common/logger/log.hpp"
#include "common/return_codes.hpp"
#include "common/services/io/vjoy.hpp"
#include "common/services/system/system.hpp"
#include "common/util/time.hpp"
#include "main_menu_list.hpp"
#include <stdio.h>

int GenV_Demo::init()
{
    for (auto page : genv_demoPageList)
    {
        if (int error = page->init(); error != GV_OK)
            LOG("GenV Demo", "%s page encountered init error %i", page->info().name, error);
        page->gpu = gpu;
    }
    reload();
    setAppState(Apps::APP_STATE_RUN);
    return GV_OK;
}

void GenV_Demo::update()
{
    System::getTime(time);

    if (currentPage != nullptr)
    {
        if (currentPage->update() != GV_OK)
            currentPage = nullptr;
    }
    else
    {
        uint32_t inputs = IO::player(IO::Player::PLAYER_1).getDigital();
        // Player 1 down moves pointer down
        if (inputs & static_cast<uint32_t>(VJoy_Input::D_Down))
        {
            if (currentMenuPos < genv_demoPageListSize - 1)
                currentMenuPos++;
            else
                currentMenuPos = 0;
        }

        // Player 1 up moves pointer up
        if (inputs & static_cast<uint32_t>(VJoy_Input::D_Up))
        {
            if (currentMenuPos > 0)
                currentMenuPos--;
            else
                currentMenuPos = genv_demoPageListSize - 1;
        }

        // Player 1 B1/Start selects page
        if (!selectIgnore && inputs & static_cast<uint32_t>(VJoy_Input::Button_1 | VJoy_Input::Start))
        {
            currentPage = genv_demoPageList[currentMenuPos];
            currentPage->reload();
            selectIgnore = true;
        }
        else if (selectIgnore && !(inputs & static_cast<uint32_t>(VJoy_Input::Button_1 | VJoy_Input::Start)))
        {
            selectIgnore = false;
        }
    }
}

void GenV_Demo::render()
{
    gpu->fillScreen(Video::Colors::Black);
    if (currentPage != nullptr)
    {
        currentPage->render();

        // Draw page title
        gpu->drawText(currentPage->info().name, titleBox, Video::Colors::White, Video::TALIGN_CENTER);
    }
    else
    {
        gpu->drawText(info().name, titleBox, Video::Colors::White, Video::TALIGN_CENTER);

        char sys[128] = {0};
        snprintf(sys, sizeof(sys), "System: %s %s", getSystem()->getSysInfo()->make, getSystem()->getSysInfo()->name);
        gpu->drawText(sys, sysBox);

        int y       = 0;
        uint8_t idx = 0;
        for (auto page : genv_demoPageList)
        {
            if (idx == currentMenuPos)
                gpu->drawChar('>', menuBox.x - 20, menuBox.y + y, Video::Colors::White);

            gpu->drawText(page->info().name, menuBox, Video::Colors::White);
            y += 10;
            idx++;
        }
    }

    // Draw clock in bottom right
    char timeStr[32] = {0};
    Time::getTimeString(time, timeStr, sizeof(timeStr), true, true);
    gpu->drawText(timeStr, timeBox, Video::Colors::White, Video::TALIGN_RIGHT);
}

void GenV_Demo::reload()
{
    if (currentPage != nullptr) currentPage->reload();
    titleBox = {
        gpu->getHorizontalRes() / 2,
        10,
        128, 10};
    timeBox = {
        gpu->getHorizontalRes() - 10,
        gpu->getVerticalRes() - 10,
        128, 10};
    menuBox = {
        30, 20, 200, 200};
    sysBox = {
        10,
        gpu->getVerticalRes() - 10,
        128, 10};
}

extern "C++" int genv_register_apps(Apps::IAppHost *host)
{
    if (!host)
        return -1;
    host->registerApplicationFactory(GenV_Demo::createApp, &GenV_Demo::infoStatic(), APP_SCREEN_TITLE);
    return 0;
}
