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

#include "common/return_codes.hpp"
#include "common/util/hash.hpp"

namespace Input
{
    const char szNullInputDevice[]    = "Null input device";
    const char szGenericInputDevice[] = "Generic input device";

    // Tells the input mapper what the device class is.
    // Note: Mouse specifically implies a relative(delta) positioning device, whilst lightgun implies absolute positioning device.
    enum InputDeviceType : uint8_t
    {
        DEVICE_TYPE_NULL,
        DEVICE_TYPE_CONTROLLER,
        DEVICE_TYPE_KEYBOARD,
        DEVICE_TYPE_MOUSE,
        DEVICE_TYPE_LIGHTGUN,
    };

    enum InputDeviceSubType : uint8_t
    {
        DEVICE_SUBTYPE_NULL,
        DEVICE_SUBTYPE_STANDARD,
    };

    // Player suggestion tells the input mapper how to handle a device.
    // If set to any, then this controller will be mapped to the next unmapped player (i.e. 1 >> 2 >> 3..8), else it will not be registered.
    // --
    // DEVICE_PLAYER_n:
    // If the player number is set, it will tell the mapper to append this device to a specific player.
    // Exmaple: If a device suggests only DEVICE_PLAYER_2, then even if player 2 already has a device attached,
    // it will attach this second device and buttons on either will be used for player 2.
    // --
    // (DEVICE_PLAYER_n | DEVICE_PLAYER_n | ...):
    // If two or more player numbers are given, then the mapper will interpret this as saying to map to one of the given numbers,
    // with the lower player number giving priority if they dont have a controller attached.
    // --
    // DEVICE_ARCADE_CABINET:
    // This type is special and means this is the service panel controls.
    // Service controls MUST be given this suggestion to work, and multiple service controls can be appended.
    enum PlayerSuggestion : uint8_t
    {
        DEVICE_PLAYER_ANY,
        DEVICE_PLAYER_1       = (1 << 0),
        DEVICE_PLAYER_2       = (1 << 1),
        DEVICE_PLAYER_3       = (1 << 2),
        DEVICE_PLAYER_4       = (1 << 3),
        DEVICE_PLAYER_5       = (1 << 4),
        DEVICE_PLAYER_6       = (1 << 5),
        DEVICE_PLAYER_7       = (1 << 6),
        DEVICE_PLAYER_8       = (1 << 7),
        DEVICE_ARCADE_CABINET = 0xFF
    };

    // Base class definition for input drivers. All input sources must have a driver associated with it.
    // This can be one driver for a static, single device (see Konami 573 JAMMA driver), or it can be
    // one driver for many types of devices (I.E, Windows HID input).
    class IInputDriver
    {
    protected:
        const char *_name = nullptr;

    public:
        IInputDriver()          = default;
        virtual ~IInputDriver() = default;
        virtual int init() { return GV_OK; };
        virtual int update() = 0;
        virtual bool reset() { return GV_OK; };
        virtual void shutdown() {};

        const char *getName() { return _name; }
    };

    // Input device controller descriptor - Used to define the behavoir of this input device.
    // Note that all controller data is expected to be in the defined GenV controller data format.
    struct IInputDevice
    {
        const char *name           = szGenericInputDevice; // Friendly printable name
        util::Hash id              = "NULL"_h;             // Device ID - Usage is defined by the driver implementation
        util::Hash subid           = "NULL"_h;             // Device SubID - Usage is defined by the driver implementation
        InputDeviceType type       = DEVICE_TYPE_NULL;     // Device Type - Tells the input mapper what class the device is
        InputDeviceSubType subtype = DEVICE_SUBTYPE_NULL;  // Device Subtype - Not presently used
        uint8_t subBusID           = 0;                    // Sub Bus ID - Usage is defined by the driver implementation
        PlayerSuggestion player    = DEVICE_PLAYER_ANY;    // Player Suggestion to give to the mapper
        struct
        {
            uint8_t numDigital  = 0; // Number of digital inputs
            uint8_t numAnalog   = 0; // Number of analog inputs
            uint8_t numRotary   = 0; // Number of relative inputs
            uint8_t numFeedback = 0; // Number of feedback/output drivers
        } capabilities;
        struct
        {
            uint32_t *digital = nullptr; // Pointer to the formatted digital button data (as bool, expects to be an array ptr above 32 inputs)
            int16_t *analog   = nullptr; // Pointer to the analog position data (as int16, always expects to be an array ptr)
            int16_t *rotary   = nullptr; // Pointer to the relative position data (as int16, always expects to be an array ptr)
        } inputs;
    };

    class NullInput;
} // namespace Input