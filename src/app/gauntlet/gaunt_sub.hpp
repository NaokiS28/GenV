/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gauntlet_sub.hpp - Created on 12-06-2025
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

namespace Gauntlet
{
    using namespace Apps;

    enum class GauntletApps
    {
        G_Title_Screen,
        G_Demo_Screen,
        G_Game_Screen
    };

    typedef void (*AppChangeCallback)(void *context, GauntletApps newApp);

    class GauntletSubApp : public Application
    {
    public:
        void setAppChangeCallback(Gauntlet::AppChangeCallback callback, void *ctx)
        {
            appChangeCallback = callback;
            callbackContext = ctx;
        }

    protected:
        void requestScreenChange(GauntletApps app)
        {
            if (appChangeCallback != nullptr)
            {
                appChangeCallback(callbackContext, app);
            }
        }

    private:
        Gauntlet::AppChangeCallback appChangeCallback = nullptr;
        void *callbackContext = nullptr;
    };

}