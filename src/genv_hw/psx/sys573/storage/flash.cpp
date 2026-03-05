/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * flash.c - Created on 26-11-2025
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

#include "common/services/storage/storage.hpp"
#include "psx/sys573/io/asic.hpp"
#include "psx/sys573/io/io.hpp"

namespace System573::IO
{
    uint8_t sys573_bank_state = 0;

    namespace SecurityCart
    {
        void SetGPIODirection(GPIODir state)
        {
            bool _st = state == GPIODir::Output;
            if (_st)
                sys573_bank_state |= (_st << 6);
            else
                sys573_bank_state &= ~(_st << 6);
            ASIC::Regs::BankCtrl = sys573_bank_state;
        }
    } // namespace SecurityCart

    namespace Flash
    {
        static constexpr uint32_t BaseAddr = DEV0_BASE | 0x000000;
        static constexpr int FlashSize     = Files::MiB(16);
        static constexpr int BankSize      = Files::MiB(4);

        /*struct FlashView
        {
            volatile uint16_t *const ptr = _ADDR16(BaseAddr);

            volatile uint16_t &operator[](int offset) const
            {
                return ptr[offset % FlashSize];
            }
        };*/

        namespace Regs
        {
            // static volatile uint16_t &Ctrl    = *_ADDR16(_A8_to_A16(ClockBaseAddr, 0x00));
        } // namespace Regs

        void SetBank(uint8_t bank)
        {
            bank &= 0x3F;
            if (!(bank & 0x30)) bank &= 0x03;
            uint8_t mask         = sys573_bank_state ^ bank;
            sys573_bank_state    = (sys573_bank_state & mask) | bank;
            ASIC::Regs::BankCtrl = sys573_bank_state;
        }
    } // namespace Flash
} // namespace System573::IO