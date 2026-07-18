/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * iface_driver.hpp - Created on 23-02-2026
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

#include "common/return_codes.hpp"
#include "common/util/hash.hpp"

namespace System
{
    class ISystem; // forward decl - every driver holds a reference to its owning system

    // Base class for all drivers. A driver represents a hardware bus, device or chip.
    class IDriver
    {
    protected:
        //! Review
        // Injected reference to the owning System, set at construction. A reference
        // (not a pointer) because a driver without a system is meaningless - this
        // makes a driver impossible to default-construct or reseat, and lets it reach
        // system services (e.g. screen allocation) without going through a global.
        ISystem &_system;
        //! End
        const char *_name = nullptr;
        util::Hash id;

    public:
        //! Review
        IDriver(ISystem &sys) : _system(sys) {}
        IDriver(ISystem &sys, util::Hash id) : _system(sys), id(id) {}
        //! End
        virtual ~IDriver() = default;
        virtual int init() { return GV_OK; };
        virtual bool update() = 0;
        virtual bool reset() { return GV_OK; };
        virtual void shutdown() {};

        const char *getName() { return _name; }
    };
} // namespace System