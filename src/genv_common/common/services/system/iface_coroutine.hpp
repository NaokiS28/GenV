/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * iface_coroutine.hpp - Created on 07-02-2026
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

#include "stdint.h"
#include "stddef.h"

enum class CoroutineStep : uint8_t
{
    Init,
    Running,
    Done
};

class ICoroutine
{
protected:
    CoroutineStep m_step = CoroutineStep::Init;

public:
    int listID = -1;

    virtual ~ICoroutine() {}

    // Called once by coroutine system
    virtual void start() = 0;

    // Called repeatedly until it returns true
    virtual bool resume() = 0;

    virtual const char *name() = 0;

    inline CoroutineStep status() const { return m_step; }
};
