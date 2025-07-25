/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * loader.hpp - Created on 25-04-2025
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

/*
 * Error Screen Application
 * ==========================
 * Used to show any errors
 */

#pragma once
#include <cstring>
#include "app/app.hpp"
#include "common/util/rect.h"
#include "common/util/tween.hpp"

using namespace Apps;

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
    constexpr Strings() : str(nullptr), len(0) {}
    constexpr Strings(const char *str) : str(str)
    {
        if (str)
            len = strlen(str);
    }
    constexpr Strings(const char *str, int len) : str(str), len(len) {}
    const char *str;
    int len;
};

constexpr const uint16_t bgFadeTime = 500;           // Time in ms for BG to fade to black level to divert user attention
constexpr const uint16_t toastAnimTime = 250;        // Time in ms for error screen to pop in/pop out
constexpr const uint16_t borderFadeTime = 600;      // Time in ms for error screen border to fade between color intensity

constexpr const int MAX_EMSG_LENGTH = 200;
constexpr const char eMsgInfoStr[] = "INFORMATION: ";
constexpr const char eMsgWarningStr[] = "WARNING: ";
constexpr const char eMsgErrorStr[] = "ERROR: ";
constexpr const char eMsgCriticalStr[] = "CRITICAL ERROR: ";

constexpr const Strings eMsgStrList[] = {
    eMsgInfoStr,
    eMsgWarningStr,
    eMsgErrorStr,
    eMsgCriticalStr};

constexpr const char eMsgOptionNone[] = "Cannot proceed\r\nPlease restart the system";
constexpr const char eMsgOptionTest[] = "TEST BUTTON = Test menu";
constexpr const char eMsgOptionTestSrv[] = "SERVICE BUTTON = Continue\r\nTEST BUTTON = Test menu";

constexpr const Strings eMsgOptionList[] = {
    eMsgOptionNone,
    eMsgOptionTest,
    eMsgOptionTestSrv};

struct ErrorScreenMessage
{
    ErrorMessageIcon icon = ErrorMessageIcon::EM_ICON_DEFAULT;
    ErrorMessageStyle style = ErrorMessageStyle::EM_STYLE_DEFAULT;
    ErrorMessageOptions action = ErrorMessageOptions::EM_BUTTONS_TEST_SEVICE;
    Strings title, message;

    ErrorScreenMessage() {}
    ErrorScreenMessage(
        const char *title,
        const char *message,
        ErrorMessageStyle style = ErrorMessageStyle::EM_STYLE_DEFAULT,
        ErrorMessageIcon icon = ErrorMessageIcon::EM_ICON_DEFAULT,
        ErrorMessageOptions action = ErrorMessageOptions::EM_BUTTONS_TEST_SEVICE)
    {
        this->title = Strings(title, strnlen(title, MAX_EMSG_LENGTH));
        this->message = Strings(message, strnlen(message, MAX_EMSG_LENGTH));
        this->style = style;
        this->action = action;
    }
    ErrorScreenMessage(
        const char *title,
        int tLen,
        const char *message,
        int mLen,
        ErrorMessageStyle style = ErrorMessageStyle::EM_STYLE_DEFAULT,
        ErrorMessageIcon icon = ErrorMessageIcon::EM_ICON_DEFAULT,
        ErrorMessageOptions action = ErrorMessageOptions::EM_BUTTONS_TEST_SEVICE)
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
    ErrorMessageStyle::EM_STYLE_CRITICAL_ERROR,
    ErrorMessageIcon::EM_ICON_CRITICAL_ERROR};

class ErrorScreen : public Application
{
private:
    const char *appName = "ErrorScreen(NRC)";
    AppVersion appVer = AppVersion(0, 0, 1);
    Util::Tween<uint8_t, Util::LinearEasing> colorIntensity;

    ErrorScreenMessage *msg;
    RectWH area;

    // SoundObject errorSound;
    bool playSound = false;
    uint8_t playCount = 0;
    uint8_t maxCount = 4;

    // Into/Outro Animation
    enum : uint8_t
    {
        STOP,
        INTRO_RUN,
        OUTRO_RUN
    } animState = INTRO_RUN;
    Util::Tween<uint8_t, Util::LinearEasing> bgAlpha;
    Util::Tween<uint16_t, Util::QuadInEasing> jumpOut;
    Util::Tween<uint16_t, Util::QuadOutEasing> jumpIn;

    ErrorScreen(ErrorScreenMessage *msg);

public:
    // Factory method when creating the message inside
    static ErrorScreen *create(
        const char *title,
        const char *text,
        ErrorMessageStyle style,
        ErrorMessageIcon icon)
    {
        ErrorScreenMessage *nMsg = new ErrorScreenMessage(title, text, style, icon);
        if (!nMsg)
            nMsg = &eMsgUnknownMsg;

        return new ErrorScreen(nMsg);
    }

    // Factory method when the user already has an ErrorScreenMessage*
    static ErrorScreen *create(ErrorScreenMessage *msg)
    {
        if (!msg)
            msg = &eMsgUnknownMsg;
        return new ErrorScreen(msg);
    }

    ~ErrorScreen();

    int init();
    void update();
    void render();
    void reload();
    void shutdown();

    inline ErrorMessageStyle getSeverity()
    {
        if (msg)
            return msg->style;
        else
            return ErrorMessageStyle::EM_STYLE_INFO;
    }

    const char *name() { return appName; }
    int version() { return appVer.toInt(); }
};