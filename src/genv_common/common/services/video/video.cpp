/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gpu.hpp - Created on 27-04-2025
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

#include "video.hpp"
#include "common/services/services.hpp"

namespace Video
{    
    size_t msToFrames(size_t millis)
    {
        size_t framerate = Services::getVideo()->getRefreshRate();

        if (millis < 17)
            millis = 17;
        if (framerate < 20 || framerate > 480)
            framerate = 60;

        size_t msPerFrame = (1000 / framerate);
        return (millis / msPerFrame);
    }

    size_t frames(){
        return Services::getVideo()->getFrameCount();
    }

    uint16_t getHorizontalRes(){ return Services::getVideo()->getHorizontalRes(); }
    uint16_t getVerticalRes(){ return Services::getVideo()->getVerticalRes(); }
    uint16_t getVerticalRes(){ return Services::getVideo()->getRefreshRate(); }
}