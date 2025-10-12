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

#include <stdint.h>
#include <string.h>

#include "common/formats/image_file.hpp"
#include "common/logger/error_strings.hpp"
#include "common/objects/font.hpp"
#include "common/objects/fonts/spice.hpp"
#include "common/objects/texture.hpp"
#include "common/return_codes.hpp"
#include "common/services/adminkey.hpp"
#include "common/logger/log.hpp"

#define LOG_FONT(fmt, ...) LOG("fontmgr", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace Fonts
{
    FontManager::FontManager(AdminClass_Key key)
    {
    }

    int FontManager::init()
    {
        LOG_FONT("Init FontManager.");
        loadFontFromMemory(spice.fontList[0]); // TODO: Configure by header rather than hardcoded here
        return GV_OK;
    }

    void FontManager::shutdown()
    {
    }

    int FontManager::_loadFont(FontObject *fObj)
    {
        if (!fObj)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject, fObj->getHeader().name,
                     LogReasonStrings::GenvParameterError);
            return FM_ERROR(GV_ERR_INVALID_PARAM);
        }

        fontList[fontListLength] = fObj;
        fontListLength += 1;

        LOG_FONT(LogErrorStrings::GenvLoadedItem,
                 ObjectStrings::FontObject,
                 fObj->getHeader().name, fObj->getHeader().designer);
        return GV_OK;
    }

    int FontManager::loadFontFromFile(const char *filePath)
    {
        if (!filePath)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject, filePath,
                     LogReasonStrings::GenvParameterError);
            return FM_ERROR(GV_ERR_INVALID_PARAM);
        }

        FontObject *fObj = nullptr;
        int r = Fonts::loadFontFromFile(&fObj, filePath);
        if (r)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvBadObject);
            return r;
        }
        if (fObj->uploadTexture() == 0)
        {
            return _loadFont(fObj);
        }
        else
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvBadObject);
            return FM_ERROR(GV_ERR_UPLOAD_FAILED);
        }

        return FM_ERROR(GV_ERR_OUT_OF_MEMORY);
    }

    int FontManager::loadFontFromMemory(const uint8_t *data, const size_t length, const FontHeader header)
    {
        if (auto p = testParams(data != nullptr, length, header.validateMagic()))
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject, header.name,
                     LogReasonStrings::GenvParameterError, p);
            return FM_ERROR(GV_ERR_INVALID_PARAM);
        }

        FontObject *fObj = nullptr;
        int r = Fonts::loadFontFromMem(&fObj, data, length);
        if (r)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvBadObject);
            return r;
        }

        if (fObj->uploadTexture() == 0)
        {
            return _loadFont(fObj);
        }
        else
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvUploadFailed);
            return FM_ERROR(GV_ERR_UPLOAD_FAILED);
        }
        return FM_ERROR(GV_ERR_OUT_OF_MEMORY);
    }

    int FontManager::loadFontFromMemory(const FontObject &font)
    {
        const FontHeader header = font.getHeader();
        if (auto p = testParams((
                header.bitmapLength != 0 &&
                header.bitmap != nullptr)))
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject, header.name,
                     LogReasonStrings::GenvParameterError, p);
            return FM_ERROR(GV_ERR_INVALID_PARAM);
        }

        Textures::TextureObject *tObj = Textures::openImageMemory(
            header.id, header.bitmapType,
            header.bitmap, header.bitmapLength);

        if (tObj)
        {
            FontObject *fObj = new FontObject(header.id, header, tObj);
            if (fObj->uploadTexture() == 0)
            {
                return _loadFont(fObj);
            }
            else
            {
                delete tObj;
                LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                         ObjectStrings::FontObject,
                         LogReasonStrings::GenvBadObject);
                return FM_ERROR(GV_ERR_UPLOAD_FAILED);
            }
        }
        else
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvTypeIncompatible);
            return FM_ERROR(GV_ERR_INCOMPATIBLE_TYPE);
        }
        return FM_ERROR(GV_ERR_OUT_OF_MEMORY);
    }

    FontObject *FontManager::getFont(uint8_t idx)
    {
        if (!fontListLength)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed, LogReasonStrings::GenvListIsEmpty);
            return nullptr;
        }

        if (idx < fontListLength)
        {
            return fontList[idx];
        }

        LOG_FONT(LogErrorStrings::GenvRemoveItemFailed,
                 ObjectStrings::FontObject,
                 LogReasonStrings::GenvItemNotFound);
        return nullptr;
    }

    FontObject *FontManager::getFont(util::Hash idx)
    {
        if (!fontListLength)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed, LogReasonStrings::GenvListIsEmpty);
            return nullptr;
        }

        for (int i = 0; i < fontListLength; i++)
        {
            if (fontList[i]->getObjectID() == idx)
                return fontList[i];
        }

        LOG_FONT(LogErrorStrings::GenvRemoveItemFailed,
                 ObjectStrings::FontObject,
                 LogReasonStrings::GenvItemNotFound);
        return nullptr;
    }

    int FontManager::setFont(uint8_t idx)
    {
        if (getFont(idx) != nullptr)
        {
            currentFontIdx = idx;
        }
        else
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvItemNotFound,
                     idx = '0');
            return FM_ERROR(GV_ERR_ITEM_NOT_FOUND);
        }
        return GV_ERR_UNSUPPORTED;
    }

    int FontManager::setFont(util::Hash fontId)
    {
        if (getFont(fontId) != nullptr)
        {
        }
        else
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvItemNotFound,
                     fontId + '0');
            return FM_ERROR(GV_ERR_ITEM_NOT_FOUND);
        }
        return GV_ERR_UNSUPPORTED;
    }

    int FontManager::unloadFontAt(const uint8_t idx)
    {
        if (!fontListLength)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed, LogReasonStrings::GenvListIsEmpty);
            return FM_ERROR(GV_ERR_ITEM_NOT_FOUND);
        }

        if (idx < fontListLength)
        {
            char temp[32];
            strncpy(temp, fontList[idx]->getHeader().name, 32);
            delete fontList[idx];
            memmove(&fontList[idx], &fontList[idx + 1], fontListLength - idx);
            return FM_ERROR(GV_ERR_OUT_OF_MEMORY);
        }

        LOG_FONT(LogErrorStrings::GenvRemoveItemFailed,
                 ObjectStrings::FontObject,
                 LogReasonStrings::GenvItemNotFound);
        return FM_ERROR(GV_ERR_ITEM_NOT_FOUND);
    }

    int FontManager::unloadFont(util::Hash id)
    {
        if (!fontListLength)
        {
            LOG_FONT(LogErrorStrings::GenvRemoveItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvListIsEmpty);
            return FM_ERROR(GV_ERR_LIST_EMPTY);
        }

        int idx = 0;
        bool fontFound = false;
        do
        {
            if (fontList[idx]->getHeader().id == id) fontFound = true;
            idx++;
        } while (idx < fontListLength && !fontFound);

        if (!fontFound)
        {
            LOG_FONT(LogErrorStrings::GenvRemoveItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvItemNotFound);
            return FM_ERROR(GV_ERR_ITEM_NOT_FOUND);
        }

        return unloadFontAt(idx);
    }
} // namespace Fonts