/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * app.hpp - Created on 24-04-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "common/services/video/video.hpp"

namespace Apps
{
    using namespace Video;

    // Forward declaration, in appmgr.hpp
    class AppManager;

    static constexpr const char *defaultLoadString = "Loading...";

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

    protected:
        IVideo *gpu;        // Local pointer to GPU object to use
        AppExecState state; // Current app working state
        bool appReady;      // App has loaded and ready to be run

        virtual void setAppState(AppExecState state) { state = state; }

    public:
        Application(IVideo *_gpu);
        virtual ~Application() = default;

        inline virtual bool isReady() { return appReady; }

        virtual int version() = 0;      // Return app's version for logging
        virtual const char *name() = 0; // Return app's name for logging

        virtual int loadProgress(const char *&str); // State of initial loading progress in percent (str for custom loading text)

        virtual int init() = 0;    // Initialize app
        virtual void update() = 0; // Update the app's logic
        virtual void render() = 0; // Request app to draw it's UI
        virtual void loadApp() {}  // Call app to load next file/object (app handles any loading list)
        virtual void shutdown() {} // Request app to begin unloading and finilization
        virtual void reload() {}   // Request app to restart from the begining (or reload assets)
    };

    /*
     * AppLoader is a special application class that is used as an overlay to load
     * additional applications by the AppManager. Allows for different loading screens
     * for different sections, such as a generic one for loading the game program, and
     * a different loading screen prior to gameplay.
     */
    class AppLoader : public Application
    {
        friend class AppManager;

    private:
        Application *app;

    public:
        AppLoader();

        virtual int init();        // Init the loading screen
        virtual void update();     // Update the loading screens logic
        virtual void render() = 0; // Request loading screen to render it's UI
    };
}