/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * gpu-d3d.hpp - Created on 24-04-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

// Not thread safe, but probably not *too* much of a problem.
#pragma once

#include "video.hpp"

class SDLGPU : public IWinVideo
{
private:

public:

    bool init() override;
    bool reset() override { return false; }
    bool beginRender() override;
    bool endRender() override;
    bool shutdown() override { return 0; }
    void setWindow(WindowObject *wObj) override {}

    int getSupportedResolutions(VideoModeList &list){}

    int setResolution(int w, int h, bool updateWindow = true){ 
        return 0;
    }

    bool setFullscreen(FullscreenMode mode) override {
        return true;
    }

    void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const { 
    };

    void drawLine(int x1, int y1, int x2, int y2, int width, Color color){
        
    }

    void drawRect(int x, int y, int w, int h, Color c) {
    }
    void drawGradientRectH(int x, int y, int w, int h, Color left, Color right) {
    }
    void drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom) {
    }
    void drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom) {
    }

    void drawGradientRect(int x, int y, int w, int h, GPUGradientMode m) {
    }
    void drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) {
    }
    void drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) {
    }
    void drawText(const char* str, int len, int x, int y, int w, int h, Color color){

    }

};
