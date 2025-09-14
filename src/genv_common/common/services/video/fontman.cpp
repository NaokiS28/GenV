/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * fontman.cpp - Created on 21-08-2025
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

#include "fontman.hpp"

#include <string.h>

#include "common/formats/image_file.hpp"
#include "common/objects/font.hpp"
#include "common/objects/fonts/spice.hpp"
#include "common/objects/texture.hpp"
#include "common/services/adminkey.hpp"
#include "common/logger/log.hpp"

#define LOG_FONT(fmt, ...) LOG("fontmgr", fmt __VA_OPT__(, ) __VA_ARGS__)

constexpr const char *szLoadedFont = "Loaded new font: %s (%s)";
constexpr const char *szRemovedFont = "Removed font: %s";
constexpr const char *szFontLoadFailed = "Could not load font %s: %s";
constexpr const char *szRemoveFontFailed = "Could not remove font: %s";
constexpr const char *szBadFontObject = "FontObject is invalid.";
constexpr const char *szParameterError = "Parameter error.";
constexpr const char *szFontNotFound = "Font not found.";
constexpr const char *szOutOfMemory = "Out of memory.";
constexpr const char *szListIsEmpty = "Font list is empty.";

namespace Fonts
{
    FontManager::FontManager(AdminClass_Key key)
    {
    }

    int FontManager::_expandList(int count)
    {
        FontObject **fList = new FontObject *[fontListLength + count];
        if (!fList)
        {
            LOG_FONT(szFontLoadFailed, szOutOfMemory);
            return FM_OUT_OF_MEMORY;
        }
        fontListLength += count;
        memmove(fList, fontList, (sizeof(FontObject *) * fontListLength));
        delete[] fontList;
        fontList = fList;
        LOG_FONT("Expanded list by %i place(s).", fontListLength);
        return FM_OKAY;
    }

    int FontManager::_shrinkList(int count)
    {
        if (count > fontListLength) return FM_PARAMETER_ERROR;

        FontObject **fList = new FontObject *[fontListLength - count];
        if (!fList)
        {
            LOG_FONT(szFontLoadFailed, szOutOfMemory);
            return FM_OUT_OF_MEMORY;
        }
        fontListLength -= count;
        memmove(fList, fontList, (sizeof(FontObject *) * fontListLength));
        delete[] fontList;
        fontList = fList;
        LOG_FONT("Shrank list by %i place(s).", fontListLength);
        return FM_OKAY;
    }

    int FontManager::init()
    {
        LOG_FONT("Init FontManager.");
        Textures::TextureObject *tObj = Textures::loadGIF_memory(
            spice_10pt.id, spice_10pt_gif, spice_10pt_size);
        loadFontFromMemory(tObj, &spice_10pt);
        return 0;
    }

    void FontManager::shutdown()
    {
        delete[] fontList;
    }

    int FontManager::_loadFont(FontObject *fObj)
    {
        if (!fObj)
        {
            LOG_FONT(szFontLoadFailed, fObj->getMetrics()->name, szBadFontObject);
            return FM_PARAMETER_ERROR;
        }

        fontList[fontListLength - 1] = fObj;
        LOG_FONT(szLoadedFont, fObj->getMetrics()->name, fObj->getMetrics()->designer);
        return FM_OKAY;
    }

    int FontManager::loadFontFromFile(const char *filePath)
    {
        if (!filePath)
        {
            LOG_FONT(szFontLoadFailed, filePath, szParameterError);
            return FM_PARAMETER_ERROR;
        }

        if (_expandList(1) == FM_OKAY)
        {
            FontObject *fObj = createFontFromFile(filePath);
            if (!fObj)
            {
                LOG_FONT(szFontLoadFailed, szBadFontObject);
                return FM_CREATE_OBJECT_FAILED;
            }
            if (fObj->uploadTexture() == 0)
            {
                return _loadFont(fObj);
            }
            else
            {
                LOG_FONT(szFontLoadFailed, szBadFontObject);
                return FM_CREATE_OBJECT_FAILED;
            }
        }
        return FM_OUT_OF_MEMORY;
    }

    int FontManager::loadFontFromMemory(const uint8_t *data, const size_t length, const FontMetrics *metrics)
    {
        if (!data || !length || !metrics)
        {
            LOG_FONT(szFontLoadFailed, metrics->name, szParameterError);
            return FM_PARAMETER_ERROR;
        }

        if (_expandList(1) == FM_OKAY)
        {
            FontObject *fObj = createFontFromMem(data, length, metrics);
            if (!fObj)
            {
                LOG_FONT(szFontLoadFailed, szBadFontObject);
                return FM_CREATE_OBJECT_FAILED;
            }

            if (fObj->uploadTexture() == 0)
            {
                return _loadFont(fObj);
            }
            else
            {
                LOG_FONT(szFontLoadFailed, szBadFontObject);
                return FM_CREATE_OBJECT_FAILED;
            }
        }
        return FM_OUT_OF_MEMORY;
    }

    int FontManager::loadFontFromMemory(Textures::TextureObject *tObj, const FontMetrics *metrics)
    {
        if (!tObj || !metrics)
        {
            LOG_FONT(szFontLoadFailed, szParameterError);
            return FM_PARAMETER_ERROR;
        }

        if (_expandList(1) == FM_OKAY)
        {
            FontObject *fObj = createFontFromMem(tObj, metrics);
            if (!fObj)
            {
                LOG_FONT(szFontLoadFailed, szBadFontObject);
                return FM_CREATE_OBJECT_FAILED;
            }

            if (fObj->uploadTexture() == 0)
            {
                return _loadFont(fObj);
            }
            else
            {
                LOG_FONT(szFontLoadFailed, szBadFontObject, metrics->name);
                return FM_CREATE_OBJECT_FAILED;
            }
        }
        return FM_OUT_OF_MEMORY;
    }

    int FontManager::unloadFontAt(const uint8_t idx)
    {
        if (!fontListLength)
        {
            LOG_FONT(szRemoveFontFailed, szListIsEmpty);
            return FM_LIST_EMPTY;
        }

        if (idx < fontListLength)
        {
            char temp[32];
            strncpy(temp, fontList[idx]->getMetrics()->name, 32);
            delete fontList[idx];
            memmove(&fontList[idx], &fontList[idx + 1], fontListLength - idx);
            if (_shrinkList(1) == FM_OKAY)
            {
                LOG_FONT(szRemovedFont, temp);
                return FM_OKAY;
            }
            return FM_OUT_OF_MEMORY;
        }

        LOG_FONT(szRemoveFontFailed, szFontNotFound);
        return FM_NOT_FOUND;
    }

    int FontManager::unloadFont(util::Hash id)
    {
        if (!fontListLength)
        {
            LOG_FONT(szRemoveFontFailed, szListIsEmpty);
            return FM_LIST_EMPTY;
        }

        int idx = 0;
        bool fontFound = false;
        do
        {
            if (fontList[idx]->getMetrics()->id == id) fontFound = true;
            idx++;
        } while (idx < fontListLength && !fontFound);

        if (!fontFound)
        {
            LOG_FONT(szRemoveFontFailed, szFontNotFound);
            return FM_NOT_FOUND;
        }

        return unloadFontAt(idx);
    }
} // namespace Fonts