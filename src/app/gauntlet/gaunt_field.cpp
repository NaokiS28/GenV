/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gauntlet_field.cpp - Created on 18-06-2025
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

#include "gaunt_field.hpp"
#include "common/formats/soundfile.hpp"
#include "common/services/services.hpp"

namespace Gauntlet
{
    const char *floorArtPath = "C:\\Users\\Naoki\\Nextcloud\\Programming\\GenV\\bin\\gauntlet\\floor1.png";
    const char *wallArtPath = "C:\\Users\\Naoki\\Nextcloud\\Programming\\GenV\\bin\\gauntlet\\wall1.png";
    const char *musicPath = "C:\\Users\\Naoki\\Nextcloud\\Programming\\GenV\\bin\\gauntlet\\music1.wav";

    GauntletField::GauntletField()
    {
        map = new Gaunt_Map();
        if (map != nullptr)
            map->generateFloor();
    }

    GauntletField::~GauntletField()
    {
    }

    int GauntletField::init()
    {
        area = {
            0, 0,
            gpu->getHorizontalRes(),
            gpu->getVerticalRes()};
        setAppState(Apps::APP_STATE_RUN);
        appReady = true;
        return 0;
    }

    void GauntletField::update()
    {
    }

    void GauntletField::render()
    {
        gpu->fillScreen(Colors::Black);
        floorArt->repeat(0, 0, 16, 16);
        int x = 0, y = 0;
        while (map->getNextObject(Gaunt_Objects::Wall, x, y).obj != Gaunt_Objects::Invalid)
        {
            wallArt->draw(x++, y);
        }
    }

    void GauntletField::loadApp()
    {
        if (floorArt == nullptr)
        {
            floorArt = new Gaunt_FloorTile(floorArtPath);
            if (floorArt != nullptr)
            {
                floorArt->uploadTexture();
                floorArt->setTileMeta(8, 8, 6, 8).useTileArray(map->getFloorMap(), Gaunt_MapSizeW, Gaunt_MapSizeH).scale(250);
            }
        }
        else if (wallArt == nullptr)
        {
            screenLoadProgrees = 33;
            wallArt = new Gaunt_WallTile(wallArtPath);
            if (wallArt != nullptr)
            {
                wallArt->uploadTexture();
                wallArt->setMapObj(map).setTileMeta(8, 8, 4, 8).scale(250);
            }
        }
        else if (fieldMusic == nullptr)
        {
            screenLoadProgrees = 66;
            fieldMusic = Audio::openAudioFile(musicPath, 66);
            if (fieldMusic != nullptr)
            {
                fieldMusic->uploadSample();
            }
        }
        else
        {
            Application::state = APP_STATE_INIT;
            screenLoadProgrees = 100;
        }
    }
} // namespace Gauntlet
