/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * terminal.hpp - Created on 10-08-2025
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

class ITerminal {
    public:
        ITerminal() = default;
        virtual ~ITerminal() = default;
        virtual void setBaud(int baud) = 0;
        virtual void setPort(int port) = 0;
        virtual void writeChar() = 0;
        virtual char readChar() = 0;

};