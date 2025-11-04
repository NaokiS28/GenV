/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * appmgr.cpp - Created on 26-04-2025
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

#include <string.h>

#include "app.hpp"
#include "appmgr.hpp"
#include "common/services/services.hpp"
#include "common/logger/log.hpp"

#include "builtin/gvss/gvss.hpp"
#include "builtin/errorscr/errorscr.hpp"

#define APPMGR_LOG(fmt, ...) LOG("appmgr", fmt __VA_OPT__(, ) __VA_ARGS__)

extern "C++" int genv_register_apps(IAppHost *host);

// TODO: Rewrite app life cycle management
// TODO: Better defined API for app management (app should request to close itself and report when it can be quit)
// TODO: Critical errors should halt the app execution flow (or rather only allow test mode).
// TODO: Pause the running apps (and sounds). Apps should still render the last frame when the error occurs.

namespace Apps
{
    constexpr const char *s_factoryListfull = "Factory list is full.";
    constexpr const char *s_factoryNotExists = "Factory does not exist in factory list.";
    constexpr const char *s_factoryAlreadyExists = "Factory is already in factory list.";
    constexpr const char *s_factoryPtrNull = "Factory pointer was null.";
    constexpr const char *s_infoPtrNull = "App info struct pointer was null.";
    constexpr const char *s_unknownError = "Unknown error occured.";

    // --- Local helpers ---------------------------------------------------------

    Application *AppManager::getApp(AppSelect type)
    {
        Application *app = nullptr;
        switch (type)
        {
        case APP_FOREGROUND: app = foregroundApp; break;
        case APP_BACKGROUND: app = backgroundApp; break;
        case APP_LOADSCREEN: app = loadingScreen; break;
        case APP_ERRORSCREEN: app = errorScreen; break;
        default: break;
        }
        return app;
    }

    void AppManager::deleteApp(AppSelect type)
    {
        switch (type)
        {
        case APP_FOREGROUND:
            delete foregroundApp;
            foregroundApp = nullptr;
            break;
        case APP_BACKGROUND:
            delete backgroundApp;
            backgroundApp = nullptr;
            break;
        case APP_LOADSCREEN:
            delete loadingScreen;
            loadingScreen = nullptr;
            break;
        case APP_ERRORSCREEN:
            delete errorScreen;
            errorScreen = nullptr;
            break;
        default: break;
        }
    }

    void AppManager::tickApp(AppSelect type)
    {
        Application *app = getApp(type);
        if (!app)
            return;

        switch (app->getState())
        {
        case APP_STATE_RUN:
            app->update();
            break;

        case APP_STATE_REGAINFOCUS:
            app->update();
            app->setAppState(APP_STATE_RUN);
            break;

        case APP_STATE_INIT:
        {
            const char *str = app->name();
            APPMGR_LOG("Init application: %s", str);
            app->init(this);
            break;
        }

        case APP_STATE_LOAD:
            app->loadApp();
            break;

        case APP_STATE_SHUTDOWN:
            app->update();
            break;

        case APP_STATE_ERROR:
            break;

        case APP_STATE_QUIT:
            if (type != APP_DEFAULT)
                deleteApp(type);
            break;

        default:
            break;
        }
    }

    static inline void reloadIf(Application *a)
    {
        if (a)
            a->reload();
    }

    // --- AppManager ------------------------------------------------------------

    AppManager::AppManager()
    {
        appFactories.clearList();
    }

    AppManager::~AppManager()
    {
        shutdown();
    }

    bool AppManager::shutdown()
    {
        closeApp(APP_FOREGROUND);
        closeApp(APP_BACKGROUND);
        closeApp(APP_LOADSCREEN);
        closeApp(APP_ERRORSCREEN);
        return true;
    }

    bool AppManager::reload()
    {
        reloadIf(foregroundApp);
        reloadIf(backgroundApp);
        reloadIf(loadingScreen);
        reloadIf(errorScreen);
        return true;
    }

