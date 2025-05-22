/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * rect.h - Created on 03-05-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include <stdint.h>

struct Rect
{
public:
    int x1, y1, x2, y2;
};

struct RectWH
{
public:
    int x, y, w, h;
};

struct RectRB
{
public:
    int x, y, r, b;
};