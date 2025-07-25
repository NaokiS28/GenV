/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sadmac.cpp - Created on 09-06-2025
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

#include "sadmac.hpp"

#include "common/util/rect.h"
#include "common/util/templates.hpp"
#include "common/services/services.hpp"

using namespace Apps;

SadMac::SadMac(ErrorScreenMessage *msg) : Application(Services::getVideo()),
                                          msg(msg)
{
    state = APP_STATE_INIT;
}

int SadMac::init()
{
    state = APP_STATE_RUN;
    playSound = true;
    errorSound = new WaveFile(sadMacSoundFile);
    if (errorSound->uploadSample() == Audio::SO_OKAY)
    {
        // close sound file?
    }

    sadCab = new Sprites::SpriteObject(sadCabImageFile);
    if (sadCab->uploadTexture() == 0)
    {
        // close image file?
    }
    
    reload();
    appReady = true;
    return 0;
}

void SadMac::update()
{
    if (playSound && playCount < maxCount && !errorSound->isPlaying())
    {
        playSound = false;
        playCount++;
        errorSound->play();
    }
}

void SadMac::reload()
{
    Coord midPoint = {
        gpu->getHorizontalRes() / 2,
        gpu->getVerticalRes() / 2};
    imgOrigin = Coord(
        midPoint.x - (sadCab->getTexture()->width / 2),
        midPoint.y - sadCab->getTexture()->height );
    titleOrigin = Coord(
        midPoint.x - 250,
        midPoint.y + 20);
    msgOrigin = Coord(
        midPoint.x - 250,
        midPoint.y + 50);
    
    sadCab->setPosition(imgOrigin);
}

void SadMac::render()
{
    if (playCount > 0)
    {
        gpu->fillScreen(Colors::Black);
        sadCab->draw();
        gpu->drawText(msg->title.str, msg->title.len, titleOrigin.x, titleOrigin.y, 500, 100, Colors::White, Video::TALIGN_CENTER);
        gpu->drawText(msg->message.str, msg->message.len, msgOrigin.x, msgOrigin.y, 500, 100, Colors::White, Video::TALIGN_CENTER);
    }
}

void SadMac::shutdown()
{
}

SadMac::~SadMac()
{
    state = APP_STATE_SHUTDOWN;
    if (msg && msg != &eMsgUnknownMsg)
        delete msg;
    if (errorSound != nullptr)
        delete errorSound;
}