    int AppManager::init()
    {
        APPMGR_LOG("Starting application manager...");
        if (!asys && firstRun)
        {
            asys = System::GetArcadeInterface();
            if (asys)
                APPMGR_LOG("System is in arcade mode.");
        }

        appFactories.clearList();
        int r = genv_register_apps(this);
        if (r != 0)
        {
            showErrorScreen("APP REGISTRATION FAILURE",
                            "REGISTRATION FUNCTION RETURNED FAILED",
                            GENV_APP_ERR_INVALID_GAME,
                            EM_STYLE_CRITICAL_ERROR,
                            EM_ICON_CRITICAL_ERROR);
            return -1;
        }

        AppFactory factory = appFactories.getFactory(APP_SCREEN_TITLE);
        if (!factory)
            factory = appFactories[0];

        foregroundApp = factory();
        if (!foregroundApp)
        {
            showErrorScreen("APP INIT FAILURE",
                            "INVALID ENTRYPOINT",
                            GENV_APP_ERR_INVALID_GAME,
                            EM_STYLE_CRITICAL_ERROR,
                            EM_ICON_CRITICAL_ERROR);
            return -1;
        }

        if (loadScreenFactory)
        {
            loadingScreen = loadScreenFactory(foregroundApp);
        }
        if (!loadingScreen)
        {
            showErrorScreen("LOADER INIT FAILURE",
                            "INVALID LOADER",
                            GENV_APP_ERR_INVALID_LOADER,
                            EM_STYLE_CRITICAL_ERROR,
                            EM_ICON_CRITICAL_ERROR);
            return -1;
        }

        enteredTestMode = ASYS_GAME_MODE;
        firstRun = false;

        foregroundApp = nullptr;

        return 0;
    }

    int AppManager::update()
    {
        // Apply any app-switch requests queued by apps in a previous frame.
        if (m_hasPending)
            applyPending();

        // --- Arcade/test-mode transitions -------------------------------------
        if (asys)
        {
            if (asys->runTestMode() && enteredTestMode < ASYS_LOAD_TEST_APP)
            {
                ArcadeTestApp *fgApp = getArcadeTestApp(foregroundApp);
                ArcadeTestApp *bgApp = getArcadeTestApp(backgroundApp);
                if (fgApp == nullptr && bgApp == nullptr)
                {
                    // Will loop and close apps
                    quitApp(APP_FOREGROUND);
                    enteredTestMode = ASYS_CLOSE_GAME_APPS;
                }
            }
            else if (!asys->runTestMode() && (enteredTestMode != ASYS_GAME_MODE && enteredTestMode != ASYS_LOAD_GAME_APP))
            {
                quitApp(APP_FOREGROUND);
                enteredTestMode = ASYS_LOAD_GAME_APP;
            }
        }

        // --- Normal app graph updates -----------------------------------------
        if (!foregroundApp && !backgroundApp && !errorScreen && !(asys && enteredTestMode == ASYS_GAME_MODE))
        {
            showErrorScreen("APPLICATION MANAGER",
                            "BOTH APP POINTERS ARE NULL",
                            GENV_APP_ERR_NO_APPS,
                            EM_STYLE_CRITICAL_ERROR,
                            EM_ICON_CRITICAL_ERROR);
        }
        else
        {
            tickApp(APP_FOREGROUND);
            tickApp(APP_BACKGROUND);
        }

        tickApp(APP_ERRORSCREEN);
        tickApp(APP_LOADSCREEN);

        // Late arcade test-mode transitions after graph changes
        if (asys && !foregroundApp && !backgroundApp)
        {
            if (enteredTestMode == ASYS_CLOSE_GAME_APPS)
            {
                // foregroundApp = new GenVTestApp;
                if (foregroundApp != nullptr)
                {
                    enteredTestMode = ASYS_LOAD_TEST_APP;
                }
            }
            else if (enteredTestMode == ASYS_TEST_MODE)
            {
                init();
                enteredTestMode = 0;
            }
        }

        return 0;
    }

