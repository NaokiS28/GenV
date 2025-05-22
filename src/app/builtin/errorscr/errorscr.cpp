/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * loader.cpp - Created on 26-04-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#include "errorscr.hpp"
#include <stdexcept>

#include "common/util/templates.hpp"
#include "common/services/services.hpp"

using namespace Apps;

ErrorScreen::ErrorScreen(ErrorScreenMessage *msg) : Application(Services::getVideo()),
                                                    msg(msg)
{
    state = APP_STATE_INIT;
}

ErrorScreen::ErrorScreen(
    const char *title,
    const char *text,
    int style,
    int icon) : Application(Services::getVideo())
{
    msg = new ErrorScreenMessage(title, text, style, icon);
    if (!msg)
        msg = &eMsgUnknownMsg;

    state = APP_STATE_INIT;
}

int ErrorScreen::init()
{
    state = APP_STATE_RUN;
    area = RectWH(
        gpu->getHorizontalRes() / 10,
        gpu->getVerticalRes() / 4,
        (gpu->getHorizontalRes() / 10) * 8,
        gpu->getVerticalRes() / 2);
    appReady = true;
    return 0;
}

void ErrorScreen::update()
{
}

void ErrorScreen::reload(){
    area = RectWH(
        gpu->getHorizontalRes() / 10,
        gpu->getVerticalRes() / 4,
        (gpu->getHorizontalRes() / 10) * 8,
        gpu->getVerticalRes() / 2);
}

void ErrorScreen::render()
{
    Color c;
    switch (msg->style)
    {
    case EM_STYLE_INFO:
        c.r = 0;
        c.b = 255;
        c.g = 100;
        break;
    case EM_STYLE_WARNING:
        c.r = 255;
        c.b = 191;
        c.g = 0;
        break;
    default:
    case EM_STYLE_CRITICAL_ERROR:
    case EM_STYLE_ERROR:
        c.r = 255;
        c.b = 0;
        c.g = 0;
        break;
    }

    gpu->drawRect(0, 0, gpu->getHorizontalRes(), gpu->getVerticalRes(), {0, 0, 0, util::percentOf(80, 255)});
    gpu->drawRect(area.x, area.y, area.w, area.h, c);
    gpu->drawRect(area.x + 5, area.y + 5, area.w - 10, area.h - 10, Color(0, 0, 0));
    gpu->drawText(msg->title.str, msg->title.len, area.x + 20, area.y + 20, area.w - 20, 50, {255, 255, 255});

    gpu->drawLine(area.x + 20, area.y + 60, (area.x + area.w) - 20, area.y + 60, 2, {255, 0, 0});
    gpu->drawText(eMsgStrList[msg->style].str, eMsgStrList[msg->style].len, area.x + 20, area.y + 70, area.w - 20, 100, c);
    gpu->drawText(msg->message.str, msg->message.len, area.x + 20, area.y + 95, area.w - 20, 100, {255, 255, 255});

    gpu->drawText(eMsgOptionList[msg->action].str, eMsgOptionList[msg->action].len, area.x + 20, (area.y + area.h) - 60, area.w - 20, 100, {255, 255, 255});
}

ErrorScreen::~ErrorScreen()
{
    if (msg && msg != &eMsgUnknownMsg)
        delete msg;
}