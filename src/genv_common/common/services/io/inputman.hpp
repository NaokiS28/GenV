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

#include "iface_input.hpp"
#include "common/services/adminkey.hpp"

namespace Input
{
    class InputManager
    {
    private:
        IInput **deviceList = nullptr;
        int deviceCount = 0;
        int deviceListLength = 0;

    public:
        InputManager(AdminClass_Key key);
        ~InputManager();

        int init();
        void update();
        void reset();
        void shutdown();

        bool attachDevice(Input::IInput *dev);
        bool detachDevice(Input::IInput *dev);
    };
} // namespace Input