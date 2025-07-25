/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * g_title.hpp - Created on 12-06-2025
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
#include "app/app.hpp"
#include "gaunt_sub.hpp"

namespace Gauntlet
{
    using namespace Apps;

    class GauntletApp : public Application
    {
    private:
        const char *appName = "Gauntlet";
        const AppVersion appVer = AppVersion(0, 0, 1);

        GauntletSubApp *newScreen = nullptr;
        GauntletSubApp *currentApp = nullptr;
        GauntletSubApp *loadScreen = nullptr;

        static void staticAppChangeCallback(void *context, GauntletApps newApp)
        {
            // Cast back to the manager instance
            GauntletApp *self = static_cast<GauntletApp *>(context);
            self->changeApp(newApp);
        }

        void changeApp(GauntletApps newApp);
        void _swapApps();

    public:
        GauntletApp();

        int version() override { return appVer.toInt(); }; // Return app's version for logging
        const char *name() override { return appName; };   // Return app's name for logging

        int loadProgress(const char *&str); // State of initial loading progress in percent (str for custom loading text)

        int init();        // Initialize app
        void update();     // Update the app's logic
        void render();     // Request app to draw it's UI
        void loadApp();    // Call app to load next file/object (app handles any loading list)
        void shutdown() {} // Request app to begin unloading and finilization
        void reload();     // Request app to restart from the begining (or reload assets)
        
    };
}