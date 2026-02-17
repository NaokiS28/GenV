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
#include <cstdint>

namespace Input
{
    constexpr util::Hash idNull = 0;
    const char szNullInputDevice[] = "Null input device";
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

    // Player flags - Used for both player suggestions (device registration) and tracking available players.
    // --
    // Player::ANY:
    // If set to any, then this controller will be mapped to the next unmapped player (i.e. 1 >> 2 >> 3..8), else it will not be registered.
    // --
    // Player::n:
    // If the player number is set, it will tell the mapper to append this device to a specific player.
    // Example: If a device suggests only Player::PLAYER_2, then even if player 2 already has a device attached,
    // it will attach this second device and buttons on either will be used for player 2.
    // --
    // (Player::n | Player::n | ...):
    // If two or more player numbers are given, then the mapper will interpret this as saying to map to one of the given numbers,
    // with the lower player number giving priority if they dont have a controller attached.
    // If both players have a controller attached, then it will try to assign as a secondary controller to the lower player number first.
    // --
    // ARCADE_CABINET:
    // This is arcade specific!
    // This type is special and means this is the service panel controls.
    // Service controls MUST be given this suggestion to work, and multiple service controls can be appended.
    enum class Player : uint16_t
    {
        NONE = 0,
        ANY = 0,
        PLAYER_1 = (1 << 0),
        PLAYER_2 = (1 << 1),
        PLAYER_3 = (1 << 2),
        PLAYER_4 = (1 << 3),
        PLAYER_5 = (1 << 4),
        PLAYER_6 = (1 << 5),
        PLAYER_7 = (1 << 6),
        PLAYER_8 = (1 << 7),
        ARCADE_CABINET = (1 << 8),
        INVALID = 0xFFFF
    };

    // Used with the device descriptor to tell VPad which player the device is attached to (sequential indexing)
    enum class PlayerIndex : uint8_t
    {
        PLAYER_1 = 0,
        PLAYER_2 = 1,
        PLAYER_3 = 2,
        PLAYER_4 = 3,
        PLAYER_5 = 4,
        PLAYER_6 = 5,
        PLAYER_7 = 6,
        PLAYER_8 = 7,
        ARCADE_CABINET = 8,
        INVALID = 0xFF
    };

    inline constexpr Player &operator++(Player &a)
    {
        if (a == Player::PLAYER_8)
            a = Player::PLAYER_1;
        else
            a = static_cast<Player>(static_cast<uint16_t>(a) << 1);
        return a;
    }

    inline constexpr Player &operator--(Player &a)
    {
        if (a == Player::PLAYER_1)
            a = Player::PLAYER_8;
        else
            a = static_cast<Player>(static_cast<uint16_t>(a) >> 1);
        return a;
    }

    inline constexpr Player operator++(Player &a, int)
    {
        if (a == Player::PLAYER_8)
            a = Player::PLAYER_1;
        else
            a = static_cast<Player>(static_cast<uint16_t>(a) << 1);
        return a;
    }

    inline constexpr Player operator--(Player &a, int)
    {
        if (a == Player::PLAYER_1)
            a = Player::PLAYER_8;
        else
            a = static_cast<Player>(static_cast<uint16_t>(a) >> 1);
        return a;
    }

    inline constexpr Player operator|(Player a, Player b)
    {
        return static_cast<Player>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
    }

    inline constexpr Player operator&(Player a, Player b)
    {
        return static_cast<Player>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
    }

    inline constexpr Player &operator|=(Player &a, Player b)
    {
        return a = a | b;
    }

    inline constexpr Player &operator&=(Player &a, Player b)
    {
        return a = a & b;
    }

    inline constexpr Player operator~(Player a)
    {
        return static_cast<Player>(~static_cast<uint16_t>(a));
    }

    // Helper function to convert player index (0-8) to Player flag
    inline constexpr Player playerIndexToFlag(uint8_t idx)
    {
        constexpr Player flags[] = {
            Player::PLAYER_1,
            Player::PLAYER_2,
            Player::PLAYER_3,
            Player::PLAYER_4,
            Player::PLAYER_5,
            Player::PLAYER_6,
            Player::PLAYER_7,
            Player::PLAYER_8,
            Player::ARCADE_CABINET};
        return (idx < 9) ? flags[idx] : Player::NONE;
    }

    inline constexpr PlayerIndex playerToIndex(Player player)
    {
        switch (player)
        {
        default: return PlayerIndex::INVALID;
        case Player::PLAYER_1: return PlayerIndex::PLAYER_1;
        case Player::PLAYER_2: return PlayerIndex::PLAYER_2;
        case Player::PLAYER_3: return PlayerIndex::PLAYER_3;
        case Player::PLAYER_4: return PlayerIndex::PLAYER_4;
        case Player::PLAYER_5: return PlayerIndex::PLAYER_5;
        case Player::PLAYER_6: return PlayerIndex::PLAYER_6;
        case Player::PLAYER_7: return PlayerIndex::PLAYER_7;
        case Player::PLAYER_8: return PlayerIndex::PLAYER_8;
        case Player::ARCADE_CABINET: return PlayerIndex::ARCADE_CABINET;
        }
    }

