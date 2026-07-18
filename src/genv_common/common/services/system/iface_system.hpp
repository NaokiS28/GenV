/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * iface_system.hpp - Created on 09-08-2025
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

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <time.h>

#include "timer.hpp"

namespace Video
{
    class Screen; // System owns the drivers and exposes their screens
    struct ScreenConfig; //! Review - config a driver hands assignScreen
}

#define SYSTEM_CALLBACK(name, type, func)                 \
    {                                                     \
        name,                                             \
        [](void *arg)                                     \
        {                                                 \
            auto service = reinterpret_cast<type *>(arg); \
            service->func();                              \
        },                                                \
        this}

namespace System
{
    struct SystemInfo;
    class IVideoDriver; //! Review - assignScreen wires a screen to its video driver

    typedef void (*CallbackFunction)(void *arg);
    class Callback
    {
    private:
        const char *m_name      = nullptr;
        CallbackFunction m_func = nullptr;
        void *m_arg             = nullptr;

    public:
        Callback() {}
        Callback(const char *name, CallbackFunction func, void *arg) : m_name(name), m_func(func), m_arg(arg) {}
        inline const char *name() { return m_name; }
        inline bool isValid() { return m_func != nullptr; }
        inline void call()
        {
            if (isValid()) m_func(m_arg);
        }
    };

    class ISystem
    {
    public:
        virtual ~ISystem() = default;

        virtual int initCore()    = 0; // Init system core
        virtual int initVideo()   = 0;
        virtual int initAudio()   = 0;
        virtual int initIO()      = 0;
        virtual int initStorage() = 0;

        virtual int update()    = 0; // Update system manager
        virtual void shutdown() = 0; // Prepare for app shutdown

        // Returns the screen (render context) at the given index, or nullptr if no
        // screen is registered there. Null is a checkable "failed access" - callers
        // must guard it (Screen draw calls forward straight into the driver).
        virtual Video::Screen *getScreen(uint8_t idx) = 0;

        //! Review
        // A video driver calls this from its init() to register the screen(s) it
        // drives. The system allocates the slot, mints + owns the Screen and returns
        // it (nullptr on failure). cfg.name == nullptr -> the slot's DISPLAY default.
        virtual Video::Screen *assignScreen(IVideoDriver *driver, const Video::ScreenConfig &cfg) = 0;
        //! End

        virtual void enterCriticalSection() = 0;
        virtual void leaveCriticalSection() = 0;

        virtual const SystemInfo *getSysInfo() const = 0;

        virtual size_t millis() = 0; // Millis since system start
        virtual size_t micros() = 0; // Micros since system start
        virtual size_t random(size_t min, size_t max)
        { // Gets a random number between given values
            return ((min + rand()) % max);
        }
        virtual bool getTime(tm &time) = 0;

        virtual const char *getWorkingDirectory() = 0;

        virtual bool registerTimerFunc(TFunc func, TChannel timer, uint8_t freq) = 0;
        virtual bool unregisterTimerFunc(TFunc func, TChannel timer)             = 0;
    };
} // namespace System