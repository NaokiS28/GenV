/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * genv_stb_image.h - Created on 25-08-2025
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

// genv_stb_image.h
// Thin wrapper to use stb_image as a conventional .h/.c library.
// This header exposes the API by including stb_image.h WITHOUT defining STB_IMAGE_IMPLEMENTATION.

#pragma once

// Forward-include the original single-header API (declarations only here).

#include "vendor_conf.h"
#include "stb_image.h"
