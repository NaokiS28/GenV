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

namespace Apps
{
    Application::Application(IAppHost *host) : gpu(getServiceManager()->getVideo())
    {
        m_host = host;
        state  = APP_STATE_LOAD;
        type   = APP_TYPE_NORMAL_APP;
    }

    ErrorScreenMessage::ErrorScreenMessage(
        const char *title,
        const char *message,
        const uint32_t errorCode,
        ErrorMessageStyle style,
        ErrorMessageIcon icon,
        ErrorMessageOptions action)
    {
        this->title   = Strings(title, strnlen(title, MAX_EMSG_LENGTH));
        this->message = Strings(message, strnlen(message, MAX_EMSG_LENGTH));
        this->style   = style;
        this->action  = action;
    }

    ErrorScreenMessage::ErrorScreenMessage(
        const char *title,
        int tLen,
        const char *message,
        int mLen,
        const uint32_t errorCode,
        ErrorMessageStyle style,
        ErrorMessageIcon icon,
        ErrorMessageOptions action)
    {
        this->title.str   = title;
        this->title.len   = tLen;
        this->message.str = message;
        this->message.len = mLen;
        this->style       = style;
        this->icon        = icon;
        this->action      = action;
    }

    Strings::Strings()
    {
    }

    Strings::Strings(const char *str)
    {
        this->str = str;
        if (str)
            this->len = strlen(str);
    }

    Strings::Strings(const char *str, int len)
    {
        this->str = str;
        this->len = len;
    }

    Application::Application(IAppHost *host, Video::IVideo *_gpu) : gpu(_gpu)
    {
        m_host = host;
        state  = APP_STATE_LOAD;
    }

    int Application::loadProgress(const char *&str)
    {
        str = defaultLoadString;
        return 100;
    }

    /* Application Loader Preset */

    LoadScreenApp::LoadScreenApp(IAppHost *host, Application *appToLoad)
        : Application(host, getServiceManager()->getVideo()),
          _appToLoad(appToLoad)
    {
        m_host = host;
        type   = APP_TYPE_LOADING_SCREEN;
    }

    int LoadScreenApp::init()
    {
        return 0;
    }

    void LoadScreenApp::update()
    {
    }

    ErrorScreenApp::ErrorScreenApp(IAppHost *host) : Application(host, getServiceManager()->getVideo())
    {
        m_host = host;
        type   = APP_TYPE_ERROR_SCREEN;
    }

    ErrorScreenApp::~ErrorScreenApp()
    {
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

    /* Arcade Test App Preset */

    ArcadeTestApp::ArcadeTestApp(IAppHost *host) : Application(host, getServiceManager()->getVideo())
    {
        m_host  = host;
        aSystem = System::GetArcadeInterface();
        type    = APP_TYPE_ARCADE_TEST_APP;
    }

    int ArcadeTestApp::init()
    {
        return 0;
    }

    void ArcadeTestApp::update()
    {
    }

    ArcadeTestApp *getArcadeTestApp(Application *app)
    {
        if (!app || app->getAppType() != APP_TYPE_ARCADE_TEST_APP)
            return nullptr;
        return static_cast<ArcadeTestApp *>(app);
    }
} // namespace Apps