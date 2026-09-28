/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * input_page.cpp - Created on 02-05-2026
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

#include "audio_page.hpp"

#include "common/return_codes.hpp"
#include "common/services/io/iface_input.hpp"
#include "common/services/io/player.hpp"
#include "common/services/io/vjoy.hpp"
#include "common/services/system/system.hpp"
#include "common/services/video/video.hpp"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <genv.hpp>

using namespace Apps;

GenV_Audio genv_demo_audioPage;

int GenV_Audio::init()
{
    IO::playerManager()->setMaximumPlayers(2);
    reload();
    return 0;
}

int GenV_Audio::update()
{
    int exitCode = GV_OK;

    bool pageUpdate = false;

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
    }

    return exitCode;
}

void GenV_Audio::render()
{
    // char textStr[32] = {'\0'};

    // int x = 10;
    // int y = 30;
}

void GenV_Audio::reload()
{
    auto vres = screen->getHorizontalRes();
    txtOrigin = Video::RectWH(10, 20, vres - 20, 200);
}
