/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * vpad.hpp - Created on 09-02-2026
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

#include <stdint.h>

#include "common/services/io/iface_input.hpp"
#include "iface_input.hpp"

namespace Input
{
    constexpr const int maxDevicesPerPlayer = 4;

    struct PlayerDeviceList
    {
        IInputDevice *const *devices;
        uint8_t count;
    };

    class VPad
    {
        friend class InputManager;

    private:
        Player m_playersAvailable = Player::NONE;
        uint8_t m_playerMax       = 4; // Maximum number of players to assign controllers to (1-8)
        // 9th player is arcade cabinet service controls
        struct PhysicalPads
        {
            uint8_t deviceCount                        = 0;
            IInputDevice *devices[maxDevicesPerPlayer] = {nullptr};
        } m_playerDeviceList[9];

        bool m_devsChanged   = false;
        bool m_inputsChanged = false;

        int m_registerDevice(IInputDevice *device, Player player);
        int m_unregisterDevice(IInputDevice *device);

    public:
        inline bool devicesChanged()
        {
            bool d        = m_devsChanged;
            m_devsChanged = false;
            return d;
        }

        // TODO: This should update when any inputs on any controller change.
        // TODO: Make test to test specific player's inputs changed - usefull?
        inline bool inputsChanged()
        {
            bool d          = m_inputsChanged;
            m_inputsChanged = false;
            return d;
        }

        inline int playerCount() { return __builtin_popcount(static_cast<int>(m_playersAvailable)); }
        inline Player getPlayersAvailable() { return m_playersAvailable; }
        inline bool isPlayerAvailable(Player player) { return (m_playersAvailable & player) == player; }

        inline PlayerDeviceList getPlayerDevices(Player player)
        {
            auto p  = playerToIndex(player);
            auto &d = m_playerDeviceList[static_cast<uint8_t>(p)];
            return {d.devices, d.deviceCount};
        }

        inline void setMaximumPlayers(uint8_t players) { m_playerMax = players; }
        inline uint8_t getMaximumPlayers() { return m_playerMax; }

        int getPlayerDigitalInputs(uint32_t &inputs, const Player player, const uint8_t bank = 0);
        int getPlayerAnalogInputs(int16_t &analog, const Player player, const uint8_t bank = 0);
        int getPlayerRotaryInputs(int16_t &rotary, const Player player, const uint8_t bank = 0);
        int getPlayerDigitalCount(Player player);
        int getPlayerAnalogCount(Player player);
        int getPlayerRotaryCount(Player player);
    };
} // namespace Input