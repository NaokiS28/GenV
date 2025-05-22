/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * loader.hpp - Created on 25-04-2025
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

/*
 * Error Screen Application
 * ==========================
 * Used to show any errors
 */

#pragma once
#include <cstring>
#include "app/app.hpp"
#include "common/util/rect.h"

using namespace Apps;

enum ErrorIconStyle : uint8_t
{
    EM_ICON_INFO,
    EM_ICON_WARNING,
    EM_ICON_ERROR,
    EM_ICON_CRITICAL_ERROR,
    EM_ICON_DEFAULT = EM_ICON_ERROR
};

enum ErrorMessageStyle : uint8_t
{
    EM_STYLE_INFO,
    EM_STYLE_WARNING,
    EM_STYLE_ERROR,
    EM_STYLE_CRITICAL_ERROR,
    EM_STYLE_DEFAULT = EM_STYLE_ERROR
};

struct Strings
{
    Strings() : str(nullptr), len(0) {}
    Strings(const char *str) : str(str)
    {
        if (str)
            len = strlen(str);
    }
    Strings(const char *str, int len) : str(str), len(len) {}
    const char *str;
    int len;
};

static const int MAX_EMSG_LENGTH = 200;
static const char eMsgInfoStr[] = "INFORMATION: ";
static const char eMsgWarningStr[] = "WARNING: ";
static const char eMsgErrorStr[] = "ERROR: ";
static const char eMsgCriticalStr[] = "CRITICAL ERROR: ";

static const Strings eMsgStrList[] = {
    eMsgInfoStr,
    eMsgWarningStr,
    eMsgErrorStr,
    eMsgCriticalStr};

static const char eMsgOptionNone[] = "Cannot proceed\r\nPlease restart the system";
static const char eMsgOptionTest[] = "TEST BUTTON = Test menu";
static const char eMsgOptionTestSrv[] = "SERVICE BUTTON = Continue\r\nTEST BUTTON = Test menu";

static const Strings eMsgOptionList[] = {
    eMsgOptionNone,
    eMsgOptionTest,
    eMsgOptionTestSrv
};

enum ErrorMessageOptions : uint8_t {
    EM_BUTTONS_NONE,
    EM_BUTTONS_TEST,
    EM_BUTTONS_TEST_SEVICE
    //EM_BUTTONS_CUSTOM
};

struct ErrorScreenMessage
{
    int icon = EM_ICON_DEFAULT;
    int style = EM_STYLE_DEFAULT;
    int action = EM_BUTTONS_TEST_SEVICE;
    Strings title, message;

    ErrorScreenMessage() {}
    ErrorScreenMessage(const char *title, const char *message, int style = EM_STYLE_DEFAULT, int action = EM_BUTTONS_TEST_SEVICE, int icon = EM_ICON_DEFAULT)
    {
        this->title = Strings(title, strnlen(title, MAX_EMSG_LENGTH));
        this->message = Strings(message, strnlen(message, MAX_EMSG_LENGTH));
        this->style = style;
        this->action = action;
    }
    ErrorScreenMessage(const char *title, int tLen, const char *message, int mLen, int style = EM_STYLE_DEFAULT, int action = EM_BUTTONS_TEST_SEVICE, int icon = EM_ICON_DEFAULT)
    {
        this->title.str = title;
        this->title.len = tLen;
        this->message.str = message;
        this->message.len = mLen;
        this->style = style;
        this->icon = icon;
        this->action = action;
    }
};

static ErrorScreenMessage eMsgUnknownMsg = {
    "General Application Error", 26,
    "An unexpected and critical error has occured. The application has been stopped.", 80,
    EM_STYLE_CRITICAL_ERROR,
    EM_ICON_CRITICAL_ERROR
};

class ErrorScreen : public Application
{
private:
    const char *appName = "ErrorScreen(NRC)";
    AppVersion appVer = AppVersion(0, 0, 1);

    ErrorScreenMessage *msg;
    RectWH area;

public:
    ErrorScreen(ErrorScreenMessage *msg);
    ErrorScreen(const char *title, const char *text, int style = EM_STYLE_DEFAULT, int icon = EM_ICON_DEFAULT);
    ~ErrorScreen();

    int init();
    void update();
    void render();
    void reload();
    void shutdown() {}

    const char *name() { return appName; }
    int version() { return appVer.toInt(); }
};