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
#include "common/objects/object.hpp"
#include "common/objects/texture.hpp"
#include "common/return_codes.hpp"
#include "common/services/adminkey.hpp"
#include "common/logger/log.hpp"
#include "common/util/hash.hpp"

#define LOG_FONT(fmt, ...) LOG("fontmgr", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace Fonts
{
    FontManager::FontManager(AdminClass_Key key)
    {
    }

    int FontManager::init()
    {
        LOG_FONT("Init FontManager.");
        if (loadFontsetFromMemory(spice_data, spice_len) == GV_OK) // TODO: Configure by header rather than hardcoded here
            setFont(spice_hash, 12);
        return GV_OK;
    }

    void FontManager::shutdown()
    {
        for (auto font : _fontList)
        {
            delete font;
        }
    }

    int FontManager::_loadFontset(FontsetObject *fObj)
    {
        if (_fontListLength < MAX_FONTS)
        {
            _fontList[_fontListLength] = fObj;
            _fontListLength += 1;
            LOG_FONT(LogErrorStrings::GenvLoadedItem,
                     ObjectStrings::FontObject,
                     fObj->getFamilyName(), fObj->getDesignerName());
            return GV_OK;
        }
        else
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontsetObject,
                     LogReasonStrings::GenvListIsFull);
            return FO_ERROR(GV_ERR_OUT_OF_SPACE);
        }
    }

    int FontManager::loadFontsetFromFile(const char *filePath)
    {
        if (!filePath)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject, filePath,
                     LogReasonStrings::GenvParameterError);
            return FM_ERROR(GV_ERR_INVALID_PARAM);
        }

        FontsetObject *fObj = Fonts::loadFontsetFromFile(filePath);
        if (!fObj)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvBadObject);
            return FO_ERROR(GV_ERR_BAD_OBJECT);
        }
        /*
        if (fObj.uploadTexture() == 0)
        {
            return _loadFont(&fObj);
        }
        else
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvBadObject);
            return FM_ERROR(GV_ERR_UPLOAD_FAILED);
        }
        */
        return FM_ERROR(GV_ERR_OUT_OF_MEMORY);
    }

    int FontManager::loadFontsetFromMemory(const uint8_t *data, const size_t length)
    {
        if (auto p = testParams(data != nullptr, length >= sizeof(FontsetObject)))
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject, "",
                     LogReasonStrings::GenvParameterError, p);
            return FM_ERROR(GV_ERR_INVALID_PARAM);
        }

        FontsetObject *fObj = Fonts::loadFontsetFromMem(data, length);
        if (!fObj || !fObj->validateMagic())
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvBadObject);
            return FO_ERROR(GV_ERR_BAD_OBJECT);
        }
        return _loadFontset(fObj);
    }

    FontObject *FontManager::getFont(util::Hash hash, uint8_t size, uint8_t flags)
    {
        if (!_fontListLength)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed, LogReasonStrings::GenvListIsEmpty);
            return nullptr;
        }

        for (int i = 0; i < _fontListLength; i++)
        {
            if (_fontList[i]->fontAt(_currentFont.size).getObjectID() == hash)
                return &_fontList[i]->fontAt(_currentFont.size);
        }

        LOG_FONT(LogErrorStrings::GenvRemoveItemFailed,
                 ObjectStrings::FontObject,
                 LogReasonStrings::GenvItemNotFound);
        return nullptr;
    }

    int FontManager::_getFontIndex(util::Hash fontsetId)
    {
        if (!_fontListLength)
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed, LogReasonStrings::GenvListIsEmpty);
            return -1;
        }

        for (int i = 0; i < _fontListLength; i++)
        {
            if (_fontList[i]->getHeader().id == fontsetId)
                return i;
        }

        LOG_FONT(LogErrorStrings::GenvRemoveItemFailed,
                 ObjectStrings::FontObject,
                 LogReasonStrings::GenvItemNotFound);
        return -1;
    }

    int FontManager::setFont(util::Hash fontsetId, uint8_t size, uint8_t flags)
    {
        int fontIdx = _getFontIndex(fontsetId);
        if (fontIdx >= 0)
        {
            _currentFont.id = fontsetId;
            _currentFont.index = fontIdx;
            _currentFont.size = size;

            int idx = _fontList[fontIdx]->find(size, flags);
            if (idx >= 0)
            {
                _currentFont.entry = idx;
                auto &fObj = _fontList[fontIdx]->fontAt(idx);
                size_t paramData = 0;
                if (!fObj.getParam(Textures::TextureUploaded, paramData) || paramData != true)
                {
                    fObj.uploadTexture();
                    auto toRemove = _uploadedFonts.add(fontsetId);
                    if (toRemove != 0)
                    {
                        // TODO: Unload texture from VRAM. It became too unpopular to be notable.
                    }
                    fObj.setParam(Textures::TextureUploaded, true);
                }
                else
                {
                    // The more times the font is used, the more protected it is from deletion.
                    _uploadedFonts.used(fontsetId);
                }
            }
        }
        else
        {
            LOG_FONT(LogErrorStrings::GenvLoadItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvItemNotFound,
                     fontsetId + '0');
            return FM_ERROR(GV_ERR_ITEM_NOT_FOUND);
        }
        return GV_ERR_UNSUPPORTED;
    }

    int FontManager::unloadFont(util::Hash fontId)
    {
        if (!_fontListLength)
        {
            LOG_FONT(LogErrorStrings::GenvRemoveItemFailed,
                     ObjectStrings::FontObject,
                     LogReasonStrings::GenvListIsEmpty);
            return FM_ERROR(GV_ERR_LIST_EMPTY);
        }

        // TODO: Unload fonts based on hashed name/details, remove from poplist and unload from VRAM if uploaded
        for (auto fsObj : _fontList)
        {
            if (fsObj->getHeader().id == fontId)
            {
                // size_t param = 0;
                // fsObj[_currentFont.] return;
            }
        }

        LOG_FONT(LogErrorStrings::GenvRemoveItemFailed,
                 ObjectStrings::FontObject,
                 LogReasonStrings::GenvItemNotFound);
        return FM_ERROR(GV_ERR_ITEM_NOT_FOUND);
    }
} // namespace Fonts