    // Base class definition for input drivers. All input sources must have a driver associated with it.
    // This can be one driver for a static, single device (see Konami 573 JAMMA driver), or it can be
    // one driver for many types of devices (I.E, Windows HID input).
    class IInputDriver
    {
    protected:
        const char *_name = nullptr;

    public:
        IInputDriver() = default;
        virtual ~IInputDriver() = default;
        virtual int init() { return GV_OK; };
        virtual bool update() = 0;
        virtual bool reset() { return GV_OK; };
        virtual void shutdown() {};

        const char *getName() { return _name; }
    };

    // Input device controller descriptor - Used to define the behavoir of this input device.
    // Note that all controller data is expected to be in the defined GenV controller data format.
    struct IInputDevice
    {
        friend class VPad;
        friend class InputManager;

    private:
        bool m_inputsChanged = false;              // Internal use only: Notifies that this device's input state changed
        uint8_t m_listID = 0xFF;                   // Internal use only: Specifies the exact position of the device within the set player.
        PlayerIndex player = PlayerIndex::INVALID; // Internal use only: When registered, this is changed to be the current player the device is registered to.

    public:
        const char *name = szNullInputDevice;             // Friendly printable name
        util::Hash id = idNull;                           // Device ID - Usage is defined by the driver implementation
        util::Hash subid = idNull;                        // Device SubID - Usage is defined by the driver implementation
        InputDeviceType type = DEVICE_TYPE_NULL;          // Device Type - Tells the input mapper what class the device is
        InputDeviceSubType subtype = DEVICE_SUBTYPE_NULL; // Device Subtype - Not presently used
        uint8_t subBusID = 0;                             // Sub Bus ID - Usage is defined by the driver implementation
        struct
        {
            uint8_t numDigital = 0;  // Number of digital inputs
            uint8_t numAnalog = 0;   // Number of analog inputs
            uint8_t numRotary = 0;   // Number of relative inputs
            uint8_t numFeedback = 0; // Number of feedback/output drivers
        } capabilities;
        struct
        {
            uint32_t *digital = nullptr; // Pointer to the formatted digital button data (as bool, expects to be an array ptr above 32 inputs, active high)
            int16_t *analog = nullptr;   // Pointer to the analog position data (as int16, always expects to be an array ptr)
            int16_t *rotary = nullptr;   // Pointer to the relative position data (as int16, always expects to be an array ptr)
        } inputs;

        constexpr IInputDevice() {}

        constexpr IInputDevice(
            const char *name, util::Hash id, util::Hash subid,
            InputDeviceType type, InputDeviceSubType subtype, uint8_t subBusID,
            uint8_t numDigital, uint32_t *digitalPtr,
            uint8_t numAnalog = 0, int16_t *analogPtr = nullptr,
            uint8_t numRotary = 0, int16_t *rotaryPtr = nullptr,
            uint8_t numFeedback = 0)
        {
            this->name = name;
            this->id = id;
            this->subid = subid;
            this->type = type;
            this->subtype = subtype;
            this->subBusID = subBusID;
            this->capabilities.numDigital = numDigital;
            this->capabilities.numAnalog = numAnalog;
            this->capabilities.numRotary = numRotary;
            this->capabilities.numFeedback = numFeedback;
            this->inputs.digital = digitalPtr;
            this->inputs.analog = analogPtr;
            this->inputs.rotary = rotaryPtr;
        }

        constexpr IInputDevice(
            const char *name, util::Hash id, util::Hash subid,
            InputDeviceType type, uint8_t subBusID,
            uint8_t numDigital, uint32_t *digitalPtr,
            uint8_t numAnalog = 0, int16_t *analogPtr = nullptr,
            uint8_t numRotary = 0, int16_t *rotaryPtr = nullptr,
            uint8_t numFeedback = 0)
        {
            this->name = name;
            this->id = id;
            this->subid = subid;
            this->type = type;
            this->subBusID = subBusID;
            this->capabilities.numDigital = numDigital;
            this->capabilities.numAnalog = numAnalog;
            this->capabilities.numRotary = numRotary;
            this->capabilities.numFeedback = numFeedback;
            this->inputs.digital = digitalPtr;
            this->inputs.analog = analogPtr;
            this->inputs.rotary = rotaryPtr;
        }

        inline constexpr void setInputsChanged() { m_inputsChanged = true; }
    };

    class NullInput;
} // namespace Input