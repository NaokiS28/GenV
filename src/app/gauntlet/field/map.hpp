/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * map.hpp - Created on 25-06-2025
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

#include "mapmeta.h"
#include "floortile.hpp"

enum class Gaunt_Objects
{
    Air = 0,
    Wall,
    Exit,
    Door,
    Player_Spawn,
    Monster_Generator,
    Monster, // Pre-baked monsters that spawn on level load
    Treasure,
    Key,
    Food,
    Potion,
    Teleporter,
    Invalid
};

struct Gaunt_Item
{
    Gaunt_Objects obj = Gaunt_Objects::Air;
    uint8_t param = 0;
};

// A map
class Gaunt_Map
{
private:
    // 2D uint8_t array of randomised tile columns and shadow data for this level.
    uint8_t **floorMap = nullptr;

    // 2D array of every non-floor object on this level, excluding players.
    Gaunt_Item objectMap[Gaunt_MapSizeH][Gaunt_MapSizeH] = {
        // Walls, food, generators, etc.
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Wall, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}},
        {{Gaunt_Objects::Wall, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}, {Gaunt_Objects::Air, 0}},
    };

public:
    // Used for flooring, gets how many walls are to the left and below of this tile
    // Return format is as follows:
    //  1 = Wall to the left, 2 = 2 walls to the left
    //  4 = Wall below, 8 = 2 Walls below.. etc
    //  F = The given x,y is itself a wall and so no shadow or tile should be drawn.
    uint8_t shadowCheck(uint8_t x, uint8_t y)
    {
        // Boundry clamping
        x %= (Gaunt_MapSizeW + 1);
        y %= (Gaunt_MapSizeH + 1);
        if (!isWall(x, y))
        {
            // TODO: This whole system can clearly be more efficient.
            uint8_t wc = ((isWall(x - 1, y + 1) << 2) | (isWall(x, y + 1) << 1) | isWall(x - 1, y));
            if (wc == 0)
                return Gaunt_FloorTile::Shadow::None;

            switch (wc)
            {
            default:
                return Gaunt_FloorTile::Shadow::None;
            case 0b001: // Wall to left but nowhere else
                return Gaunt_FloorTile::Shadow::Left_Diag;
            case 0b010: // Wall below but nowhere else
                return Gaunt_FloorTile::Shadow::Bottom_Diag;
            case 0b100: // Wall to BL corner but nowhere else
                return Gaunt_FloorTile::Shadow::Lower_Left_Corner;
            case 0b101: // Wall to BL corner and to left
                return Gaunt_FloorTile::Shadow::Left_Solid;
            case 0b110:
                return Gaunt_FloorTile::Shadow::Bottom_Solid;
            case 0b111:
            case 0b011:
                return Gaunt_FloorTile::Shadow::Left_Bottom_Solid;
            }
        }
        else
        {
            return Gaunt_FloorTile::Shadow::Hidden_Floor;
        }
    }

    // Returns whether any walls surround this tile in clockwise order, starting from
    //   tile to top left of the selected tile.
    //   Remember Naoki, VSCode shows variables in decimal, not hex.
    uint8_t wallCheck(uint8_t x, uint8_t y)
    {
        return (isWall(x - 1, y - 1) | isWall(x, y - 1) << 1 | isWall(x + 1, y - 1) << 2 |
                isWall(x + 1, y) << 3 | isWall(x + 1, y + 1) << 4 | isWall(x, y + 1) << 5 |
                isWall(x - 1, y + 1) << 6 | isWall(x - 1, y) << 7);
    }

    // Returns randomly generated floor map. It's used for the FloorTiles but likely to change
    const uint8_t **getFloorMap()
    {
        return (const uint8_t **)floorMap;
    }

    // Gets the item/object located at the grid X, Y coord
    const Gaunt_Item getMapObject(int x, int y)
    {
        // Clamping
        if (x < 0)
            x = (Gaunt_MapSizeW + x);
        else if (x > (int)Gaunt_MapSizeW)
            x %= Gaunt_MapSizeW;
        if (y < 0)
            y = (Gaunt_MapSizeH + y);
        else if (y > (int)Gaunt_MapSizeH)
            y %= Gaunt_MapSizeH;

        return objectMap[y][x];
    }

    // Searches through map to find the next iteration of the object
    //   specified to look for. If a new object type is searched for,
    //   the search resets to 0,0. If no more instances of the object
    //   are found, the list resets to 0,0 and return an invalid object.
    const Gaunt_Item getNextObject(const Gaunt_Objects obj, int &x, int &y)
    {
        static Gaunt_Objects lastObj = Gaunt_Objects::Invalid;
        if (lastObj != obj)
        {
            x = 0;
            y = 0;
            lastObj = obj;
        }
        for (; y < (int)Gaunt_MapSizeH; y++)
        {
            for (; x < (int)Gaunt_MapSizeW; x++)
            {
                const Gaunt_Item iObj = getMapObject(x, y);
                if (iObj.obj == obj)
                {
                    return iObj;
                }
            }
            x = 0;
        }
        x = 0;
        y = 0;
        return Gaunt_Item(Gaunt_Objects::Invalid, 0);
    }

    // Returns whether the object at a given grid X, Y coord is a wall.
    //   Felt like it was self explanatory but comments make our code look pretty.
    bool isWall(int x, int y)
    {
        return (getMapObject(x, y).obj == Gaunt_Objects::Wall);
    }

    // Load in a map from storage
    bool loadMap(){ return false; }

    void generateFloor();
};