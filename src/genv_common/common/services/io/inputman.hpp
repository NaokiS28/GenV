/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * inputman.hpp - Created on 22-08-2025
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

#include "common/util/templates.hpp"
#include "iface_input.hpp"
#include "common/services/adminkey.hpp"
#include <string.h>

namespace Input
{
    static constexpr const int initialDrivers     = 10;
    static constexpr const int initialControllers = 12;
    static constexpr const int initialKeyboards   = 6;
    static constexpr const int initialMice        = 6;

    // Placeholders - VKey and VMouse will provide a universal inteface to the app
    class VPad
    {
    };

    class VKeyboard
    {
    };

    class VMouse
    {
    };

    class InputManager
    {
    private:
        util::PointerList<IInputDriver *, initialDrivers> _driverList;
        util::PointerList<IController *, initialControllers> _padList;
        util::PointerList<IKeyboard *, initialKeyboards> _keyList;
        util::PointerList<IMouse *, initialMice> _mouseList;

    public:
        inline InputManager(AdminClass_Key key) {};
        ~InputManager();

        int init();
        void update();
        void reset();
        void shutdown();

        int registerDriver(Input::IInputDriver *device);
        int unregisterDriver(Input::IInputDriver *device);

        int attachDevice(Input::IInputDevice *driver);
        int detachDevice(Input::IInputDevice *driver);
    };
} // namespace Input