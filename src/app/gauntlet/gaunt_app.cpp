/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gaunt_app.cpp - Created on 12-06-2025
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

#include "gaunt_app.hpp"
#include "gaunt_screens.hpp"
#include "app/builtin/loadscr/loadscr.hpp"

namespace Gauntlet
{
    GauntletApp::GauntletApp() : Application()
    {
    }

    int GauntletApp::init()
    {
        changeApp(GauntletApps::G_Title_Screen);
        setAppState(APP_STATE_RUN);
        return 0;
    }

    void GauntletApp::loadApp()
    {
        setAppState(APP_STATE_INIT);
        appReady = true;
    }

    void GauntletApp::reload()
    {
    }

    void GauntletApp::update()
    {
        if (currentApp != nullptr)
        {
            switch (currentApp->getState())
            {
            default:
                break;
            case APP_STATE_LOAD:
                currentApp->loadApp();
                break;
            case APP_STATE_INIT:
                currentApp->init();
                break;
            case APP_STATE_RUN:
                currentApp->update();
                break;
            case APP_STATE_QUIT:
                if (newScreen != nullptr)
                {
                    _swapApps();
                }
                break;
            }
        } else if (newScreen != nullptr){
            _swapApps();
        }
    }

    void GauntletApp::changeApp(GauntletApps newApp)
    {
        newScreen = createNewApp(newApp);
        if (newScreen != nullptr && currentApp != nullptr)
        {
            currentApp->shutdown();
        }
    }

    void GauntletApp::render()
    {
        if (currentApp != nullptr && currentApp->getState() == APP_STATE_RUN && currentApp->isReady())
        {
            currentApp->render();
        }
    }

    int GauntletApp::loadProgress(const char *&str)
    {
        return 100;
    }

    void GauntletApp::_swapApps()
    {
        if (newScreen != nullptr)
        {
            if(currentApp != nullptr) 
                delete currentApp;
            currentApp = newScreen;
            newScreen = nullptr;
            currentApp->setAppChangeCallback(&GauntletApp::staticAppChangeCallback, this);
            //setAppState(APP_STATE_LOAD);
        }
    }
}