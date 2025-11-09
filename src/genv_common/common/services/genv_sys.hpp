/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * genv_sys.hpp - Created on 09-08-2025
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

#include "adminkey.hpp"

// Core GenV class. This exists as a class so it can use the adminkey system
// to directly manipulate the services available. Else it would be C, trust me ken.
class GenvSystemClass
{
private:
    AdminClass_Key adminKey;

public:
    GenvSystemClass() : adminKey(AdminClass_Key()) {}
    void startup();
    void shutdown();
    void halt(int return_code = -1); // On non-computer targets, halts execution
};