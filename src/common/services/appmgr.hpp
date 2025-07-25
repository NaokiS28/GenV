/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * appmgr.hpp - Created on 26-04-2025
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
#include <stdbool.h>

#include "app/app.hpp"
#include "app/builtin/errorscr/errorscr.hpp"

namespace Apps
{
    enum AppSelect : uint8_t {
        APP_DEFAULT = 0,
        APP_FOREGROUND,
        APP_BACKGROUND
    };
    
    static constexpr const char *appmgrName = "Application Manager";
    
    class AppManager {
        private:
            Apps::Application *foregroundApp = nullptr;
            Apps::Application *backgroundApp = nullptr;
            Apps::AppLoader *loadingScreen = nullptr;
            ErrorScreen *errorScreen = nullptr;
    
            void quitApp(AppSelect app = APP_FOREGROUND);
            void swapApps();
            
        public:
            ~AppManager();
            AppManager();
            
            int init();
            int update();
            int loadApp();
            int render();
            bool reload();
            bool shutdown();

            bool showErrorScreen(const char *title, const char *text, ErrorMessageStyle style = EM_STYLE_DEFAULT, ErrorMessageIcon icon = EM_ICON_DEFAULT);
            bool showErrorScreen(ErrorScreenMessage *msg);
    
            Apps::Application *getForegroundApp(){ return foregroundApp; }
            Apps::Application *getBackgroundApp(){ return backgroundApp; }
    };
} // namespace Apps