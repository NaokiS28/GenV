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

#include "appmgr.hpp"

#define CloseApp(app) {app->shutdown(); delete app; app = nullptr;}

using namespace Apps;

extern "C++" Apps::Application* genv_register_app();

AppManager::AppManager()
{
    foregroundApp = nullptr;
    backgroundApp = nullptr;
    loadingScreen = nullptr;
    errorScreen = nullptr;
}

AppManager::~AppManager()
{
    shutdown();
}

bool AppManager::shutdown()
{
    if (foregroundApp) CloseApp(foregroundApp);
    if (backgroundApp) CloseApp(backgroundApp);
    if (loadingScreen) CloseApp(loadingScreen);
    if (errorScreen) CloseApp(errorScreen);
    return 1;
}

bool AppManager::reload()
{
    if (foregroundApp) foregroundApp->reload();
    if (backgroundApp) backgroundApp->reload();
    if (loadingScreen) loadingScreen->reload();
    if (errorScreen) errorScreen->reload();
    return 1;
}

int AppManager::init()
{
    loadingScreen = new Apps::TMSS;
    if( !loadingScreen){
        showErrorScreen("APP INIT FAILURE", "INVALID LOADER", EM_STYLE_CRITICAL_ERROR, EM_ICON_CRITICAL_ERROR);
        return -1;
    }
    foregroundApp = genv_register_app();
    if (!foregroundApp) {
        showErrorScreen("APP INIT FAILURE", "INVALID ENTRYPOINT", EM_STYLE_CRITICAL_ERROR, EM_ICON_CRITICAL_ERROR);
        return -1;
    }

    return 0;
}

int AppManager::update()
{
    if (!foregroundApp && !backgroundApp && !errorScreen)
    {
        // Both apps are gone, default
        showErrorScreen(
            "APPLICATION MANAGER", 
            "BOTH APP POINTERS ARE NULL",
            EM_STYLE_CRITICAL_ERROR,
            EM_ICON_CRITICAL_ERROR
        );
    }
    else
    {
        if (foregroundApp)
        {
            switch (foregroundApp->state)
            {
            case APP_STATE_RUN:
                foregroundApp->update();
                break;
            case APP_STATE_REGAINFOCUS:
                foregroundApp->update();
                foregroundApp->setAppState(APP_STATE_RUN);
                break;
            case APP_STATE_INIT:
                foregroundApp->init();
                break;
            case APP_STATE_ERROR:
                swapApps();
                break;
            case APP_STATE_QUIT:
                CloseApp(foregroundApp);
                swapApps();
                break;
            case APP_STATE_LOAD:
                foregroundApp->loadApp();
                break;
            default:
                break;
            }
        }
        else
        {
            swapApps();
        }

        if (backgroundApp)
        {
            backgroundApp->update();
            switch (backgroundApp->state)
            {
            case APP_STATE_INIT:
                backgroundApp->init();
                break;
            case APP_STATE_QUIT:
                CloseApp(backgroundApp);
                break;
            case APP_STATE_LOAD:
                backgroundApp->loadApp();
                break;
            default:
                break;
            }
        }
    }

    if (errorScreen)
    {
        switch (errorScreen->state)
        {
        case APP_STATE_RUN:
            errorScreen->update();
            break;
        case APP_STATE_INIT:
            errorScreen->init();
            break;
        case APP_STATE_QUIT:
            CloseApp(errorScreen);
            break;
        default:
            break;
        }
    }

    if (loadingScreen)
    {
        switch (loadingScreen->state)
        {
        case APP_STATE_RUN:
            loadingScreen->update();
            break;
        case APP_STATE_INIT:
            loadingScreen->init();
            break;
        case APP_STATE_QUIT:
            CloseApp(loadingScreen);
            break;
        case APP_STATE_LOAD:
            loadingScreen->loadApp();
            break;
        default:
            break;
        }
    }

    return 0;
}

int AppManager::render()
{
    if (backgroundApp && backgroundApp->isReady())
        backgroundApp->render(); // Drawn first and rearmost
    if (foregroundApp && foregroundApp->isReady())
        foregroundApp->render();
    if (loadingScreen && loadingScreen->isReady())
        loadingScreen->render(); // Drawn last and frontmost
    if (errorScreen && errorScreen->isReady())
        errorScreen->render(); // Drawn last and frontmost
    return 0;
}

void AppManager::swapApps()
{
    // Swaps foreground app to background and vice versa
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
    switch (app)
    {
    case APP_FOREGROUND:
        if (foregroundApp) foregroundApp->shutdown();
        if (backgroundApp) swapApps();
        break;
    case APP_BACKGROUND:
        if (backgroundApp) backgroundApp->shutdown();
        break;
    default:
        break;
    }
}

int AppManager::loadApp()
{

    return 0;
}

bool AppManager::showErrorScreen(
    const char *title, 
    const char *text, 
    ErrorMessageStyle style, 
    ErrorMessageIcon icon
){
    ErrorScreenMessage *esApp = new ErrorScreenMessage(title, text, style, icon);
    if(!esApp) return false;

    return showErrorScreen(esApp);
}

bool AppManager::showErrorScreen(ErrorScreenMessage *msg){
    if(!msg)
        return false;
    
    if(errorScreen){
        ErrorMessageStyle currSeverity = errorScreen->getSeverity();
        if(msg->style >= currSeverity){
            errorScreen->shutdown();
            delete errorScreen;
            errorScreen = nullptr;
        }
    }

    errorScreen = DefaultErrorScreen::create(msg);
    return (errorScreen != nullptr);
}