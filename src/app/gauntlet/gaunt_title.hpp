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

#pragma once
#include "gaunt_sub.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/sound.hpp"
#include "common/util/tween.hpp"

namespace Gauntlet
{
    using namespace Apps;

    class GauntletTitle : public GauntletSubApp
    {
    private:
        RectWH area;
        Sprites::SpriteObject *bgArt = nullptr;
        Audio::SoundObject *attractMusic = nullptr;
        Sprites::SpriteObject *gauntletLogo = nullptr;

        Util::Tween<int, Util::LinearEasing> logoTween;
        Util::Tween<int, Util::QuadInOutEasing> bgTween;
        Util::Tween<int, Util::LinearEasing> textTween;

        bool textFadeIn = true;
        size_t frameTimer = 0;

        enum
        {
            GTITLE_START,
            GTITLE_LOGO_TWEEN,
            GTITLE_BG_TWEEN1,
            GTITLE_LOGO_NUMERAL,
            GTITLE_BG_TWEEN2,
            GTITLE_ANIM_DONE,
            GTITLE_END
        } titleAnimState = GTITLE_START;

        const char *appName = "GauntletTitle";
        const AppVersion appVer = AppVersion(0, 0, 1);

        unsigned int screenLoadProgrees = 0;

    public:
        GauntletTitle();
        ~GauntletTitle();

        int version() override { return appVer.toInt(); }; // Return app's version for logging
        const char *name() override { return appName; };   // Return app's name for logging

        int loadProgress(const char *&str) override { return screenLoadProgrees; }

        int init();        // Initialize app
        void update();     // Update the app's logic
        void render();     // Request app to draw it's UI
        void loadApp();    // Call app to load next file/object (app handles any loading list)
        void shutdown();   // Request app to begin unloading and finilization
        void reload() {}   // Request app to restart from the begining (or reload assets)
    };
}