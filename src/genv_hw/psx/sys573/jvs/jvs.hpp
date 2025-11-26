/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * jvs.hpp - Created on 26-11-2025
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

#include "common/services/io/iface_input.hpp"
#include "common/services/services.hpp"

namespace System::PSX
{
    using namespace Input;

    constexpr const char GX700_JVS_NAME[] = "System 573 JVS";

    class Sys573JVS : public IInputDriver
    {
    private:
        AsyncService _sys573JVSService = {
            "System 573 JVS",
            [](void *arg)
            {
                auto service = reinterpret_cast<Sys573JVS *>(arg);
                service->processPackets_();
            },
            this};

        void processPackets_();

        IInputDevice _devs[4];
        util::RingBuffer<uint8_t, 64> _packetBuffer;

    public:
        inline Sys573JVS()
        {
            _name = GX700_JVS_NAME;
        }

        int init() override;
        int update() override;
    };
} // namespace System::PSX