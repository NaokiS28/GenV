/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gaunt_title.hpp - Created on 12-06-2025
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

#include "gaunt_title.hpp"
#include "common/services/services.hpp"
#include "common/formats/soundfile.hpp"

namespace Gauntlet
{

    const char *bgArtPath = "C:\\Users\\Naoki\\Nextcloud\\Programming\\GenV\\bin\\gauntlet\\bg.png";
    const char *logoArtPath = "C:\\Users\\Naoki\\Nextcloud\\Programming\\GenV\\bin\\gauntlet\\logo.png";
    const char *attractSoundPath = "C:\\Users\\Naoki\\Nextcloud\\Programming\\GenV\\bin\\gauntlet\\intro.wav";

    GauntletTitle::GauntletTitle()
    {
    }

    GauntletTitle::~GauntletTitle()
    {
    }

    int GauntletTitle::init()
    {
        area = {
            0, 0,
            gpu->getHorizontalRes(),
            gpu->getVerticalRes()};
        logoTween.setValue(area.h + 20);
        bgTween.setValue(0);
        textTween.setValue(0);
        gauntletLogo->setX(area.w / 2 - (util::percentOf(70, gauntletLogo->getTexture()->width / 2)));
        gauntletLogo->setY(area.h + 20);
        setAppState(Apps::APP_STATE_RUN);
        appReady = true;
        return 0;
    }

    void GauntletTitle::update()
    {
        switch (titleAnimState)
        {
        case GTITLE_START:
            logoTween.setValue(Services::frames(), 20, Services::msToFrames(2400));
            titleAnimState = GTITLE_LOGO_TWEEN;
            attractMusic->play();
            break;
        case GTITLE_LOGO_TWEEN:
            if (logoTween.isDone(Services::frames()))
            {
                bgTween.setValue(Services::frames(), -20, Services::msToFrames(150));
                titleAnimState = GTITLE_BG_TWEEN1;
            }
            else
            {
                gauntletLogo->setY(logoTween.getValue(Services::frames()));
            }
            break;
        case GTITLE_BG_TWEEN1:
            if (bgTween.isDone(Services::frames()))
            {
                bgTween.setValue(Services::frames(), 20, Services::msToFrames(150));
                titleAnimState = GTITLE_BG_TWEEN2;
            }
            break;
        case GTITLE_BG_TWEEN2:
            if (bgTween.isDone(Services::frames()))
            {
                titleAnimState = GTITLE_ANIM_DONE;
            }
            break;
        case GTITLE_LOGO_NUMERAL:
            break;
        case GTITLE_ANIM_DONE:
            if (textTween.isDone(Services::frames()))
            {
                if (textFadeIn)
                {
                    if (!attractMusic->isPlaying() && !frameTimer)
                    {
                        frameTimer = Services::frames() + Services::secondsToFrames(5);
                    }
                    else if (Services::frames() >= frameTimer)
                    {
                        // volatile size_t test = Services::frames();
                        titleAnimState = GTITLE_END;
                    }
                    else
                    {
                        textTween.setValue(Services::frames(), util::percentOf(30, UINT8_MAX), UINT8_MAX, Services::msToFrames(750));
                        textFadeIn = false;
                    }
                }
                else
                {
                    textTween.setValue(Services::frames(), UINT8_MAX, util::percentOf(30, UINT8_MAX), Services::msToFrames(750));
                    textFadeIn = true;
                }
            }
            break;
        default:
        case GTITLE_END:
            // tell gauntlet app to move.
            requestScreenChange(GauntletApps::G_Demo_Screen);
            break;
        }
    }

    void GauntletTitle::render()
    {
        gpu->fillScreen(Colors::Black);
        bgArt->draw(0, bgTween.getValue(Services::frames()), area.w, area.h);
        gauntletLogo->drawScaled(70);
        gpu->drawText(
            "INSERT COIN", 12,
            (area.w / 2) - 100,
            (area.h / 4) * 3,
            200,
            50,
            Colors::Alpha(Colors::Amber, textTween.getValue(Services::frames())),
            Video::TALIGN_CENTER);
    }

    void GauntletTitle::loadApp()
    {
        if (bgArt == nullptr)
        {
            bgArt = new Sprites::SpriteObject(bgArtPath);
            if (bgArt != nullptr)
            {
                bgArt->uploadTexture();
            }
        }
        else if (gauntletLogo == nullptr)
        {
            screenLoadProgrees = 33;
            gauntletLogo = new Sprites::SpriteObject(logoArtPath);
            if (gauntletLogo != nullptr)
            {
                gauntletLogo->uploadTexture();
            }
        }
        else if (attractMusic == nullptr)
        {
            screenLoadProgrees = 66;
            attractMusic = Audio::openAudioFile(attractSoundPath, 65);
            if (attractMusic != nullptr)
            {
                attractMusic->uploadSample();
            }
        }
        else
        {
            Application::state = APP_STATE_INIT;
            screenLoadProgrees = 100;
        }
    }

    void GauntletTitle::shutdown()
    {
        attractMusic->stop();
        delete gauntletLogo;
        delete bgArt;
        delete attractMusic;
        setAppState(APP_STATE_QUIT);
    }
}