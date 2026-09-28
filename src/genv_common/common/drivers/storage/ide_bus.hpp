/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * iface_ide.hpp - Created on 23-07-2026
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

#include "ide_defs.hpp"

#include <stdint.h>
#include <stddef.h>

namespace IDE
{
    class Device
    {
    };

    typedef uint16_t (*ReadCommand)(void);
    typedef uint16_t (*ReadControl)(void);
    typedef void (*WriteCommand)(CS0Register reg, uint16_t data);
    typedef void (*WriteControl)(CS1Register reg, uint16_t data);

    class BaseIDEDriver
    {
        struct CommandFunctions
        {
            ReadCommand read   = nullptr;
            WriteCommand write = nullptr;
        } _cmd;

        struct ControlFunctions
        {
            ReadControl read   = nullptr;
            WriteControl write = nullptr;
        } _ctrl;

    protected:
        int _readPIO(LBA address, size_t count = 1);
        int _writePIO(LBA address, Sector *sector, size_t count = 1);
        int _readDMA(LBA address, size_t count = 1);
        int _writeDMA(LBA address, Sector *sector, size_t count = 1);

    public:
        inline BaseIDEDriver(
            ReadCommand readCmd,
            WriteCommand writeCmd,
            ReadControl readCtrl,
            WriteControl writeCtrl)
            : _cmd(readCmd, writeCmd),
              _ctrl(readCtrl, writeCtrl) {}

        virtual int init();
        virtual bool update();
        virtual bool reset();
        virtual void shutdown();

        virtual void isr();
    };
} // namespace IDE