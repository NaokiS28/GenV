/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * psx_strings.hpp - Created on 13-11-2025
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

namespace System::PSX
{
    constexpr const char szPlaystation[] = "PlayStation";
    constexpr const char szSony[]        = "Sony";

    constexpr const char szRedux[] = "Detected host as PCSX-Redux.";

    constexpr const char PSX_IO_ERROR_FMT[]      = "Failed to %s PSX %s driver.";
    constexpr const char PSX_IO_PORT_ERROR_FMT[] = "Failed to %s PSX %s driver on port %d.";
    constexpr const char PSX_GPU_STR[]           = "PlayStation GPU";
    constexpr const char PSX_CDROM_DRIVE_STR[]   = "CD-ROM drive";
    constexpr const char PSX_PC_DRIVE_STR[]      = "PC drive";
    constexpr const char PSX_MEMORY_CARD_STR[]   = "memory card";
    constexpr const char PSX_JOYPAD_STR[]        = "joypad";
    constexpr const char PSX_CREATE_STR[]        = "create";
    constexpr const char PSX_INIT_STR[]          = "init";

    constexpr const char PSX_PS_CONTROLLER_STR[] = "PlayStation Controller";
    constexpr const char PSX_PS_MEMCARD_STR[]    = "PlayStation Memory Card";

    constexpr const char PSX_PLAYSTATION_STR[] = "PlayStation";
    constexpr const char PSX_DIGITAL_STR[]     = "PlayStation Digital";
    constexpr const char PSX_ANALOG_STR[]      = "PlayStation DualAnalog/DualShock";
    constexpr const char PSX_MOUSE_STR[]       = "PlayStation Mouse";
    constexpr const char PSX_TWINSTICK_STR[]   = "PlayStation TwinStick";
    constexpr const char PSX_GUNCON_STR[]      = "Namco GunCon";
    constexpr const char PSX_JUSTIFIER_STR[]   = "Konami Justifier";
    constexpr const char PSX_DUALSHOCK_STR[]   = "PlayStation DualShock";
    constexpr const char PSX_DUALSHOCK2_STR[]  = "PlayStation DualShock2";
    constexpr const char PSX_NEGCON_STR[]      = "Namco NegCon";
    constexpr const char PSX_JOGCON_STR[]      = "Namco JogCon";

} // namespace System::PSX