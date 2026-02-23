/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * playerman.hpp - Created on 22-02-2026
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
#include "common/services/io/iface_output.hpp"

namespace IO
{
    constexpr const int maxInputDevicesPerPlayer  = 4;
    constexpr const int maxOutputDevicesPerPlayer = 4;

    // Snapshot of a player's assigned input devices, returned by getPlayerInputDevices().
    struct PlayerInputDeviceList
    {
        Input::IInputDevice *const *devices;
        uint8_t count;
    };

    // Snapshot of a player's assigned output devices, returned by getPlayerOutputDevices().
    struct PlayerOutputDeviceList
    {
        Output::IOutputDevice *const *devices;
        uint8_t count;
    };

    // Central player registry. Owns the static Player[9] array (players 0-7 + ARCADE_CABINET).
    // InputManager and OutputManager both register devices here.
    // VPad and VOutput read/write through this class.
    class PlayerManager
    {
    private:
        Player m_playersAvailable = Player::NONE;
        uint8_t m_playerMax       = 4; // Maximum number of player slots to assign to (1-8)

        int m_inputDeviceCount  = 0;
        int m_outputDeviceCount = 0;

        struct PlayerSlot
        {
            uint8_t inputCount                                        = 0;
            uint8_t outputCount                                       = 0;
            Input::IInputDevice *inputs[maxInputDevicesPerPlayer]     = {nullptr};
            Output::IOutputDevice *outputs[maxOutputDevicesPerPlayer] = {nullptr};
        } m_players[9]; // 0-7 = players, 8 = ARCADE_CABINET

        bool m_devsChanged = false;

    public:
        // Returns true and clears the flag if any device assignment changed since last call.
        inline bool devicesChanged()
        {
            bool d        = m_devsChanged;
            m_devsChanged = false;
            return d;
        }

        inline int inputDeviceCount() { return m_inputDeviceCount; }
        inline int outputDeviceCount() { return m_outputDeviceCount; }
        inline int playerCount() { return __builtin_popcount(static_cast<int>(m_playersAvailable)); }
        inline Player getPlayersAvailable() { return m_playersAvailable; }
        inline bool isPlayerAvailable(Player player) { return (m_playersAvailable & player) == player; }

        inline void setMaximumPlayers(uint8_t players) { m_playerMax = players; }
        inline uint8_t getMaximumPlayers() { return m_playerMax; }

        // Device registration — called by drivers during their init()
        int attachInputDevice(Input::IInputDevice *device, Player player);
        int detachInputDevice(Input::IInputDevice *device);
        int attachOutputDevice(Output::IOutputDevice *device, Player player);
        int detachOutputDevice(Output::IOutputDevice *device);

        PlayerInputDeviceList getPlayerInputDevices(Player player);
        PlayerOutputDeviceList getPlayerOutputDevices(Player player);

        // Input read interface - aggregates across all input devices assigned to the player
        int getPlayerDigitalInputs(uint32_t &inputs, Player player, uint8_t bank = 0);
        int getPlayerAnalogInputs(int16_t &analog, Player player, uint8_t bank = 0);
        int getPlayerRotaryInputs(int16_t &rotary, Player player, uint8_t bank = 0);
        int getPlayerDigitalCount(Player player);
        int getPlayerAnalogCount(Player player);
        int getPlayerRotaryCount(Player player);
    };

    // Lightweight view into one player slot. Holds a pointer to PlayerManager and a Player
    // enum value — cheap to construct and copy. All reads return 0/false if the slot has no
    // devices, so game code does not need to guard every call.
    class PlayerView
    {
        PlayerManager *m_manager;
        Player m_player;

    public:
        constexpr PlayerView(PlayerManager *mgr, Player player)
            : m_manager(mgr), m_player(player) {}

        inline bool isAvailable() const
        {
            return m_manager && m_manager->isPlayerAvailable(m_player);
        }

        inline uint32_t getDigital(uint8_t bank = 0) const
        {
            uint32_t v = 0;
            if (m_manager) m_manager->getPlayerDigitalInputs(v, m_player, bank);
            return v;
        }

        inline int16_t getAnalog(uint8_t channel = 0) const
        {
            int16_t v = 0;
            if (m_manager) m_manager->getPlayerAnalogInputs(v, m_player, channel);
            return v;
        }

        inline int16_t getRotary(uint8_t channel = 0) const
        {
            int16_t v = 0;
            if (m_manager) m_manager->getPlayerRotaryInputs(v, m_player, channel);
            return v;
        }

        inline int digitalCount() const
        {
            return m_manager ? m_manager->getPlayerDigitalCount(m_player) : 0;
        }

        inline int analogCount() const
        {
            return m_manager ? m_manager->getPlayerAnalogCount(m_player) : 0;
        }

        inline int rotaryCount() const
        {
            return m_manager ? m_manager->getPlayerRotaryCount(m_player) : 0;
        }

        inline PlayerInputDeviceList inputDevices() const
        {
            if (m_manager) return m_manager->getPlayerInputDevices(m_player);
            return {nullptr, 0};
        }

        inline PlayerOutputDeviceList outputDevices() const
        {
            if (m_manager) return m_manager->getPlayerOutputDevices(m_player);
            return {nullptr, 0};
        }
    };

    // Free functions — game-facing API. PlayerManager is guaranteed valid post-startup.
    // IO::player(PLAYER_1).getDigital() etc.
    PlayerManager *playerManager();
    PlayerView player(Player p);
    PlayerView arcade(); // alias for the ARCADE_CABINET slot

} // namespace IO
