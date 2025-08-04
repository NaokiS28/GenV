/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * app_errorcodes.hpp - Created on 02-08-2025
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

enum GenVAppErrorCodes : uint32_t {
    GENV_APP_ERR_UNKNOWN,
    GENV_APP_ERR_NO_APPS,
    GENV_APP_ERR_INVALID_LOADER,
    GENV_APP_ERR_INVALID_GAME,
};