/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * loader.cpp - Created on 26-04-2025
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

#include "errorscr.hpp"

#include "app/app.hpp"
#include "common/util/templates.hpp"
#include "common/services/services.hpp"
#include "common/services/system/arcade/arcade.hpp"
#include "common/services/system/arcade/errorcodes.hpp"

using namespace Apps;

DefaultErrorScreen::DefaultErrorScreen(IAppHost *host, ErrorScreenMessage *msg)
    : ErrorScreenApp(host),
      colorIntensity(UINT8_MAX)

{
    setAppState(APP_STATE_INIT);
    this->msg = msg;
}

int DefaultErrorScreen::init()
{
    setAppState(APP_STATE_RUN);
    area = RectWH(
        gpu->getHorizontalRes() / 10,
        gpu->getVerticalRes() / 4,
        (gpu->getHorizontalRes() / 10) * 8,
        gpu->getVerticalRes() / 2);

    bgAlpha.setValue(
        Video::frames(),
        0,
        util::percentOf(80, UINT8_MAX),
        Video::msToFrames(bgFadeTime));
    jumpOut.setValue(0);
    jumpIn.setValue(
        Video::frames(),
        getServiceManager()->getVideo()->getVerticalRes(),
        0,
        Video::msToFrames(toastAnimTime));
    playSound = true;
    animState = INTRO_RUN;
    // errorSound = new WaveFile(eMsgSoundFile);
    // if (getServiceManager()->getAudio()->uploadSample(errorSound) == GV_OK)
    //{
    // close sound file?
    //}

    ArcadeFunc(
        if (msg->style == EM_STYLE_CRITICAL_ERROR)
            Genv_Arcade->getNVRAM()
                .addErrorCode(GENV_ARC_ERR_CRITICAL_APP_ERROR, 0, msg->errorCode););

    return 0;
}

void DefaultErrorScreen::update()
{
    // Animations
    switch (animState)
    {
    case INTRO_INIT:
        break;
    case INTRO_RUN:
        if (jumpIn.isDone(Video::frames()))
        {
            animState = STOP;
        }
        break;
    case OUTRO_RUN:
        if (jumpOut.isDone(Video::frames()))
            animState = STOP;
        break;
    default:
        break;
    }

    // Border colour flash
    if (msg && msg->style != EM_STYLE_INFO)
    {
        if (colorIntensity.isDone(Video::frames()))
        {
            if (colorIntensity.getTargetValue() != UINT8_MAX)
            {
                colorIntensity.setValue(
                    Video::frames(),
                    UINT8_MAX,
                    Video::msToFrames(bgFadeTime));
                playSound = true;
            }
            else
            {
                colorIntensity.setValue(
                    Video::frames(),
                    150,
                    Video::msToFrames(bgFadeTime));
            }
        }
    }

    if (playSound && playCount < maxCount && errorSound && !errorSound->isPlaying())
    {
        playSound = false;
        playCount++;
        errorSound->play();
    }
}

void DefaultErrorScreen::reload()
{
    area = RectWH(
        gpu->getHorizontalRes() / 10,
        gpu->getVerticalRes() / 4,
        (gpu->getHorizontalRes() / 10) * 8,
        gpu->getVerticalRes() / 2);
}

void DefaultErrorScreen::render()
{
    Video::Color c1; // Animates
    Video::Color c2; // Not animates
    switch (msg->style)
    {
    case ErrorMessageStyle::EM_STYLE_INFO:
        c1 = Video::Colors::LightBlue;
        break;
    case ErrorMessageStyle::EM_STYLE_WARNING:
        c1 = Video::Colors::Amber;
        break;
    default:
    case ErrorMessageStyle::EM_STYLE_CRITICAL_ERROR:
    case ErrorMessageStyle::EM_STYLE_ERROR:
        c1 = Video::Colors::Red;
        break;
    }

    c2   = c1;
    c1.a = colorIntensity.getValue(Video::frames());
    Video::premultiply(c1);

    // Set yOffset for jumpin or jump out animations
    int yOffset = area.y;
    switch (animState)
    {
    case INTRO_RUN:
        yOffset += jumpIn.getValue(Video::frames());
        break;
    case OUTRO_RUN:
        yOffset += jumpOut.getValue(Video::frames());
        break;
    default:
        break;
    }

    // Window
    gpu->fillScreen(
        Video::Colors::Alpha(Video::Colors::Black, bgAlpha.getValue(Video::frames())));
    gpu->drawRect(area.x, yOffset, area.w, area.h, c1);
    gpu->drawRect(
        area.x + 5, yOffset + 5,
        area.w - 10, area.h - 10,
        Video::Colors::Black);

    // Contents
    //	Title
    gpu->drawText(msg->title.str, area.x + 20, yOffset + 20, area.w - 20, 50, Video::Colors::White);
    gpu->drawLine(area.x + 20, yOffset + 35, (area.x + area.w) - 20, yOffset + 35, 2, c2);
    //	Severity
    gpu->drawText(eMsgStrList[msg->style].str, area.x + 20, yOffset + 40, area.w - 20, 100, c2);
    //	Cause
    gpu->drawText(msg->message.str, area.x + 20, yOffset + 60, area.w - 20, 100, Video::Colors::White);

    // 	Button Options
    if (msg->style != EM_STYLE_CRITICAL_ERROR) gpu->drawText(eMsgOptionList[msg->action].str, area.x + 20, (yOffset + area.h) - 40, area.w - 20, 100, Video::Colors::White);
}

void DefaultErrorScreen::shutdown()
{
    setAppState(APP_STATE_SHUTDOWN);
    if (msg && msg != &eMsgUnknownMsg)
        delete msg;
    if (errorSound != nullptr)
        delete errorSound;
}

DefaultErrorScreen::~DefaultErrorScreen()
{
    shutdown();
}