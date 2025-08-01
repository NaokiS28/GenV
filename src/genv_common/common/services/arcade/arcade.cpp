/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * arcade.cpp - Created on 01-08-2025
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

#include <assert.h>

#include "arcade.hpp"

namespace System
{
    uint8_t ArcadeSystem::setPhysicalPlayers(uint8_t players)
    {
        // TODO: Is this needing any extra logic?
        physicalPlayers = players;
    }

    uint8_t ArcadeSystem::setPhysicalCoinSlots(uint8_t slots)
    {
        if (playerCoins != nullptr)
            delete playerCoins;
        playerCoins = new Arcade::CoinData[4];
        assert(playerCoins != nullptr && "playerCoins is null!");
        if (!playerCoins)
            return 0;
        physicalCoinSlots = slots;
        return slots;
    }

    int8_t ArcadeSystem::coinsAvailable(int8_t slot)
    {
        if (!playerCoins || slot >= physicalCoinSlots)
            return false;

        int8_t coinsIn = 0;
        if (slot < 0)
        {
            for (int8_t p = 0; p < physicalCoinSlots; p++)
            {
                if (playerCoins[p].coinsIn > 0)
                    return p;
            }
            return -1;
        }
        else
        {
            return playerCoins[slot].coinsIn;
        }
    }

    uint8_t ArcadeSystem::coinsAvailable(uint8_t *array, uint8_t size)
    {
        if (!playerCoins || !array)
            return 0;

        if (size > physicalCoinSlots)
            size = physicalCoinSlots;

        uint8_t players = 0;
        for (int8_t p = 0; p < physicalCoinSlots; p++)
        {
            array[p] = playerCoins[p].coinsIn;
            players++;
        }
        return players;
    }

    uint8_t ArcadeSystem::addCoin(uint8_t slot, uint8_t amount)
    {
        if (slot >= physicalCoinSlots)
            return 0xFF;
        playerCoins[slot].coinsIn += amount;
        playerCoins[slot].counterTicks += amount;
        return slot;
    }

    uint8_t ArcadeSystem::addServiceCoin(uint8_t slot, uint8_t amount)
    {
        if (slot >= physicalCoinSlots)
            slot = 0;
        playerCoins[slot].coinsIn += amount;
        return slot;
    }

    uint8_t ArcadeSystem::increaseCoinCounter(uint8_t counter){
        if(counter >= physicalCoinSlots)
            return 0xFF;
        // TODO: Check IO service?
        return 0xFF;
    }

    uint8_t ArcadeSystem::setOutputs(uint8_t bank, uint8_t data)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
    uint8_t ArcadeSystem::setSingleOutput(uint8_t outputNumber, bool state)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
    uint8_t ArcadeSystem::writeAnalogueOut(uint8_t analogOutput, uint8_t state)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
    uint8_t ArcadeSystem::writeAnalogueOut16(uint8_t analogOutput, uint16_t state)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
    uint8_t ArcadeSystem::writeAnalogueOut32(uint8_t analogOutput, uint32_t state)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
}