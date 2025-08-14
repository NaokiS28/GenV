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

#include "app.hpp"
#include "iapp_host.hpp"
#include "common/services/system/arcade/iface_arcade.hpp"

namespace Apps
{
    enum AppSelect : uint8_t
    {
        APP_DEFAULT = 0,
        APP_FOREGROUND,
        APP_BACKGROUND
    };

    static constexpr const char *appmgrName = "Application Manager";
    static constexpr const int AppMgr_Max_App_Factories = 10;

    class AppManager : public IAppHost
    {
    private:
        typedef Application *(*AppFactory)();
        typedef LoadScreenApp *(*LoadScreenFactory)(Application *appToLoad);
        typedef ErrorScreenApp *(*ErrorScreenFactory)(ErrorScreenMessage *msg);

        enum AppTestmodeSetup : uint8_t
        {
            ASYS_GAME_MODE,
            ASYS_CLOSE_GAME_APPS,
            ASYS_LOAD_TEST_APP,
            ASYS_TEST_MODE,
            ASYS_LOAD_GAME_APP
        };

        struct AppFactoryList
        {
            struct AppFactoryEntry
            {
                const AppInfo *info;
                AppScreenType type = APP_SCREEN_GENERIC;
                AppFactory factory = nullptr;
                const char *name = nullptr;
            } factoryList[AppMgr_Max_App_Factories];

            uint8_t listCount = 0;
            const int listSize = AppMgr_Max_App_Factories;

            template <typename T>
            AppFactory operator[](T idx)
            {
                return factoryList[idx].factory;
            }

            // Adds an app factory constructor at a fixed position
            int addFactoryAt(uint8_t idx, AppFactory factory, const AppInfo *info, AppScreenType type);
            // Adds an app factory constructor at the next free position
            int addFactory(AppFactory factory, const AppInfo *info, AppScreenType type);
            // Removes an app factory constructor
            inline int removeFactory(AppScreenType type)
            {
                return removeFactory(getTypeID(type));
            }
            // Removes an app factory constructor
            int removeFactory(AppID id);
            // Removes all factory constructors
            void clearList();
            // Retrieves an app factory constructor using a hashed app ID
            inline AppFactory getFactory(AppScreenType type)
            {
                return getFactory(getTypeID(type));
            }
            AppFactory getFactory(AppID id);
            // Retrieves an app factory constructor using a generic app type value
            AppID getTypeID(AppScreenType type);
        } appFactories;

        bool firstRun = true;
        uint8_t enteredTestMode = 0; // arcade specific

        Application *foregroundApp = nullptr;
        Application *backgroundApp = nullptr;
        LoadScreenApp *loadingScreen = nullptr;
        ErrorScreenApp *errorScreen = nullptr;

        System::IArcadeSystem *asys = nullptr;

        ErrorScreenFactory errorScreenFactory = nullptr;
        const AppInfo *errorScreenInfo = nullptr;
        LoadScreenFactory loadScreenFactory = nullptr;
        const AppInfo *loadScreenInfo = nullptr;


        bool m_hasPending = false;
        AppID m_pendingId = 0;
        uint32_t m_pendingFlags = 0;

        void applyPending();
        Application *createApp(AppID id);
        void closeApp(Application *app);
        void quitApp(AppSelect app = APP_FOREGROUND);
        void swapApps();

        bool showErrorScreen(
            const char *title, const char *text,
            const uint32_t errorCode,
            ErrorMessageStyle style = EM_STYLE_DEFAULT,
            ErrorMessageIcon icon = EM_ICON_DEFAULT);
        bool showErrorScreen(ErrorScreenMessage *msg);

    public:
        AppManager();
        virtual ~AppManager();

        int init();
        bool shutdown();

        // IAppHost
        void requestSwitch(AppID id, uint32_t flags) override;
        void requestSwitch(AppScreenType type, uint32_t flags) override;
        void requestQuitForeground() override;
        void requestError(ErrorScreenMessage *msg) override;
        void requestError(const char *title, const char *text, uint32_t code, ErrorMessageStyle style, ErrorMessageIcon icon) override;

        // Forwarders to Applications
        int update();
        int loadApp();
        int render();
        bool reload();

        Application *getForegroundApp() { return foregroundApp; }
        Application *getBackgroundApp() { return backgroundApp; }

        // App factory registry
        void removeApplicationFactory(AppID id) override;
        void registerApplicationFactory(Application *(*factory)(), const AppInfo *info, AppScreenType type) override;
        void registerErrorScreenFactory(ErrorScreenApp *(*factory)(ErrorScreenMessage *msg), const AppInfo *info) override;
        void registerLoadingScreenFactory(LoadScreenApp *(*factory)(Application *appToLoad), const AppInfo *info) override;
    };

    AppManager *getAppManager();
} // namespace Apps