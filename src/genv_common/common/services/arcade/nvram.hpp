/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * nvram.hpp - Created on 31-07-2025
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
#include <string.h>

#include "errorcodes.hpp"
#include "common/util/misc.hpp"

using HighScoreAlpha = char;
constexpr const HighScoreAlpha GENV_Highscore_Alphabet[] = {
    ' ', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q',
    'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8',
    '9', '.', '-', '=', '@', '$', '&', '*', '(',
    ')', '/', '\\', '\e'};

struct GameSettings;

struct PlayerAnalytics
{
    struct Header
    {
        char magic[5] = {'\0'};
        uint16_t checksum = 0;
        uint32_t blockLength = 0;
    } header;

    uint32_t totalGames = 0;
    uint32_t minutesPlayed = 0;
    uint32_t averageSeconds = 0;
    uint32_t lastLevel = 0;

    PlayerAnalytics();

    bool valid();
    inline void recalcSum();
};

constexpr const char GENV_NVRAM_AnalyticsMagic[5] = "GPBK";
constexpr const uint16_t GENV_NVRAM_AnalyticsStart = (sizeof(PlayerAnalytics::header));
constexpr const uint16_t GENV_NVRAM_AnalyticsLength = (sizeof(PlayerAnalytics) - (sizeof(PlayerAnalytics::header)));

struct PlayerHighscore
{
    struct Header
    {
        char magic[5] = {'\0'};   // Block magic number
        uint16_t checksum = 0;    // Block checksum
        uint32_t blockLength = 0; // Block length
    } header;

    struct PlayerScore
    {
        char name[5] = {' ', ' ', ' ', ' ', '\0'}; // Players initials (is always null terminated)
        uint32_t score = 0;                        // Highscore
        uint16_t level = 0;                        // Level achieved on
    } table[10];                                   // Highscore table for 10 players

    PlayerHighscore();
    int8_t isHighscore(uint32_t score);
    bool addScore(const HighScoreAlpha *name, uint32_t score, uint16_t level = 0);
    bool valid();
    void recalcSum();
};

constexpr const char GENV_NVRAM_HighscoreMagic[5] = "GPHS";
constexpr const uint16_t GENV_NVRAM_HighscoreStart = (sizeof(PlayerHighscore::header));
constexpr const uint16_t GENV_NVRAM_HighscoreLength = (sizeof(PlayerHighscore) - (sizeof(PlayerHighscore::header)));

// GenV Arcade NVRAM Save data
// This struct should be used for storing settings and data on arcade games.
struct NVRAM
{
    // Header
    struct Header
    {
        char magic[5] = {'\0'};                         // Game engine header magic
        char gameID[9] = {'\0'};                        // Game ID code
        uint16_t checksum = 0;                          // Checksum of this chunk
        uint16_t blockLength = 0; // Length of this block
    } header;

    NVRAM();

    // Player statistics
    uint8_t playerCount = 0;                     // How many players this game uses. Used for analytics array and highscore table
    PlayerAnalytics *playerAnalytics = nullptr;  // Location of the player analytics table
    PlayerHighscore *playerScoreTable = nullptr; // Location of the player highscore table

    // Coin statistics
    uint8_t coinSlots = 0;        // Physical coin slots present. Used for various coin related checks.
    uint8_t coinsToStart = 0;     // Coins required to start. The engine won't directly use this, but its a fairly usual requirement that makes it worth standardising.
    uint32_t totalCoins[6] = {0}; // 4 coin slots + two credit sources. Surely enough for any occasion.

    // Error logging
    uint8_t errorCount = 0; // Count of how many errors since last reset
    struct ErrorCodeTable
    {
        uint32_t time = 0;                                    // Time when the error occured
        ErrorCode code = 0;                                   // The code of the error
        uint16_t param[2] = {0, 0};                           // (optional) Parameters for the error code
    } errorCodes[20];                                         // Error code table
    uint32_t lastResetTime = 0;                               // Time when the NVRAM was last cleared
    ShutdownCode lastShutdownReason = GENV_SHUTDOWN_GRACEFUL; // What caused the last software restart (not always reliable)
    GameSettings *gameConfig = nullptr;                       // Location of the game specific configuration file

    // Sets the game ID to the one supplied. Game ID must be a max of 8 characters.
    //  The 9th character and beyond WILL be truncated.
    inline void setGameID(const char *gameID)
    {
        strncpy(this->header.gameID, gameID, 10);
        this->header.gameID[9] = '\0';
    }

    // Checks to see if the NVRAM header is valid. Must be given the 8 character game ID.
    //  If the header is NOT valid, the program should assume the entire NVRAM is invalid.
    //  Note: only the first 8 characters of gameID are checked, the remaining characters
    //        are igored.
    bool valid(const char *gameID);

    // Stores an error code in the NVRAM error logging table.
    // Code: The error code, either a GenV default code or a custom game code.
    // Time: The time at which point the error occured
    // Param: (Optional) The parameter associated with the error code (I.E, the coin mech for COIN_MECH_JAM)
    void addErrorCode(uint16_t code, uint32_t time, uint16_t param1 = 0, uint16_t param2 = 0);

    // Recalculates the NVRAM header checksum. Should be called after any modification.
    // Note: This does not recalculate the checksum for the player statistics or the game config
    //        tables.
    void recalcSum();
};

constexpr const char GENV_NVRAM_HeaderMagic[5] = "GENV";
constexpr const uint8_t GENV_NVRAM_ErrorCode_Length = 20;
constexpr const uint16_t GENV_NVRAM_HeaderStart = (sizeof(NVRAM::header));
constexpr const uint16_t GENV_NVRAM_HeaderLength = (sizeof(NVRAM) - sizeof(NVRAM::header));