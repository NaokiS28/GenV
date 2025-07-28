/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * app.cpp - Created on 09-05-2025
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

#include "app.hpp"
#include "common/services/services.hpp"

using namespace Apps;

Application::Application() : gpu(Services::getVideo()),
                             state(APP_STATE_LOAD),
                             appReady(false)
{
}

Application::Application(IVideo *_gpu) : gpu(_gpu),
                                         state(APP_STATE_LOAD),
                                         appReady(false)
{
}

int Application::loadProgress(const char *&str)
{
    str = defaultLoadString;
    return 100;
}

/* Application Loader Preset */

LoadScreenApp::LoadScreenApp() : Application(Services::getVideo())
{
}

int LoadScreenApp::init()
{
    return 0;
}

void LoadScreenApp::update()
{
}

ErrorScreenApp::ErrorScreenApp() : Application(Services::getVideo())
{
}

ErrorScreenApp::~ErrorScreenApp()
{
    state = APP_STATE_SHUTDOWN;
    if (msg && msg != &eMsgUnknownMsg)
        delete msg;
    if (errorSound != nullptr)
        delete errorSound;
}

int ErrorScreenApp::init()
{
    return 0;
}

void ErrorScreenApp::update()
{
}