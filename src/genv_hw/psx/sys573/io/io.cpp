/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * io.c - Created on 26-11-2025
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

#include <stdint.h>
#include <stdbool.h>

#include "io.hpp"

namespace System573::IO
{
    namespace EXTOUT
    {
        uint8_t ext_out_state = 0;

        void SetState(uint8_t data)
        {
            ext_out_state = data;
            ExtOut        = ext_out_state;
        }

        uint8_t GetState()
        {
            return ext_out_state;
        }

        void WriteBite(uint8_t bit, bool state)
        {
            if (bit > 7) return;
            if (state)
                ext_out_state |= (1 << bit);
            else
                ext_out_state &= ~(1 << bit);
            ExtOut = ext_out_state;
        }
    } // namespace EXTOUT

    namespace SecurityCart
    {

    } // namespace SecurityCart
} // namespace System573::IO