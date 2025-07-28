/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * text.cpp - Created on 03-05-2025
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

#include "text.h"
#include <string.h>

const char *WordWrap(const char* text, RectWH boundry){
    if(text == nullptr) return nullptr;

    char *token;
    char *str = new char(strlen(text));
    int newLines = 0;
    //int lineWidth = 0;

    if(str == nullptr) return nullptr;
    strcpy(str, text);

    token = strtok(str, " -");
    while(token != NULL){
        if(false){          // If this word will go over the boundry
            newLines++; 
        }
        token = strtok(str, " -");
    }
    return nullptr;
}