/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * iapp_host.hpp - Created on 13-08-2025
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

#include <stdint.h>
#include "app.hpp"
#include "common/services/services.hpp"
#include "common/util/hash.hpp"

namespace Apps
{
    using AppID = util::Hash;

    typedef Application *(*AppFactory)(IAppHost *host);
    typedef LoadScreenApp *(*LoadScreenFactory)(IAppHost *host, Application *appToLoad);
    typedef ErrorScreenApp *(*ErrorScreenFactory)(IAppHost *host, ErrorScreenMessage *msg);
    typedef ArcadeTestApp *(*ArcadeTestScreenFactory)(IAppHost *host);

    enum AppActionFlags : uint32_t
    {
        APPACT_NONE = 0,
        APPACT_REPLACE = 1u << 0,    // replace foreground
        APPACT_BACKGROUND = 1u << 1, // send current to background
        APPACT_CLEANBG = 1u << 2,    // close background
    };

    enum AppScreenType : uint8_t
    {
        APP_SCREEN_GENERIC,
        APP_SCREEN_TITLE,
        APP_SCREEN_GAMEPLAY,
        APP_SCREEN_ATTRACT,
        APP_SCREEN_TESTMODE,
    };

    class IAppHost
    {
    public:
        virtual void requestSwitch(AppID id, uint32_t flags) = 0;
        virtual void requestSwitch(AppScreenType type, uint32_t flags) = 0;
        virtual void requestQuitForeground() = 0;
        virtual void requestError(ErrorScreenMessage *msg) = 0;
        virtual void requestError(const char *title, const char *text, uint32_t code, ErrorMessageStyle style, ErrorMessageIcon icon) = 0;
        virtual void removeApplicationFactory(AppID id) = 0;
        virtual void registerApplicationFactory(AppFactory factory, const AppInfo *info, AppScreenType type = APP_SCREEN_GENERIC) = 0;
        virtual void registerErrorScreenFactory(ErrorScreenFactory factory, const AppInfo *info) = 0;
        virtual void registerLoadingScreenFactory(LoadScreenFactory factory, const AppInfo *info) = 0;
        virtual void registerGameTestModeFactory(ArcadeTestScreenFactory factory, const AppInfo *info) = 0;
        virtual void registerSystemTestModeFactory(AdminClass_Key key, ArcadeTestScreenFactory factory, const AppInfo *info) = 0;

    protected:
        ~IAppHost() {} // interface, non-owning
    };
} // namespace Apps
