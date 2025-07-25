/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gaunt_screens.cpp - Created on 25-06-2025
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

#include "gaunt_screens.hpp"

namespace Gauntlet {
    GauntletSubApp *createNewApp(GauntletApps app){
        GauntletSubApp *screen = nullptr;
        switch(app){
            case GauntletApps::G_Title_Screen:
                screen = new GauntletTitle;
                break;
            case GauntletApps::G_Game_Screen:
                screen = new GauntletField;
                break;
            case GauntletApps::G_Demo_Screen:
                screen = new GauntletField;
                break;
        }
        return screen;
    }
}