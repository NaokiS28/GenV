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
#include "common/services/io/player.hpp"
#include "common/services/system/arcade/iface_arcade.hpp"

namespace System
{
    using namespace IO;

    CoinSlot BaseArcadeSystem::setPhysicalCoinSlots(CoinSlot slots)
    {
        if (playerCoins != nullptr) delete playerCoins;

        playerCoins = new ArcadeEnv::CoinData[4];
        assert(playerCoins != nullptr && "playerCoins is null!");
        if (!playerCoins)
            return CoinSlot::None;
        physicalCoinSlots = slots;
        return slots;
    }

    CoinCounter BaseArcadeSystem::getCoinCounterBuffer(CoinSlot slot)
    {
        if (slot >= physicalCoinSlots) return CoinCounter::None;

        CoinCounter counter = CoinCounter::None;
        /*
        for (CoinSlot c = CoinSlot::Coin1; c < physicalCoinSlots;)
        {
            counter |= (uint8_t)((playerCoins[c].counterTicks > 0) << c);
            if (playerCoins[c].counterTicks > 0)
                playerCoins[c].counterTicks--;
        }
        return counter;
        */

        // return playerCoins[slot].counterTicks--;
        return counter;
    }

    int8_t BaseArcadeSystem::coinsAvailable(Player slot)
    {
        // if (!playerCoins || slot >= physicalCoinSlots)
        //    return false;
        /*
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
            */
        return 0;
    }

    Player BaseArcadeSystem::coinsAvailable(uint8_t *array, Player players)
    {
        if (!playerCoins || !array)
            return Player::NONE;

        CoinSlot size = static_cast<CoinSlot>(playerToIndex(players));
        if (size > physicalCoinSlots)
            size = physicalCoinSlots;

        Player available = Player::NONE;
        for (CoinSlot p = CoinSlot::Coin1; p < physicalCoinSlots;) // p++
        {
            // array[p] = playerCoins[p].coinsIn;
            players++;
        }
        return available;
    }

    CoinSlot BaseArcadeSystem::addCoin(CoinSlot slot, uint8_t amount)
    {
        if (slot >= physicalCoinSlots)
            return CoinSlot::None;

        auto idx = coinSlotToIndex(slot);
        playerCoins[idx].coinsIn += amount;
        playerCoins[idx].counterTicks += amount;
        /*
        if (slot < _eeprom.coinSlots)
        {
            // Should always happen, but code safety ennit.
            _eeprom.totalCoins[slot]++;
        }
            */
        return slot;
    }

    CoinSlot BaseArcadeSystem::addServiceCoin(CoinSlot slot, uint8_t amount)
    {
        if (slot >= physicalCoinSlots) return CoinSlot::None;
        playerCoins[coinSlotToIndex(slot)].coinsIn += amount;
        return slot;
    }

    CoinCounter BaseArcadeSystem::increaseCoinCounter(CoinCounter counter)
    {
        // if (counter >= physicalCoinSlots)
        //    return CoinCounter::None;
        // TODO: Check IO service?
        return CoinCounter::None;
    }

    uint8_t BaseArcadeSystem::setOutputs(uint8_t bank, uint8_t data)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
    uint8_t BaseArcadeSystem::setSingleOutput(uint8_t outputNumber, bool state)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
    uint8_t BaseArcadeSystem::writeAnalogueOut(uint8_t analogOutput, uint8_t state)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
    uint8_t BaseArcadeSystem::writeAnalogueOut16(uint8_t analogOutput, uint16_t state)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
    uint8_t BaseArcadeSystem::writeAnalogueOut32(uint8_t analogOutput, uint32_t state)
    {
        // TODO: Check IO service?
        return 0xFF;
    }
} // namespace System