    int AppManager::render()
    {
        if (backgroundApp && backgroundApp->isReady())
            backgroundApp->render(); // bottom layer
        if (foregroundApp && foregroundApp->isReady())
            foregroundApp->render(); // 3rd layer
        if (loadingScreen && loadingScreen->isReady())
            loadingScreen->render(); // 2nd layer
        if (errorScreen && errorScreen->isReady())
            errorScreen->render(); // top layer
        return 0;
    }

    Application *AppManager::createApp(AppID id)
    {
        AppFactory factory = appFactories.getFactory(id);
        if (!factory)
        {
            APPMGR_LOG("Failed to create app for ID %X: %s", id, s_factoryNotExists);
            return nullptr;
        }
        return factory();
    }

    void AppManager::closeApp(AppSelect type)
    {
    }

    void AppManager::swapApps()
    {
        // Swap foreground <-> background (preserving focus state)
        APPMGR_LOG("Swapping applications from %s to %s", foregroundApp->name(), backgroundApp->name());
        Application *fApp = nullptr;

        if (foregroundApp)
        {
            foregroundApp->setAppState(APP_STATE_OUTOFFOCUS);
            fApp = foregroundApp;
        }
        if (backgroundApp)
        {
            backgroundApp->setAppState(APP_STATE_REGAINFOCUS);
            foregroundApp = backgroundApp;
        }
        backgroundApp = fApp;
    }

    void AppManager::quitApp(AppSelect app)
    {
        APPMGR_LOG("Closing app %s", foregroundApp->name());
        switch (app)
        {
        case APP_FOREGROUND:
            if (foregroundApp)
                foregroundApp->shutdown();
            if (backgroundApp)
                swapApps();
            break;

        case APP_BACKGROUND:
            if (backgroundApp)
                backgroundApp->shutdown();
            break;

        default:
            break;
        }
    }

    int AppManager::loadApp()
    {
        // Reserved for future use (explicit manager-side loading).
        return 0;
    }

    bool AppManager::showErrorScreen(
        const char *title,
        const char *text,
        const uint32_t errorCode,
        ErrorMessageStyle style,
        ErrorMessageIcon icon)
    {
        ErrorScreenMessage *esApp = new ErrorScreenMessage(title, text, errorCode, style, icon);
        if (!esApp)
            return false;
        return showErrorScreen(esApp);
    }

    bool AppManager::showErrorScreen(ErrorScreenMessage *msg)
    {
        if (!msg)
            return false;

        if (errorScreen)
        {
            const ErrorMessageStyle currSeverity = errorScreen->getSeverity();
            if (msg->style >= currSeverity)
            {
                errorScreen->shutdown();
                delete errorScreen;
                errorScreen = nullptr;
            }
        }
        APPMGR_LOG("Error screen shown: (%s): %s", msg->title, msg->message);
        errorScreen = errorScreenFactory ? errorScreenFactory(msg) : nullptr;
        return (errorScreen != nullptr);
    }

    void AppManager::registerApplicationFactory(Application *(*factory)(), const AppInfo *info, AppScreenType type)
    {
        const int code = appFactories.addFactory(factory, info, type);
        if (code != 0)
        {
            const char *str = nullptr;
            switch (code)
            {
            case 1:
                str = s_factoryPtrNull;
                break;
            case 2:
                str = s_factoryListfull;
                break;
            case 3:
                str = s_factoryAlreadyExists;
                break;
            default:
                str = s_unknownError;
                break;
            }
            APPMGR_LOG("Register app factory \"%s\" (%X) failed: %s (code %d)", info->name, info->id, str, code);
        }
    }

    void AppManager::removeApplicationFactory(AppID id)
    {
        const int code = appFactories.removeFactory(id);
        if (code != 0)
        {
            const char *str = (code == 1) ? s_factoryNotExists : s_unknownError;
            APPMGR_LOG("Removing app factory (%X) failed: %s (code %d)", id, str, code);
        }
    }

