/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * playerman.cpp - Created on 22-02-2026
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
#include "playerman.hpp"

#define PMLOG(fmt, ...) LOG("playerman", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace IO
{
    // -------------------------------------------------------------------------
    // Input device registration (called by InputManager)
    // -------------------------------------------------------------------------

    int PlayerManager::m_registerInputDevice(Input::IInputDevice *device, Player player)
    {
        bool success = false;
        if (player == Player::INVALID)
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);

        m_devsChanged = true;

        if (player == Player::ANY)
        {
            // Balance input devices across available player slots
            uint8_t maxPlayers = static_cast<uint8_t>(m_playerMax);
            int bestIdx        = -1;
            uint8_t minCount   = maxInputDevicesPerPlayer + 1;

            for (int idx = 0; idx < maxPlayers; idx++)
            {
                if (m_players[idx].inputCount < minCount)
                {
                    minCount = m_players[idx].inputCount;
                    bestIdx  = idx;
                }
            }

            if (bestIdx >= 0 && m_players[bestIdx].inputCount < maxInputDevicesPerPlayer)
            {
                auto &slot = m_players[bestIdx];
                for (auto &entry : slot.inputs)
                    if (entry == nullptr)
                    {
                        entry         = device;
                        device->player = static_cast<PlayerIndex>(bestIdx);
                        if (slot.inputCount == 0) m_playersAvailable |= playerIndexToFlag(bestIdx);
                        slot.inputCount++;
                        success = true;
                        break;
                    }
            }
        }
        else if (player == Player::ARCADE_CABINET)
        {
            int idx    = 8;
            auto &slot = m_players[idx];
            if (slot.inputCount >= maxInputDevicesPerPlayer)
                return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_LIST_FULL);

            for (auto &entry : slot.inputs)
                if (entry == nullptr)
                {
                    entry = device;
                    break;
                }
            if (slot.inputCount == 0) m_playersAvailable |= playerIndexToFlag(idx);
            slot.inputCount++;
            device->player = PlayerIndex::ARCADE_CABINET;
            success        = true;
        }
        else
        {
            uint8_t u8_player    = static_cast<uint8_t>(player);
            uint8_t maxPlayers   = static_cast<uint8_t>(m_playerMax);
            bool allOutOfRange   = true;

            for (int idx = 0; idx < maxPlayers; idx++)
            {
                if (u8_player & (1 << idx))
                {
                    allOutOfRange = false;
                    break;
                }
            }

            if (allOutOfRange)
                return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);

            int bestIdx      = -1;
            uint8_t minCount = maxInputDevicesPerPlayer + 1;

            for (int idx = 0; idx < maxPlayers; idx++)
            {
                if ((u8_player & (1 << idx)) && m_players[idx].inputCount < minCount)
                {
                    minCount = m_players[idx].inputCount;
                    bestIdx  = idx;
                }
            }

            if (bestIdx >= 0 && m_players[bestIdx].inputCount < maxInputDevicesPerPlayer)
            {
                auto &slot = m_players[bestIdx];
                for (auto &entry : slot.inputs)
                    if (entry == nullptr)
                    {
                        entry         = device;
                        device->player = static_cast<PlayerIndex>(bestIdx);
                        if (slot.inputCount == 0) m_playersAvailable |= playerIndexToFlag(bestIdx);
                        slot.inputCount++;
                        success = true;
                        break;
                    }
            }
        }

        return (success ? GV_OK : GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_CREATE_FAILED));
    }

    int PlayerManager::m_unregisterInputDevice(Input::IInputDevice *device)
    {
        if (device->player == PlayerIndex::INVALID)
        {
            PMLOG("Failed to unregister input device: not registered.");
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);
        }

        m_devsChanged = true;
        uint8_t playerIndex = (device->player == PlayerIndex::ARCADE_CABINET)
                                  ? 8
                                  : static_cast<uint8_t>(device->player);
        auto &slot = m_players[playerIndex];

        if (slot.inputCount == 0)
        {
            PMLOG("Failed to unregister input device: player slot is empty.");
            device->player = PlayerIndex::INVALID;
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);
        }

        for (auto &entry : slot.inputs)
        {
            if (entry == device)
            {
                slot.inputCount--;
                if (slot.inputCount == 0) m_playersAvailable &= ~playerIndexToFlag(playerIndex);
                device->player = PlayerIndex::INVALID;
                entry          = nullptr;
                return GV_OK;
            }
        }

        device->player = PlayerIndex::INVALID;
        PMLOG("Failed to unregister input device: not found in player slot.");
        return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_ITEM_NOT_FOUND);
    }

    // -------------------------------------------------------------------------
    // Output device registration (called by OutputManager)
    // -------------------------------------------------------------------------

    int PlayerManager::m_registerOutputDevice(Output::IOutputDevice *device, Player player)
    {
        bool success = false;
        if (player == Player::INVALID)
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);

        m_devsChanged = true;

        if (player == Player::ANY)
        {
            uint8_t maxPlayers = static_cast<uint8_t>(m_playerMax);
            int bestIdx        = -1;
            uint8_t minCount   = maxOutputDevicesPerPlayer + 1;

            for (int idx = 0; idx < maxPlayers; idx++)
            {
                if (m_players[idx].outputCount < minCount)
                {
                    minCount = m_players[idx].outputCount;
                    bestIdx  = idx;
                }
            }

            if (bestIdx >= 0 && m_players[bestIdx].outputCount < maxOutputDevicesPerPlayer)
            {
                auto &slot = m_players[bestIdx];
                for (auto &entry : slot.outputs)
                    if (entry == nullptr)
                    {
                        entry          = device;
                        device->player = static_cast<PlayerIndex>(bestIdx);
                        if (slot.outputCount == 0) m_playersAvailable |= playerIndexToFlag(bestIdx);
                        slot.outputCount++;
                        success = true;
                        break;
                    }
            }
        }
        else if (player == Player::ARCADE_CABINET)
        {
            int idx    = 8;
            auto &slot = m_players[idx];
            if (slot.outputCount >= maxOutputDevicesPerPlayer)
                return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_LIST_FULL);

            for (auto &entry : slot.outputs)
                if (entry == nullptr)
                {
                    entry = device;
                    break;
                }
            if (slot.outputCount == 0) m_playersAvailable |= playerIndexToFlag(idx);
            slot.outputCount++;
            device->player = PlayerIndex::ARCADE_CABINET;
            success        = true;
        }
        else
        {
            uint8_t u8_player  = static_cast<uint8_t>(player);
            uint8_t maxPlayers = static_cast<uint8_t>(m_playerMax);
            bool allOutOfRange = true;

            for (int idx = 0; idx < maxPlayers; idx++)
            {
                if (u8_player & (1 << idx))
                {
                    allOutOfRange = false;
                    break;
                }
            }

            if (allOutOfRange)
                return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);

            int bestIdx      = -1;
            uint8_t minCount = maxOutputDevicesPerPlayer + 1;

            for (int idx = 0; idx < maxPlayers; idx++)
            {
                if ((u8_player & (1 << idx)) && m_players[idx].outputCount < minCount)
                {
                    minCount = m_players[idx].outputCount;
                    bestIdx  = idx;
                }
            }

            if (bestIdx >= 0 && m_players[bestIdx].outputCount < maxOutputDevicesPerPlayer)
            {
                auto &slot = m_players[bestIdx];
                for (auto &entry : slot.outputs)
                    if (entry == nullptr)
                    {
                        entry          = device;
                        device->player = static_cast<PlayerIndex>(bestIdx);
                        if (slot.outputCount == 0) m_playersAvailable |= playerIndexToFlag(bestIdx);
                        slot.outputCount++;
                        success = true;
                        break;
                    }
            }
        }

        return (success ? GV_OK : GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_CREATE_FAILED));
    }

    int PlayerManager::m_unregisterOutputDevice(Output::IOutputDevice *device)
    {
        if (device->player == PlayerIndex::INVALID)
        {
            PMLOG("Failed to unregister output device: not registered.");
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);
        }

        m_devsChanged = true;
        uint8_t playerIndex = (device->player == PlayerIndex::ARCADE_CABINET)
                                  ? 8
                                  : static_cast<uint8_t>(device->player);
        auto &slot = m_players[playerIndex];

        if (slot.outputCount == 0)
        {
            PMLOG("Failed to unregister output device: player slot is empty.");
            device->player = PlayerIndex::INVALID;
            return GV_ERROR(GV_SERVICE_GENERIC, GV_CATEGORY_GENERIC, GV_ERR_INVALID_PARAM);
        }

        for (auto &entry : slot.outputs)
        {
            if (entry == device)
            {
                slot.outputCount--;
                if (slot.outputCount == 0 && slot.inputCount == 0)
                    m_playersAvailable &= ~playerIndexToFlag(playerIndex);
                device->player = PlayerIndex::INVALID;
                entry          = nullptr;
                return GV_OK;
            }
        }

        device->player = PlayerIndex::INVALID;
        PMLOG("Failed to unregister output device: not found in player slot.");
        return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_ITEM_NOT_FOUND);
    }

    // -------------------------------------------------------------------------
    // Device list queries
    // -------------------------------------------------------------------------

    PlayerInputDeviceList PlayerManager::getPlayerInputDevices(Player player)
    {
        auto p     = playerToIndex(player);
        auto &slot = m_players[static_cast<uint8_t>(p)];
        return {slot.inputs, slot.inputCount};
    }

    PlayerOutputDeviceList PlayerManager::getPlayerOutputDevices(Player player)
    {
        auto p     = playerToIndex(player);
        auto &slot = m_players[static_cast<uint8_t>(p)];
        return {slot.outputs, slot.outputCount};
    }

    // -------------------------------------------------------------------------
    // Input read interface
    // -------------------------------------------------------------------------

    int PlayerManager::getPlayerDigitalInputs(uint32_t &inputs, const Player player, const uint8_t bank)
    {
        uint8_t playerIndex = (player == Player::ARCADE_CABINET)
                                  ? 8
                                  : static_cast<uint8_t>(playerToIndex(player));

        auto &slot = m_players[playerIndex];
        if (slot.inputCount == 0)
            return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_ITEM_NOT_FOUND);

        uint32_t temp = 0;
        for (auto device : slot.inputs)
        {
            if (device == nullptr) continue;
            if (device->capabilities.numDigital == 0) continue;

            int numBanks = (device->capabilities.numDigital + 31) / 32;
            if (bank >= numBanks) continue;

            if (device->m_inputsChanged) device->m_inputsChanged = false;
            temp |= device->inputs.digital[bank];
        }
        inputs = temp;
        return GV_OK;
    }

    int PlayerManager::getPlayerAnalogInputs(int16_t &analog, const Player player, const uint8_t bank)
    {
        uint8_t playerIndex = (player == Player::ARCADE_CABINET)
                                  ? 8
                                  : static_cast<uint8_t>(playerToIndex(player));

        auto &slot = m_players[playerIndex];
        if (slot.inputCount == 0)
            return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_ITEM_NOT_FOUND);

        int16_t temp = 0;
        for (auto device : slot.inputs)
        {
            if (device == nullptr) continue;
            if (bank >= device->capabilities.numAnalog) continue;

            if (device->m_inputsChanged) device->m_inputsChanged = false;
            temp += device->inputs.analog[bank];
        }
        analog = temp;
        return GV_OK;
    }

    int PlayerManager::getPlayerRotaryInputs(int16_t &rotary, const Player player, const uint8_t bank)
    {
        uint8_t playerIndex = (player == Player::ARCADE_CABINET)
                                  ? 8
                                  : static_cast<uint8_t>(playerToIndex(player));

        auto &slot = m_players[playerIndex];
        if (slot.inputCount == 0)
            return GV_ERROR(GV_SERVICE_INPUT, GV_CATEGORY_GENERIC, GV_ERR_ITEM_NOT_FOUND);

        int16_t temp = 0;
        for (auto device : slot.inputs)
        {
            if (device == nullptr) continue;
            if (bank >= device->capabilities.numRotary) continue;

            if (device->m_inputsChanged) device->m_inputsChanged = false;
            temp += device->inputs.rotary[bank];
        }
        rotary = temp;
        return GV_OK;
    }

    int PlayerManager::getPlayerDigitalCount(const Player player)
    {
        uint8_t playerIndex = (player == Player::ARCADE_CABINET)
                                  ? 8
                                  : static_cast<uint8_t>(playerToIndex(player));

        auto &slot = m_players[playerIndex];
        if (slot.inputCount == 0) return 0;

        uint8_t minimum = UINT8_MAX;
        for (auto device : slot.inputs)
        {
            if (device == nullptr) continue;
            if (device->capabilities.numDigital == 0) continue;
            if (device->capabilities.numDigital < minimum)
                minimum = device->capabilities.numDigital;
        }
        return (minimum == UINT8_MAX) ? 0 : minimum;
    }

    int PlayerManager::getPlayerAnalogCount(const Player player)
    {
        uint8_t playerIndex = (player == Player::ARCADE_CABINET)
                                  ? 8
                                  : static_cast<uint8_t>(playerToIndex(player));

        auto &slot = m_players[playerIndex];
        if (slot.inputCount == 0) return 0;

        uint8_t minimum = UINT8_MAX;
        for (auto device : slot.inputs)
        {
            if (device == nullptr) continue;
            if (device->capabilities.numAnalog == 0) continue;
            if (device->capabilities.numAnalog < minimum)
                minimum = device->capabilities.numAnalog;
        }
        return (minimum == UINT8_MAX) ? 0 : minimum;
    }

    int PlayerManager::getPlayerRotaryCount(const Player player)
    {
        uint8_t playerIndex = (player == Player::ARCADE_CABINET)
                                  ? 8
                                  : static_cast<uint8_t>(playerToIndex(player));

        auto &slot = m_players[playerIndex];
        if (slot.inputCount == 0) return 0;

        uint8_t minimum = UINT8_MAX;
        for (auto device : slot.inputs)
        {
            if (device == nullptr) continue;
            if (device->capabilities.numRotary == 0) continue;
            if (device->capabilities.numRotary < minimum)
                minimum = device->capabilities.numRotary;
        }
        return (minimum == UINT8_MAX) ? 0 : minimum;
    }

} // namespace IO
