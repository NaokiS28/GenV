/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * adminkey.hpp - Created on 09-08-2025
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

class GenvSystemClass;
class ServiceManager;
namespace System
{
    class BaseSystem;
}

// Allow access to administrative functions to specific classes.
// This exists purely to restrict what functions and classes can
// modify critical system resources. It should not need to be touched
// by anything else.
class AdminClass_Key
{
    friend class ServiceManager;
    friend class GenvSystemClass;
    friend class System::BaseSystem;

private:
    inline AdminClass_Key() {}
};