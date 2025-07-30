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

namespace Video
{
    uint32_t IVideo::findNearestVideoMode(const VideoModeList *list, uint16_t reqW, uint16_t reqH, uint16_t reqR) const
    {
        if(list == nullptr)
            return V_RES_LIST_INVALID;
        if (list->resLength == 0 || list->resList == nullptr || list->refreshLength == 0 || list->refreshList == nullptr)
            return V_RES_LIST_INVALID;

        int bestIndex = -1;
        uint32_t bestScore = 0xFFFFFFFF; // lower score = better fit

        for (uint16_t i = 0; i < list->resLength; i++)
        {
            const VideoResolution &res = list->resList[i];

            // Rule 1: Skip if the resolution is smaller than requested (would crop)
            if (res.width < reqW || res.height < reqH)
                continue;

            // Compute how well it fits: prioritize matching exactly, then closeness
            uint16_t dw = reqW - res.width;
            uint16_t dh = reqH - res.height;
            uint32_t score = (uint32_t)dw * dw + (uint32_t)dh * dh;

            // Prefer exact match if found
            if (dw == 0 && dh == 0)
            {
                bestIndex = i;
                bestScore = 0;
                break;
            }

            if (score < bestScore)
            {
                bestScore = score;
                bestIndex = i;
            }
        }

        // If no suitable mode found under or equal to requested, pick the largest available one
        if (bestIndex == -1)
        {
            uint32_t largestArea = 0;
            for (uint16_t i = 0; i < list->resLength; i++)
            {
                const VideoResolution &res = list->resList[i];
                uint32_t area = (uint32_t)res.width * res.height;
                if (area > largestArea)
                {
                    bestIndex = i;
                    largestArea = area;
                }
            }
        }

        // Handle refresh rate similarly: pick the closest below or equal, else lowest available
        int bestRefreshIndex = -1;
        uint16_t bestRefreshDiff = 0xFFFF;
        for (uint16_t i = 0; i < list->refreshLength; i++)
        {
            uint16_t diff = (reqR >= list->refreshList[i]) ? (reqR - list->refreshList[i]) : 0xFFFF;
            if (diff < bestRefreshDiff)
            {
                bestRefreshDiff = diff;
                bestRefreshIndex = i;
            }
        }

        // If still no refresh found, pick the first
        if (bestRefreshIndex == -1 && list->refreshLength > 0)
            bestRefreshIndex = 0;

        uint32_t result = 0;
        if (bestIndex >= 0)
        {
            result |= bestIndex;
            if (list->resList[bestIndex].width != reqW || list->resList[bestIndex].height != reqH)
                result |= V_RES_MODIFIED;
        }
        else
        {
            return V_RES_UNSUPPORTED;
        }

        if (bestRefreshIndex >= 0)
        {
            result |= (bestRefreshIndex << 8);
            if (list->refreshList[bestRefreshIndex] != reqR)
                result |= V_REFRESH_MODIFIED;
        }

        return result;
    }

}