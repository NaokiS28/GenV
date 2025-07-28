/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * rect.h - Created on 03-05-2025
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
#include "common/util/ifloat.hpp"

struct Coord
{
public:
    Coord() {}
    Coord(int _x, int _y) : x(_x), y(_y) {}
    Coord(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
    int x = 0, y = 0, z = 0;
};

struct CoordUV {
    ifloat u = 0.0f;
    ifloat v = 0.0f; 
};

struct Vertex
{
    public:
    Vertex(){}
    Vertex(int _x, int _y, int _z){
        pos.x = _x;
        pos.y = _y;
        pos.z = _z;
    }
    Vertex(int _x, int _y, int _z, ifloat _rhw, ifloat _u, ifloat _v){
        pos.x = _x;
        pos.y = _y;
        pos.z = _z;
        rhw = _rhw;
        tex.u = _u;
        tex.v = _v;
    }
    Coord pos = {0, 0, 0};
    ifloat rhw = 1.0f;
    CoordUV tex = { 0, 0 };
};

struct Rect
{
public:
    Rect() {}
    Rect(int _x1, int _y1, int _x2, int _y2) : x1(_x1), y1(_y1), x2(_x2), y2(_y2) {}
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
};

struct RectWH
{
public:
    RectWH() {}
    RectWH(int _x, int _y, int _w, int _h) : x(_x), y(_y), w(_w), h(_h) {}
    int x = 0, y = 0, w = 0, h = 0;
};

struct RectUV
{
public:
    RectUV() {}
    RectUV(ifloat _u1, ifloat _v1, ifloat _u2, ifloat _v2) : u1(_u1), v1(_v1), u2(_u2), v2(_v2) {}
    ifloat u1 = 0, v1 = 0, u2 = 0, v2 = 0;
};

struct RectRB
{
public:
    RectRB() {}
    RectRB(int _x, int _y, int _r, int _b) : x(_x), y(_y), r(_r), b(_b) {}
    int x = 0, y = 0, r = 0, b = 0;
};