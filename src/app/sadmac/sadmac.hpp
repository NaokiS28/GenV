/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * sadmac.hpp - Created on 09-06-2025
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
#include "app/builtin/errorscr/errorscr.hpp"
#include "common/objects/sprite.hpp"

constexpr const char sadMacSoundFile[] = "C:\\Users\\Naoki\\Nextcloud\\Programming\\GenV\\bin\\sadmac.wav";
constexpr const char sadCabImageFile[] = "C:\\Users\\Naoki\\Nextcloud\\Programming\\GenV\\bin\\sadcab.gif";

class SadMac : public Application
{
private:
    const char *appName = "SadMac(NRC)";
    AppVersion appVer = AppVersion(0, 0, 1);

    ErrorScreenMessage *msg = nullptr;
    Coord imgOrigin;
    Coord titleOrigin;
    Coord msgOrigin;

    Audio::SoundObject *errorSound = nullptr;
    bool playSound = false;
    uint8_t playCount = 0;
    uint8_t maxCount = 1;

    Sprites::SpriteObject *sadCab = nullptr;

    SadMac(ErrorScreenMessage *msg);

public:
    // Factory method when creating the message inside
    static SadMac *create(
        const char *title,
        const char *text,
        ErrorMessageStyle style,
        ErrorMessageIcon icon)
    {
        ErrorScreenMessage *nMsg = new ErrorScreenMessage(title, text, style, icon);
        if (!nMsg)
            nMsg = &eMsgUnknownMsg;

        return new SadMac(nMsg);
    }

    // Factory method when the user already has an ErrorScreenMessage*
    static SadMac *create(ErrorScreenMessage *msg)
    {
        if (!msg)
            msg = &eMsgUnknownMsg;
        return new SadMac(msg);
    }

    ~SadMac();

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