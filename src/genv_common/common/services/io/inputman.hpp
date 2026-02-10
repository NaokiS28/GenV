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

#include "common/services/io/inputman.hpp"
#include "common/services/io/vpad.hpp"
#include "common/util/templates.hpp"
#include "iface_input.hpp"
#include "common/services/adminkey.hpp"
#include <string.h>

namespace Input
{
    static constexpr const int initialDrivers = 4;
    static constexpr const int initialControllers = 8;
    static constexpr const int initialKeyboards = 2;
    static constexpr const int initialMice = 4;

    // Placeholders - VKey and VMouse will provide a universal inteface to the app
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
        util::PointerList<const IInputDevice *, initialControllers> _devList;

    public:
        inline InputManager(AdminClass_Key key) {};
        ~InputManager();

        VPad vpad;

        int init();
        void update();
        void reset();
        void shutdown();

        int registerDriver(Input::IInputDriver *device);
        int unregisterDriver(Input::IInputDriver *device);

        int attachDevice(Input::IInputDevice *driver, Input::Player player);
        int detachDevice(Input::IInputDevice *driver);

        const char *deviceName(size_t idx);
        const int devicePlayer(size_t idx);

        inline size_t deviceCount() { return _devList.length(); }
    };
} // namespace Input