    void AppManager::registerLoadingScreenFactory(LoadScreenApp *(*factory)(Application *appToLoad), const AppInfo *info)
    {
        const char *str = nullptr;
        if (!factory)
            str = s_factoryPtrNull;
        if (!info)
            str = s_infoPtrNull;
        if (str)
        {
            APPMGR_LOG("Register loading screen factory failed: %s", str);
            return;
        }
        APPMGR_LOG("Registered new loading screen \"%s\" (%X) factory (0x%X)", info->name, info->id, factory);
        loadScreenFactory = factory;
        loadScreenInfo = info;
    }

    void AppManager::registerErrorScreenFactory(ErrorScreenApp *(*factory)(ErrorScreenMessage *msg), const AppInfo *info)
    {
        const char *str = nullptr;
        if (!factory)
            str = s_factoryPtrNull;
        if (!info)
            str = s_infoPtrNull;
        if (str)
        {
            APPMGR_LOG("Register error screen factory failed: %s", str);
            return;
        }
        APPMGR_LOG("Registered new error screen \"%s\" (%X) factory (0x%X)", info->name, info->id, factory);
        errorScreenFactory = factory;
        errorScreenInfo = info;
    }

    void AppManager::requestSwitch(AppScreenType type, uint32_t flags)
    {
        // Defer graph mutation to the end of the frame.
        if (type != APP_SCREEN_GENERIC)
        {
            m_pendingId = appFactories.getTypeID(type);
            if (!m_pendingId)
                return;
            m_hasPending = true;
            m_pendingFlags = flags;
        }
    }

    void AppManager::requestSwitch(AppID id, uint32_t flags)
    {
        // Defer graph mutation to the end of the frame.
        m_hasPending = true;
        m_pendingId = id;
        m_pendingFlags = flags;
    }

    void AppManager::requestQuitForeground()
    {
        m_hasPending = true;
        m_pendingId = 0;                 // no new app
        m_pendingFlags = APPACT_REPLACE; // close foreground
    }

    void AppManager::requestError(ErrorScreenMessage *msg)
    {
        if (msg)
            showErrorScreen(msg);
    }

    void AppManager::requestError(const char *title,
                                  const char *text,
                                  uint32_t code,
                                  ErrorMessageStyle style,
                                  ErrorMessageIcon icon)
    {
        showErrorScreen(title, text, code, style, icon);
    }

    void AppManager::applyPending()
    {
        // Semantics:
        //  - APPACT_REPLACE:   close foreground, create new as foreground.
        //  - APPACT_BACKGROUND:move current foreground to background.
        //  - APPACT_CLEANBG:   close existing background.

        if (m_pendingFlags & APPACT_CLEANBG)
        {
            closeApp(APP_BACKGROUND);
        }

        if (m_pendingFlags & APPACT_REPLACE)
        {
            closeApp(APP_FOREGROUND);
        }
        else if (m_pendingFlags & APPACT_BACKGROUND)
        {
            if (backgroundApp)
                closeApp(APP_BACKGROUND);
            if (foregroundApp)
            {
                foregroundApp->setAppState(APP_STATE_OUTOFFOCUS);
                backgroundApp = foregroundApp;
                foregroundApp = nullptr;
            }
        }

        if (m_pendingId != 0)
        {
            Application *next = createApp(m_pendingId);
            if (next)
            {
                foregroundApp = next;
                foregroundApp->setAppState(APP_STATE_INIT);
                foregroundApp->init(this);
            }
            else
            {
                requestError("APP INIT",
                             "FAILED TO CREATE APP",
                             0,
                             EM_STYLE_CRITICAL_ERROR,
                             EM_ICON_CRITICAL_ERROR);
            }
        }

        m_hasPending = false;
        m_pendingId = 0;
        m_pendingFlags = 0;
    }

