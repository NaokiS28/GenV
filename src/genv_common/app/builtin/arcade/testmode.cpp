/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * testmode.cpp - Created on 02-08-2025
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

#include "testmode.hpp"

GenVTestApp::GenVTestApp() : ArcadeTestApp()
{
}

int GenVTestApp::init(IAppHost *host)
{
    m_host = host;
    return 0;
}
void GenVTestApp::update()
{
    if (aSystem->runTestMode())
    {
    }
    else
    {
        state = APP_STATE_QUIT;
        // leave
    }
}
void GenVTestApp::render()
{
}