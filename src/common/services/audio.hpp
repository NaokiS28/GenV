/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * common/classes.hpp - Created on 24-04-2025
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

namespace Audio
{
    class IAudio
    {
    public:
        IAudio() = default;
        virtual ~IAudio() = default;
        virtual bool init() = 0;
        virtual bool reset() = 0;
        virtual void shutdown() = 0;
    };
}