    // --- AppFactoryList --------------------------------------------------------

    int AppManager::AppFactoryList::addFactory(AppFactory factory, const AppInfo *info, AppScreenType type)
    {
        if (!factory || !info)
            return 1;
        if (listCount >= listSize)
            return 2; // list full
        if (getFactory(info->id))
            return 3; // already exists

        for (int idx = 0; idx < listSize; ++idx)
        {
            if (factoryList[idx].info == nullptr)
            {
                factoryList[idx].info = info;
                factoryList[idx].factory = factory;
                factoryList[idx].type = type;
                listCount++;
                APPMGR_LOG("Added \"%s\" (%X) factory at 0x%X to app factory list at position %u.", info->name, info->id, factoryList[idx].factory, idx);
                return 0;
            }
        }
        return -1;
    }

    int AppManager::AppFactoryList::addFactoryAt(uint8_t idx, AppFactory factory, const AppInfo *info, AppScreenType type)
    {
        if (!factory || !info)
            return 1;
        if (listCount >= listSize)
            return 2;
        if (getFactory(info->id))
            return 3;
        if (idx > listCount || idx >= listSize)
            return 4;

        // Make room for one element if not appending
        if (idx < listCount)
        {
            const int moveCount = listCount - idx;
            memmove(&factoryList[idx + 1], &factoryList[idx],
                    sizeof(AppFactoryEntry) * moveCount);
        }
        factoryList[idx].factory = factory;
        factoryList[idx].info = info;
        factoryList[idx].type = type;
        ++listCount;
        APPMGR_LOG("Added \"%s\" (%X) factory at 0x%X to app factory list at position %u.", info->name, info->id, factoryList[idx].factory, idx);
        return 0;
    }

    int AppManager::AppFactoryList::removeFactory(AppID id)
    {
        if (id != 0)
        {
            for (int c = 0; c < listSize; ++c)
            {
                if (factoryList[c].info != nullptr)
                {
                    APPMGR_LOG("Removed \"%s\" (%X) factory at 0x%X", factoryList[c].info->name, factoryList[c].info->id, factoryList[c].factory);
                    memset(&factoryList[c], 0, sizeof(AppFactoryEntry));
                    --listCount;
                    return 0;
                }
            }
        }
        return 1; // not found
    }

    void AppManager::AppFactoryList::clearList()
    {
        memset(factoryList, 0, sizeof(factoryList));
        listCount = 0;
    }

    AppManager::AppFactory AppManager::AppFactoryList::getFactory(AppID id)
    {
        for (int c = 0; c < listSize; ++c)
        {
            if (factoryList[c].info != nullptr && factoryList[c].info->id == id)
                return factoryList[c].factory;
        }
        return nullptr;
    }

    AppID AppManager::AppFactoryList::getTypeID(AppScreenType type)
    {
        for (int c = 0; c < listSize; ++c)
        {
            if (factoryList[c].type == type && factoryList[c].info != nullptr)
                return factoryList[c].info->id;
        }
        return 0;
    }

    // --- AppService ------------------------------------------------------------

    static AppManager *appMgr = nullptr;
    AppManager *getAppManager()
    {
        if (!appMgr)
        {
            appMgr = new Apps::AppManager;
            if (!appMgr)
            {
                APPMGR_LOG("Failed to create AppManager.");
                return nullptr;
            }

            appMgr->registerErrorScreenFactory(DefaultErrorScreen::create, &DefaultErrorScreen::infoStatic());
            appMgr->registerLoadingScreenFactory(TMSS::createApp, &TMSS::infoStatic());
            int r = appMgr->init();
            if (r != 0)
            {
                APPMGR_LOG("Application manager failed to init (%i).", r);
                if (appMgr)
                {
                    delete appMgr;
                    appMgr = nullptr;
                }
                return nullptr;
            }
        }
        return appMgr;
    }

} // namespace Apps
