/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * nvram.cpp - Created on 05-08-2025
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

#include "nvram.hpp"

PlayerAnalytics::PlayerAnalytics()
{
    header.blockLength = GENV_NVRAM_AnalyticsLength;
}

bool PlayerAnalytics::valid()
{
    if (strcmp(this->header.magic, GENV_NVRAM_AnalyticsMagic) != 0)
        return false;
    uint16_t sum = util::checksum(&this[GENV_NVRAM_AnalyticsStart], GENV_NVRAM_AnalyticsLength);
    if (sum != header.checksum)
        return false;
    return true;
}

void PlayerAnalytics::recalcSum()
{
    this->header.checksum = util::checksum(&this[GENV_NVRAM_AnalyticsStart], GENV_NVRAM_AnalyticsLength);
}

PlayerHighscore::PlayerHighscore()
{
    header.blockLength = GENV_NVRAM_HighscoreLength;
}

int8_t PlayerHighscore::isHighscore(uint32_t score)
{
    uint8_t pos = -1;
    for (uint8_t idx = 9; idx > 0; idx--)
    {
        if (score >= this->table[idx].score)
        {
            pos = idx;
        }
        else
        {
            break;
        }
    }
    return pos;
}

bool PlayerHighscore::addScore(const HighScoreAlpha *name, uint32_t score, uint16_t level)
{
    uint8_t pos = 10;
    bool added = false;
    for (uint8_t idx = 9; idx > 0; idx--)
    {
        if (score >= this->table[idx].score)
        {
            pos = idx;
        }
        else
        {
            break;
        }
    }
    if (pos < 10)
    {
        memmove(&this->table[pos], &this->table[pos + 1], sizeof(PlayerScore) * (9 - pos));
        memcpy(&this->table[pos].name, name, 4);
        this->table[pos].name[4] = '\0';
        this->table[pos].score = score;
        this->table[pos].level = level;
        added = true;
    }
    return added;
}

bool PlayerHighscore::valid()
{
    if (strcmp(this->header.magic, GENV_NVRAM_HighscoreMagic) != 0)
        return false;
    uint16_t sum = util::checksum(&this[GENV_NVRAM_HighscoreStart], GENV_NVRAM_HighscoreLength);
    if (sum != header.checksum)
        return false;
    return true;
}

void PlayerHighscore::recalcSum()
{
    this->header.checksum = util::checksum(&this[GENV_NVRAM_HighscoreStart], GENV_NVRAM_HighscoreLength);
}

NVRAM::NVRAM()
{
    header.blockLength = GENV_NVRAM_ErrorCode_Length;
}

bool NVRAM::valid(const char *gameID)
{
    if (strcmp(this->header.magic, GENV_NVRAM_HeaderMagic) != 0 || strcmp(this->header.gameID, gameID) != 0)
        return false;
    uint16_t sum = util::checksum(&this[GENV_NVRAM_HeaderStart], GENV_NVRAM_HeaderLength);
    if (sum != header.checksum)
        return false;
    return true;
}

void NVRAM::addErrorCode(uint16_t code, uint32_t time, uint16_t param1, uint16_t param2)
{
    memmove(&errorCodes[1], &errorCodes[0], sizeof(ErrorCode) * (GENV_NVRAM_ErrorCode_Length - 1));
    errorCodes[0].time = time;
    errorCodes[0].code = code;
    errorCodes[0].param[0] = param1;
    errorCodes[0].param[1] = param2;
}

void NVRAM::recalcSum()
{
    this->header.checksum = util::checksum(&this[GENV_NVRAM_HeaderStart], GENV_NVRAM_HeaderLength);
}