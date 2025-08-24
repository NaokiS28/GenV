/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * roboto.cpp - Created on 21-08-2025
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

#include "roboto.hpp"
#include "common/objects/font.hpp"

namespace Fonts
{
    GlyphMetrics roboto_16pt_table[] = {
        {.c = ' ', .x = 0, .y = 0, .width = 2},
        {.c = '!', .x = 0, .y = 0, .width = 2},
        {.c = '"', .x = 0, .y = 0, .width = 2},
        {.c = '#', .x = 0, .y = 0, .width = 2},
        {.c = '$', .x = 0, .y = 0, .width = 2},
        {.c = '%', .x = 0, .y = 0, .width = 2},
        {.c = '&', .x = 0, .y = 0, .width = 2},
        {.c = '\'', .x = 0, .y = 0, .width = 2},
        {.c = '(', .x = 0, .y = 0, .width = 2},
        {.c = ')', .x = 0, .y = 0, .width = 2},
        {.c = '*', .x = 0, .y = 0, .width = 2},
        {.c = '+', .x = 0, .y = 0, .width = 2},
        {.c = ',', .x = 0, .y = 0, .width = 2},
        {.c = '-', .x = 0, .y = 0, .width = 2},
        {.c = '.', .x = 0, .y = 0, .width = 2},
        {.c = '/', .x = 0, .y = 0, .width = 2},
        {.c = '0', .x = 0, .y = 0, .width = 2},
        {.c = '1', .x = 0, .y = 0, .width = 2},
        {.c = '2', .x = 0, .y = 0, .width = 2},
        {.c = '3', .x = 0, .y = 0, .width = 2},
        {.c = '4', .x = 0, .y = 0, .width = 2},
        {.c = '5', .x = 0, .y = 0, .width = 2},
        {.c = '6', .x = 0, .y = 0, .width = 2},
        {.c = '7', .x = 0, .y = 0, .width = 2},
        {.c = '8', .x = 0, .y = 0, .width = 2},
        {.c = '9', .x = 0, .y = 0, .width = 2},
        {.c = ':', .x = 0, .y = 0, .width = 2},
        {.c = ';', .x = 0, .y = 0, .width = 2},
        {.c = '<', .x = 0, .y = 0, .width = 2},
        {.c = '=', .x = 0, .y = 0, .width = 2},
        {.c = '>', .x = 0, .y = 0, .width = 2},
        {.c = '?', .x = 0, .y = 0, .width = 2},
        {.c = '@', .x = 0, .y = 0, .width = 2},
        {.c = 'A', .x = 0, .y = 0, .width = 2},
        {.c = 'B', .x = 0, .y = 0, .width = 2},
        {.c = 'C', .x = 0, .y = 0, .width = 2},
        {.c = 'D', .x = 0, .y = 0, .width = 2},
        {.c = 'E', .x = 0, .y = 0, .width = 2},
        {.c = 'F', .x = 0, .y = 0, .width = 2},
        {.c = 'G', .x = 0, .y = 0, .width = 2},
        {.c = 'H', .x = 0, .y = 0, .width = 2},
        {.c = 'I', .x = 0, .y = 0, .width = 2},
        {.c = 'J', .x = 0, .y = 0, .width = 2},
        {.c = 'K', .x = 0, .y = 0, .width = 2},
        {.c = 'L', .x = 0, .y = 0, .width = 2},
        {.c = 'M', .x = 0, .y = 0, .width = 2},
        {.c = 'N', .x = 0, .y = 0, .width = 2},
        {.c = 'O', .x = 0, .y = 0, .width = 2},
        {.c = 'P', .x = 0, .y = 0, .width = 2},
        {.c = 'Q', .x = 0, .y = 0, .width = 2},
        {.c = 'R', .x = 0, .y = 0, .width = 2},
        {.c = 'S', .x = 0, .y = 0, .width = 2},
        {.c = 'T', .x = 0, .y = 0, .width = 2},
        {.c = 'U', .x = 0, .y = 0, .width = 2},
        {.c = 'V', .x = 0, .y = 0, .width = 2},
        {.c = 'W', .x = 0, .y = 0, .width = 2},
        {.c = 'X', .x = 0, .y = 0, .width = 2},
        {.c = 'Y', .x = 0, .y = 0, .width = 2},
        {.c = 'Z', .x = 0, .y = 0, .width = 2},
        {.c = '[', .x = 0, .y = 0, .width = 2},
        {.c = '\\', .x = 0, .y = 0, .width = 2},
        {.c = ']', .x = 0, .y = 0, .width = 2},
        {.c = '^', .x = 0, .y = 0, .width = 2},
        {.c = '_', .x = 0, .y = 0, .width = 2},
        {.c = '`', .x = 0, .y = 0, .width = 2},
        {.c = 'a', .x = 0, .y = 0, .width = 2},
        {.c = 'b', .x = 0, .y = 0, .width = 2},
        {.c = 'c', .x = 0, .y = 0, .width = 2},
        {.c = 'd', .x = 0, .y = 0, .width = 2},
        {.c = 'e', .x = 0, .y = 0, .width = 2},
        {.c = 'f', .x = 0, .y = 0, .width = 2},
        {.c = 'g', .x = 0, .y = 0, .width = 2},
        {.c = 'h', .x = 0, .y = 0, .width = 2},
        {.c = 'i', .x = 0, .y = 0, .width = 2},
        {.c = 'j', .x = 0, .y = 0, .width = 2},
        {.c = 'k', .x = 0, .y = 0, .width = 2},
        {.c = 'l', .x = 0, .y = 0, .width = 2},
        {.c = 'm', .x = 0, .y = 0, .width = 2},
        {.c = 'n', .x = 0, .y = 0, .width = 2},
        {.c = 'o', .x = 0, .y = 0, .width = 2},
        {.c = 'p', .x = 0, .y = 0, .width = 2},
        {.c = 'q', .x = 0, .y = 0, .width = 2},
        {.c = 'r', .x = 0, .y = 0, .width = 2},
        {.c = 's', .x = 0, .y = 0, .width = 2},
        {.c = 't', .x = 0, .y = 0, .width = 2},
        {.c = 'u', .x = 0, .y = 0, .width = 2},
        {.c = 'v', .x = 0, .y = 0, .width = 2},
        {.c = 'w', .x = 0, .y = 0, .width = 2},
        {.c = 'x', .x = 0, .y = 0, .width = 2},
        {.c = 'y', .x = 0, .y = 0, .width = 2},
        {.c = 'z', .x = 0, .y = 0, .width = 2},
        {.c = '{', .x = 0, .y = 0, .width = 2},
        {.c = '|', .x = 0, .y = 0, .width = 2},
        {.c = '}', .x = 0, .y = 0, .width = 2},
        {.c = '~', .x = 0, .y = 0, .width = 2},
        {.c = '\0', .x = 0, .y = 0, .width = 2}};

    FontMetrics roboto_16pt = makeFontInfo(
        "Roboto",
        "Christian Robertson, Paratype, Font Bureau",
        16,
        roboto_16pt_table,
        (sizeof(roboto_16pt_table) / sizeof(GlyphMetrics)));
} // namespace Fonts