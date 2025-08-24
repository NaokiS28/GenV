/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * app.hpp - Created on 24-04-2025
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
#include <stdbool.h>

#include "app_errorcodes.hpp"
#include "common/util/hash.hpp"

#include "common/logger/log.hpp"
#include "common/services/system/iface_system.hpp"
#include "common/services/system/arcade/iface_arcade.hpp"
#include "common/services/video/video.hpp"
#include "common/objects/sound.hpp"

namespace Apps
{

    // Forward declaration, in appmgr.hpp
    class AppManager;

    // Forward declaration, in iapp_host.hpp
    struct IAppHost;

    static constexpr const char *defaultLoadString = "Loading...";

    using AppID = util::Hash;

    enum AppExecState : uint8_t
    {
        APP_STATE_LOAD,        // Initial state, app is currently loading required resources
        APP_STATE_INIT,        // App needs to run init phase
        APP_STATE_RUN,         // App is running normally
        APP_STATE_REGAINFOCUS, // App has returned to the forground (used if reloading or reset is needed)
        APP_STATE_OUTOFFOCUS,  // App has been demoted to the background application and should not run unecessary code
        APP_STATE_ERROR,       // App encountered a severe error and has to stop
        APP_STATE_SHUTDOWN,    // App has been requested to shutdown
        APP_STATE_QUIT         // App has finished and is ready to be deleted from memory
    };

    constexpr const char *getAppStateString(AppExecState state)
    {
        switch (state)
        {
        case APP_STATE_LOAD:
            return "App loading";
        case APP_STATE_INIT:
            return "App initialising";
        case APP_STATE_RUN:
            return "App running";
        case APP_STATE_REGAINFOCUS:
            return "App gain focus";
        case APP_STATE_OUTOFFOCUS:
            return "App lost focus";
        case APP_STATE_ERROR:
            return "App error occured";
        case APP_STATE_SHUTDOWN:
            return "App shutting down";
        case APP_STATE_QUIT:
            return "App quit";
        default:
            return "Unkown state";
        }
    }

    enum AppType : uint8_t
    {
        APP_TYPE_UNSET,
        APP_TYPE_NORMAL_APP,
        APP_TYPE_ARCADE_TEST_APP,
        APP_TYPE_INFO_SCREEN,
        APP_TYPE_ERROR_SCREEN,
        APP_TYPE_LOADING_SCREEN
    };

    struct AppVersion
    {
        int major;
        int minor;
        int patch;

        constexpr AppVersion(int maj, int min, int pat)
            : major(maj), minor(min), patch(pat) {}

        constexpr int toInt() const
        {
            return major * 10000 + minor * 100 + patch;
        }
    };

    struct AppInfo
    {
        const char *name;
        const char *maker;
        AppID id;
        AppVersion version;
    };

    constexpr AppInfo makeAppInfo(const char *name, const char *maker, AppVersion ver)
    {
        return AppInfo{name, maker, (util::hash(name, -1, 0) + util::hash(maker, -1, 0)), ver};
    }

    /*
     * Applications using the Application class are programs that are ran by the engine
     * which allows it to be more modular and change functions rather easily. All apps
     * start in a loading state where they are expected to load their required files and
     * then signal their ready state by setting appReady to true. At which point AppManager
     * will then transition to that application as the foreground app.
     */
    class Application
    {
        friend class AppManager;

    private:
        AppExecState state; // Current app working state

    protected:
        Video::IVideo *gpu; // Local pointer to GPU object to use
        AppType type;
        IAppHost *m_host = nullptr;
        void setHost(IAppHost *host) { m_host = host; }
        void setAppState(AppExecState state)
        {
            LOG("app", "\"%s\" app state changed to: %s", name(), getAppStateString(state));
            this->state = state;
        }

    public:
        Application();
        Application(Video::IVideo *_gpu);
        virtual ~Application() = default;

        inline virtual bool isReady() { return state != APP_STATE_LOAD; }
        inline virtual AppExecState getState() { return state; }
        inline virtual AppType getAppType() { return type; }

        // App information for logging
        virtual const AppInfo &info() const = 0;
        const char *name() const { return info().name; }
        const char *maker() const { return info().maker; }
        int version() const { return info().version.toInt(); }
        AppID id() const { return info().id; }

        virtual int loadProgress(const char *&str); // State of initial loading progress in percent (str for custom loading text)

        virtual int init(IAppHost *host) = 0;                        // Initialize app
        virtual void update() = 0;                                   // Update the app's logic
        virtual void render() = 0;                                   // Request app to draw it's UI
        virtual void loadApp() { setAppState(APP_STATE_INIT); }      // Call app to load next file/object (app handles any loading list)
        virtual void shutdown() { setAppState(APP_STATE_SHUTDOWN); } // Request app to begin unloading and finilization
        virtual void reload() {}                                     // Request app to restart from the begining (or reload assets)
    };

    /*
     * LoadScreenApp is a special application class that is used as an overlay to load
     * additional applications by the AppManager. Allows for different loading screens
     * for different sections, such as a generic one for loading the game program, and
     * a different loading screen prior to gameplay.
     */
    class LoadScreenApp : public Application
    {
        friend class AppManager;

    private:
        Application *app;

    public:
        LoadScreenApp();

        virtual int init(IAppHost *host); // Init the loading screen
        virtual void update();            // Update the loading screens logic
        virtual void render() = 0;        // Request loading screen to render it's UI
        virtual void setAppToLoad(Application *app)
        {
            this->app = app;
        }
    };

