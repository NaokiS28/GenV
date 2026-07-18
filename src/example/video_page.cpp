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

#include "video_page.hpp"

#include "common/return_codes.hpp"
#include "common/services/io/iface_input.hpp"
#include "common/services/io/player.hpp"
#include "common/services/io/vjoy.hpp"
#include "common/services/system/system.hpp"
#include "common/services/video/video.hpp"
#include "common/util/rect.hpp"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <genv.hpp>

using namespace Apps;

GenV_Video genv_demo_videoPage;

int GenV_Video::init()
{
    IO::playerManager()->setMaximumPlayers(8);
    reload();

    // Textures::TextureObject *textTest[256];
    // for (int i = 0; i < 256; i++)
    //{
    //    textTest[i] = Textures::createDefaultTexture();
    //    textTest[i]->uploadTexture();
    //}
    return 0;
}

constexpr const char genv_demo_videostr[] =
    {
        "Screen %d:\r\n"
        "\tName: %s\r\n"
        "\tWidth: %d\r\n"
        "\tHieght: %d\r\n"
        "\tRefresh: %dhz\r\n"};

int GenV_Video::update()
{
    int exitCode = GV_OK;

    bool pageUpdate = false;

    // If player count changes (IO::playerManager() added/removed), update page
    auto playerCount = IO::playerManager()->playerCount();
    maxPage          = (playerCount > 0 ? playerCount - 1 : 0);

    static int lastMaxPage = 0;
    if (lastMaxPage != maxPage)
    {
        lastMaxPage = maxPage;
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

    if (pageUpdate && playerCount != 0)
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

    if (exitCode == GV_OK)
        snprintf(pageStr, sizeof(pageStr), genv_demo_videostr,
                 screen->getNumber(),
                 screen->getName(),
                 screen->getHorizontalRes(),
                 screen->getVerticalRes(),
                 screen->getRefreshRate());

    return exitCode;
}

void GenV_Video::render()
{
    screen->drawText(pageStr, txtOrigin);
}

void GenV_Video::reload()
{
    auto vres = screen->getHorizontalRes();
    txtOrigin = Video::RectWH(10, 20, vres - 20, 200);
}
