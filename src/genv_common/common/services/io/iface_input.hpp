/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * input.hpp - Created on 09-05-2025
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

#include "common/util/hash.hpp"

namespace Input
{
    const char szGenericInputDevice[] = "Generic input device";

    enum InputDeviceType : uint8_t
    {
        DEVICE_TYPE_CONTROLLER,
        DEVICE_TYPE_KEYBOARD,
        DEVICE_TYPE_MOUSE
    };

    class IInputDriver
    {
    protected:
        const char *_name = nullptr;

    public:
        IInputDriver()          = default;
        virtual ~IInputDriver() = default;
        virtual int init()      = 0;
        virtual int update()    = 0;
        virtual bool reset()    = 0;
        virtual void shutdown() = 0;

        const char *getName() { return _name; }
    };

    class IInputDevice
    {
    private:
        const InputDeviceType _deviceType;
        const util::Hash _deviceID;

        uint8_t _frameCount = 0;

    protected:
        const char *_name      = szGenericInputDevice;
        uint8_t _pollFrequency = 1; // In frames, i.e. 0 = Don't auto-poll, 1 = every frame

        uint8_t _busType = 0;
        uint8_t _busID   = 0;

    public:
        IInputDevice(InputDeviceType type, util::Hash id) : _deviceType(type), _deviceID(id) {}
        inline virtual void shutdown() {}

        InputDeviceType getType() { return _deviceType; }
        inline util::Hash getID() { return _deviceID; }
        inline const char *getName() { return _name; }
        inline uint8_t getBusType() { return _busType; }
        inline uint8_t getBusID() { return _busID; }

        virtual int poll() = 0;

        inline bool canPoll()
        {
            _frameCount++;
            if (_frameCount == _pollFrequency)
            {
                _frameCount = 0;
                return true;
            }
            return false;
        }
    };

    class IController : public IInputDevice
    {
    public:
        IController(InputDeviceType type, util::Hash id) : IInputDevice(type, id) {}
        virtual ~IController() = default;
    };

    class IKeyboard : public IInputDevice
    {
    public:
        IKeyboard(InputDeviceType type, util::Hash id) : IInputDevice(type, id) {}
        virtual ~IKeyboard() = default;
    };

    class IMouse : public IInputDevice
    {
    public:
        IMouse(InputDeviceType type, util::Hash id) : IInputDevice(type, id) {}
        virtual ~IMouse() = default;
    };

    class NullInput;
} // namespace Input