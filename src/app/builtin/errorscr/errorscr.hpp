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
#include "common/objects/sound.hpp"
#include "common/formats/wav.hpp"

using namespace Apps;

class DefaultErrorScreen : public ErrorScreenApp
{
private:
    const char *appName = "ErrorScreen(NRC)";
    AppVersion appVer = AppVersion(0, 0, 1);
    Util::Tween<uint8_t, Util::LinearEasing> colorIntensity;

    RectWH area;

    // Into/Outro Animation
    enum : uint8_t
    {
        STOP,
        INTRO_RUN,
        OUTRO_RUN,
        INTRO_INIT
    } animState = INTRO_RUN;

    Util::Tween<uint8_t, Util::LinearEasing> bgAlpha;
    Util::Tween<uint16_t, Util::QuadInEasing> jumpOut;
    Util::Tween<uint16_t, Util::QuadOutEasing> jumpIn;

    DefaultErrorScreen(ErrorScreenMessage *msg);

public:
    // Factory method when creating the message inside
    static DefaultErrorScreen *create(
        const char *title,
        const char *text,
        ErrorMessageStyle style,
        ErrorMessageIcon icon)
    {
        ErrorScreenMessage *nMsg = new ErrorScreenMessage(title, text, style, icon);
        if (!nMsg)
            nMsg = &eMsgUnknownMsg;

        return new DefaultErrorScreen(nMsg);
    }

    // Factory method when the user already has an ErrorScreenMessage*
    static DefaultErrorScreen *create(ErrorScreenMessage *msg)
    {
        if (!msg)
            msg = &eMsgUnknownMsg;
        return new DefaultErrorScreen(msg);
    }

    ~DefaultErrorScreen();

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