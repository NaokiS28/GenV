/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * vpad.cpp - Created on 09-02-2026
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

#include <stdint.h>
#include <stddef.h>

#include "common/logger/log.hpp"
#include "common/return_codes.hpp"
#include "iface_input.hpp"
#include "vpad.hpp"

#define VLOG(fmt, ...) LOG("vpad", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace Input
{

    int VPad::m_registerDevice(IInputDevice *device, Player player)
    {
        bool success = false;
        if (player == Player::INVALID)
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);

        m_devsChanged = true;
        if (player == Player::ANY)
        {
            // Handle PLAYER_ANY suggestion - Balance controllers across all available players
            uint8_t maxPlayers = static_cast<uint8_t>(m_playerMax);
            int bestIdx = -1;
            uint8_t minCount = maxDevicesPerPlayer + 1;

            for (int idx = 0; idx < maxPlayers; idx++)
            {
                if (m_playerDeviceList[idx].deviceCount < minCount)
                {
                    minCount = m_playerDeviceList[idx].deviceCount;
                    bestIdx = idx;
                }
            }

            // Assign to the player with fewest controllers
            if (bestIdx >= 0 && m_playerDeviceList[bestIdx].deviceCount < maxDevicesPerPlayer)
            {
                auto &pPads = m_playerDeviceList[bestIdx];

                for (auto &entry : pPads.devices)
                    if (entry == nullptr)
                    {
                        entry = device;
                        device->player = static_cast<PlayerIndex>(bestIdx);
                        if (pPads.deviceCount == 0) m_playersAvailable |= playerIndexToFlag(bestIdx);
                        pPads.deviceCount++;
                        success = true;
                        break;
                    }
            }
        }
        else if (player != Player::INVALID)
        {
            uint8_t u8_player = static_cast<uint8_t>(player);

            // Handle ARCADE_CABINET as a special case
            if (player == Player::ARCADE_CABINET)
            {
                int idx = 8;
                auto &pPads = m_playerDeviceList[idx];
                if (pPads.deviceCount >= maxDevicesPerPlayer)
                    return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_LIST_FULL);

                for (auto &entry : m_playerDeviceList[idx].devices)
                    if (entry == nullptr)
                    {
                        entry = device;
                        break;
                    }
                if (pPads.deviceCount == 0) m_playersAvailable |= playerIndexToFlag(idx);
                pPads.deviceCount++;
                device->player = PlayerIndex::ARCADE_CABINET;
                success = true;
            }
            else
            {
                // Check if any suggested player is beyond m_playerMax limit
                uint8_t maxPlayers = static_cast<uint8_t>(m_playerMax);
                bool allPlayersOutOfRange = true;

                for (int idx = 0; idx < maxPlayers; idx++)
                {
                    uint8_t mask = 1 << idx;
                    if (u8_player & mask)
                    {
                        allPlayersOutOfRange = false;
                        break;
                    }
                }

                // If all suggested players are beyond m_playerMax, exit early
                if (allPlayersOutOfRange)
                    return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);

                // Find the player with the fewest controllers among suggested players
                // This balances controllers across players
                int bestIdx = -1;
                uint8_t minCount = maxDevicesPerPlayer + 1;

                for (int idx = 0; idx < maxPlayers; idx++)
                {
                    uint8_t mask = 1 << idx;
                    if ((u8_player & mask) && m_playerDeviceList[idx].deviceCount < minCount)
                    {
                        minCount = m_playerDeviceList[idx].deviceCount;
                        bestIdx = idx;
                    }
                }

                // Assign to the player with fewest controllers (or first available)
                if (bestIdx >= 0 && m_playerDeviceList[bestIdx].deviceCount < maxDevicesPerPlayer)
                {
                    auto &pPads = m_playerDeviceList[bestIdx];

                    for (auto &entry : pPads.devices)
                        if (entry == nullptr)
                        {
                            entry = device;
                            device->player = static_cast<PlayerIndex>(bestIdx);
                            if (pPads.deviceCount == 0) m_playersAvailable |= playerIndexToFlag(bestIdx);
                            pPads.deviceCount++;
                            success = true;
                            break;
                        }
                }
            }
        }
        return (success ? GV_OK : GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_CREATE_FAILED));
    }

    int VPad::m_unregisterDevice(IInputDevice *device)
    {
        if (device->player == PlayerIndex::INVALID)
        {
            VLOG("Failed to unregister device: Device not registered.");
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);
        }

        m_devsChanged = true;
        uint8_t playerIndex = (device->player == PlayerIndex::ARCADE_CABINET) ? 8 : static_cast<uint8_t>(device->player);
        auto &pPads = m_playerDeviceList[playerIndex];

        if (pPads.deviceCount == 0)
        {
            VLOG("Failed to unregister device: Player has no pads assigned?.");
            device->player = PlayerIndex::INVALID;
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);
        }

        for (int idx = 0; idx < maxDevicesPerPlayer; idx++)
        {
            auto &thisDev = pPads.devices[idx];
            if (thisDev == device)
            {
                pPads.deviceCount--;
                if (pPads.deviceCount == 0) m_playersAvailable &= ~playerIndexToFlag(playerIndex);
                device->player = PlayerIndex::INVALID;
                thisDev = nullptr;
                return GV_OK;
            }
        }
        device->player = PlayerIndex::INVALID;
        VLOG("Failed to unregister device: Device not in player list.");
        return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_ITEM_NOT_FOUND);
    }

    int VPad::getPlayerDigitalInputs(uint32_t &inputs, const Player player, const uint8_t bank)
    {
        // Return collective OR of all inputs in this bank. Any button pressed will be active for the player.
        uint8_t playerIndex = 0;
        if (player == Player::ARCADE_CABINET)
            playerIndex = 8;
        else
            playerIndex = static_cast<uint8_t>(playerToIndex(player));

        uint32_t temp_inputs = 0;
        if (m_playerDeviceList[playerIndex].deviceCount == 0)
            return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_ITEM_NOT_FOUND);

        for (auto device : m_playerDeviceList[playerIndex].devices)
        {
            if (device == nullptr) continue;
            if (device->capabilities.numDigital == 0) continue;

            int numBanks = (device->capabilities.numDigital + 31) / 32;
            if (bank >= numBanks) continue;

            if (device->m_inputsChanged) device->m_inputsChanged = false;
            temp_inputs |= device->inputs.digital[bank];
        }
        inputs = temp_inputs;
        return GV_OK;
    }

    int VPad::getPlayerAnalogInputs(int16_t &analog, const Player player, const uint8_t bank)
    {
        // Return blended analog values of controllers for players.
        // If three controllers say 0, but one says 1024 then that value is returned.
        // If one is 1024 and another -1024 then return 0
        uint8_t playerIndex = 0;
        if (player == Player::ARCADE_CABINET)
            playerIndex = 8;
        else
            playerIndex = static_cast<uint8_t>(playerToIndex(player));

        int16_t temp_val = 0;
        if (m_playerDeviceList[playerIndex].deviceCount == 0)
            return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_ITEM_NOT_FOUND);

        for (auto device : m_playerDeviceList[playerIndex].devices)
        {
            if (device == nullptr) continue;
            if (bank > device->capabilities.numAnalog)
                continue;

            if (device->m_inputsChanged) device->m_inputsChanged = false;
            temp_val += device->inputs.analog[bank];
        }
        analog = temp_val;
        return GV_OK;
    }

    int VPad::getPlayerRotaryInputs(int16_t &rotary, const Player player, const uint8_t bank)
    {
        // Return blended rotary delta values of controllers for players.
        // If three controllers say 0, but one says 1024 then that value is returned.
        // If one is 1024 and another -1024 then return 0
        uint8_t playerIndex = 0;
        if (player == Player::ARCADE_CABINET)
            playerIndex = 8;
        else
            playerIndex = static_cast<uint8_t>(playerToIndex(player));

        int16_t temp_val = 0;
        if (m_playerDeviceList[playerIndex].deviceCount == 0)
            return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_ITEM_NOT_FOUND);

        for (auto device : m_playerDeviceList[playerIndex].devices)
        {
            if (device == nullptr) continue;
            if (bank > device->capabilities.numRotary)
                continue;

            if (device->m_inputsChanged) device->m_inputsChanged = false;
            temp_val += device->inputs.rotary[bank];
        }
        rotary = temp_val;
        return GV_OK;
    }

    int VPad::getPlayerDigitalCount(const Player player)
    {
        // Returns minimum digital inputs for a player (excluding 0)
        uint8_t playerIndex = 0;
        if (player == Player::ARCADE_CABINET)
            playerIndex = 8;
        else
            playerIndex = static_cast<uint8_t>(playerToIndex(player));

        if (m_playerDeviceList[playerIndex].deviceCount == 0) return 0;

        uint8_t currentMinimum = UINT8_MAX;
        for (auto device : m_playerDeviceList[playerIndex].devices)
        {
            if (device == nullptr) continue;
            if (device->capabilities.numDigital == 0) continue;

            if (device->capabilities.numDigital < currentMinimum)
                currentMinimum = device->capabilities.numDigital;
        }
        if (currentMinimum == UINT8_MAX) currentMinimum = 0; // No input devices have requested inputs
        return currentMinimum;
    }

    int VPad::getPlayerAnalogCount(const Player player)
    {
        // Returns minimum analog inputs for a player (excluding 0)
        uint8_t playerIndex = 0;
        if (player == Player::ARCADE_CABINET)
            playerIndex = 8;
        else
            playerIndex = static_cast<uint8_t>(playerToIndex(player));

        if (m_playerDeviceList[playerIndex].deviceCount == 0) return 0;

        uint8_t currentMinimum = UINT8_MAX;
        for (auto device : m_playerDeviceList[playerIndex].devices)
        {
            if (device == nullptr) continue;
            if (device->capabilities.numAnalog == 0) continue;

            if (device->capabilities.numAnalog < currentMinimum)
                currentMinimum = device->capabilities.numAnalog;
        }
        if (currentMinimum == UINT8_MAX) currentMinimum = 0; // No input devices have requested inputs
        return currentMinimum;
    }

    int VPad::getPlayerRotaryCount(const Player player)
    {
        // Returns minimum analog inputs for a player (excluding 0)
        uint8_t playerIndex = 0;
        if (player == Player::ARCADE_CABINET)
            playerIndex = 8;
        else
            playerIndex = static_cast<uint8_t>(playerToIndex(player));

        if (m_playerDeviceList[playerIndex].deviceCount == 0) return 0;

        uint8_t currentMinimum = UINT8_MAX;
        for (auto device : m_playerDeviceList[playerIndex].devices)
        {
            if (device == nullptr) continue;
            if (device->capabilities.numRotary == 0) continue;

            if (device->capabilities.numRotary < currentMinimum)
                currentMinimum = device->capabilities.numRotary;
        }
        if (currentMinimum == UINT8_MAX) currentMinimum = 0; // No input devices have requested inputs
        return currentMinimum;
    }

} // namespace Input