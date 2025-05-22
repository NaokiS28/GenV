/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * shellapp.hpp - Created on 12-05-2025
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
#include "app/app.hpp"
#include "common/services/services.hpp"

using namespace Apps;

class PSXShell : public Application {
    private:
        const char *appName = "PSX Shell";
        const AppVersion appVer = AppVersion(0, 0, 1);

    public:
    PSXShell();
    ~PSXShell();

    int version(){ return appVer.toInt(); };               // Return app's version for logging
    const char *name(){ return appName; };          // Return app's name for logging

    int loadProgress(const char *&str){ return 0; } // State of initial loading progress in percent (str for custom loading text)

    int init(){ return 0; }                     // Initialize app
    void update(){}                  // Update the app's logic
    void render(){}                  // Request app to draw it's UI
    void shutdown() {}                  // Request app to begin unloading and finilization
};