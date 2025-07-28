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

#include "common/util/templates.hpp"
#include "common/services/services.hpp"

using namespace Apps;

DefaultErrorScreen::DefaultErrorScreen(ErrorScreenMessage *msg) : colorIntensity(UINT8_MAX)
{
    state = APP_STATE_INIT;
    this->msg = msg;
}

int DefaultErrorScreen::init()
{
    state = APP_STATE_RUN;
    area = RectWH(
        gpu->getHorizontalRes() / 10,
        gpu->getVerticalRes() / 4,
        (gpu->getHorizontalRes() / 10) * 8,
        gpu->getVerticalRes() / 2);

    bgAlpha.setValue(
        Services::frames(), 
        0, 
        util::percentOf(80, UINT8_MAX), 
        Services::msToFrames(bgFadeTime));
    jumpOut.setValue(0);
    jumpIn.setValue(
        Services::frames(), 
        Services::getVideo()->getVerticalRes(), 
        0, 
        Services::msToFrames(toastAnimTime));
    playSound = true;
    animState = INTRO_RUN;
    errorSound = new WaveFile(eMsgSoundFile);
    if (Services::getAudio()->uploadSample(errorSound) == Audio::SO_OKAY)
    {
        // close sound file?
    }
    appReady = true;
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
        if (jumpIn.isDone(Services::frames()))
        {
            animState = STOP;
        }
        break;
    case OUTRO_RUN:
        if (jumpOut.isDone(Services::frames()))
            animState = STOP;
        break;
    default:
        break;
    }

    // Border colour flash
    if (msg && msg->style != EM_STYLE_INFO)
    {
        if (colorIntensity.isDone(Services::frames()))
        {
            if (colorIntensity.getTargetValue() != UINT8_MAX)
            {
                colorIntensity.setValue(
                    Services::frames(),
                    UINT8_MAX,
                    Services::msToFrames(bgFadeTime));
                playSound = true;
            }
            else
            {
                colorIntensity.setValue(
                    Services::frames(),
                    150,
                    Services::msToFrames(bgFadeTime));
            }
        }
    }

    if (playSound && playCount < maxCount && !errorSound->isPlaying())
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
    Color c1; // Animates
    Color c2; // Not animates
    switch (msg->style)
    {
    case ErrorMessageStyle::EM_STYLE_INFO:
        c1 = Colors::LightBlue;
        break;
    case ErrorMessageStyle::EM_STYLE_WARNING:
        c1 = Colors::Amber;
        break;
    default:
    case ErrorMessageStyle::EM_STYLE_CRITICAL_ERROR:
    case ErrorMessageStyle::EM_STYLE_ERROR:
        c1 = Colors::Red;
        break;
    }

    c2 = c1;
    c1.a = colorIntensity.getValue(Services::frames());
    Video::premultiply(c1);

    // Set yOffset for jumpin or jump out animations
    int yOffset = area.y;
    switch (animState)
    {
    case INTRO_RUN:
        yOffset += jumpIn.getValue(Services::frames());
        break;
    case OUTRO_RUN:
        yOffset += jumpOut.getValue(Services::frames());
        break;
    default:
        break;
    }

    // Window
    gpu->drawRect(
        0, 0,                    // X/Y
        gpu->getHorizontalRes(), // Width
        gpu->getVerticalRes(),   // Height
        Colors::Alpha(Colors::Black, bgAlpha.getValue(Services::frames())));
    gpu->drawRect(area.x, yOffset, area.w, area.h, c1);
    gpu->drawRect(
        area.x + 5, yOffset + 5,
        area.w - 10, area.h - 10,
        Colors::Black);

    // Contents
    gpu->drawText(msg->title.str, msg->title.len, area.x + 20, yOffset + 20, area.w - 20, 50, Colors::White);
    gpu->drawLine(area.x + 20, yOffset + 60, (area.x + area.w) - 20, yOffset + 60, 2, c2);
    gpu->drawText(eMsgStrList[msg->style].str, eMsgStrList[msg->style].len, area.x + 20, yOffset + 70, area.w - 20, 100, c2);
    gpu->drawText(msg->message.str, msg->message.len, area.x + 20, yOffset + 95, area.w - 20, 100, Colors::White);

    // Options
    gpu->drawText(eMsgOptionList[msg->action].str, eMsgOptionList[msg->action].len, area.x + 20, (yOffset + area.h) - 60, area.w - 20, 100, Colors::White);
}

void DefaultErrorScreen::shutdown()
{
    state = APP_STATE_SHUTDOWN;
    if (msg && msg != &eMsgUnknownMsg)
        delete msg;
    if (errorSound != nullptr)
        delete errorSound;
}

DefaultErrorScreen::~DefaultErrorScreen()
{
    shutdown();
}