    enum ErrorMessageIcon : uint8_t
    {
        EM_ICON_INFO,
        EM_ICON_WARNING,
        EM_ICON_ERROR,
        EM_ICON_CRITICAL_ERROR,
        EM_ICON_DEFAULT = EM_ICON_ERROR,
        EM_ICON_CUSTOM
    };

    enum ErrorMessageStyle : uint8_t
    {
        EM_STYLE_INFO,
        EM_STYLE_WARNING,
        EM_STYLE_ERROR,
        EM_STYLE_CRITICAL_ERROR,
        EM_STYLE_DEFAULT = EM_STYLE_ERROR
    };

    enum ErrorMessageOptions : uint8_t
    {
        EM_BUTTONS_NONE,
        EM_BUTTONS_TEST,
        EM_BUTTONS_TEST_SEVICE
        // EM_BUTTONS_CUSTOM
    };

    struct Strings
    {
        Strings();
        Strings(const char *str);
        Strings(const char *str, int len);
        const char *str;
        int len = 0;
    };

    constexpr const uint16_t bgFadeTime = 500;     // Time in ms for BG to fade to black level to divert user attention
    constexpr const uint16_t toastAnimTime = 250;  // Time in ms for error screen to pop in/pop out
    constexpr const uint16_t borderFadeTime = 500; // Time in ms for error screen border to fade between color intensity

    constexpr const int MAX_EMSG_LENGTH = 200;
    constexpr const char eMsgInfoStr[] = "INFORMATION: ";
    constexpr const char eMsgWarningStr[] = "WARNING: ";
    constexpr const char eMsgErrorStr[] = "ERROR: ";
    constexpr const char eMsgCriticalStr[] = "CRITICAL ERROR: ";

    constexpr const char eMsgSoundFile[] = "C:\\Users\\Naoki\\Nextcloud\\Programming\\GenV\\bin\\KBMHELL.wav";

    const Strings eMsgStrList[] = {
        eMsgInfoStr,
        eMsgWarningStr,
        eMsgErrorStr,
        eMsgCriticalStr};

    constexpr const char eMsgOptionNone[] = "Cannot proceed\r\nPlease restart the system";
    constexpr const char eMsgOptionTest[] = "TEST BUTTON = Test menu";
    constexpr const char eMsgOptionTestSrv[] = "SERVICE BUTTON = Continue\r\nTEST BUTTON = Test menu";

    const Strings eMsgOptionList[] = {
        eMsgOptionNone,
        eMsgOptionTest,
        eMsgOptionTestSrv};

    struct ErrorScreenMessage
    {
        ErrorMessageIcon icon = ErrorMessageIcon::EM_ICON_DEFAULT;
        ErrorMessageStyle style = ErrorMessageStyle::EM_STYLE_DEFAULT;
        ErrorMessageOptions action = ErrorMessageOptions::EM_BUTTONS_TEST_SEVICE;
        uint32_t errorCode = 0x0;
        Strings title, message;

        ErrorScreenMessage() {}
        ErrorScreenMessage(
            const char *title,
            const char *message,
            const uint32_t errorCode,
            ErrorMessageStyle style = ErrorMessageStyle::EM_STYLE_DEFAULT,
            ErrorMessageIcon icon = ErrorMessageIcon::EM_ICON_DEFAULT,
            ErrorMessageOptions action = ErrorMessageOptions::EM_BUTTONS_TEST_SEVICE);
        ErrorScreenMessage(
            const char *title,
            int tLen,
            const char *message,
            int mLen,
            const uint32_t errorCode,
            ErrorMessageStyle style = ErrorMessageStyle::EM_STYLE_DEFAULT,
            ErrorMessageIcon icon = ErrorMessageIcon::EM_ICON_DEFAULT,
            ErrorMessageOptions action = ErrorMessageOptions::EM_BUTTONS_TEST_SEVICE);
    };

    static ErrorScreenMessage eMsgUnknownMsg = {
        "General Application Error", 26,
        "An unexpected and critical error has occured. The application has been stopped.", 80,
        GENV_APP_ERR_UNKNOWN,
        ErrorMessageStyle::EM_STYLE_CRITICAL_ERROR,
        ErrorMessageIcon::EM_ICON_CRITICAL_ERROR};

    /*
     * ErrorScreenApp is a special application class that is used as an overlay an error
     * message when a game encounters a serious error.
     */
    class ErrorScreenApp : public Application
    {
        friend class AppManager;

    protected:
        ErrorScreenMessage *msg = nullptr;
        Audio::SoundObject *errorSound = nullptr;
        bool playSound = false;
        uint8_t playCount = 0;
        uint8_t maxCount = 1;

    public:
        ErrorScreenApp();
        ~ErrorScreenApp();

        inline ErrorMessageStyle getSeverity()
        {
            if (msg)
                return msg->style;
            else
                return ErrorMessageStyle::EM_STYLE_INFO;
        }

        virtual int init(IAppHost *host); // Init the error screen
        virtual void update();            // Update the error screens logic
        virtual void render() = 0;
    };

    /*
     * ArcadeTestApp is a special application class that is used only on arcade games
     * and when the system is in service/test mode. Games that are to be used in arcade
     * systems MUST implement a test mode and register it within genv_register_app.
     */
    class ArcadeTestApp : public Application
    {
        friend class AppManager;

    protected:
        System::IArcadeSystem *aSystem = nullptr;

    public:
        ArcadeTestApp();

        virtual int init(IAppHost *host); // Init the loading screen
        virtual void update();            // Update the loading screens logic
        virtual void render() = 0;        // Request loading screen to render it's UI
    };

    ArcadeTestApp *getArcadeTestApp(Application *app);
}