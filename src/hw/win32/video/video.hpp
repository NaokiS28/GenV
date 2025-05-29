/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * video.hpp - Created on 26-04-2025
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

#pragma once
#include <windows.h>
#include "common/services/video/video.hpp"
#include "hw/win32/window-mgr.hpp"

using namespace Video;

class WinVideo;

class IWinVideo : public IVideo
{
public:
    virtual void setWindow(WindowObject *hWnd) = 0;
};

class WinVideo : public IWinVideo
{
private:
    IWinVideo *gpuDriver = nullptr;

public:
    WinVideo() {}
    ~WinVideo();

    bool init() override;
    inline bool reset() override { return gpuDriver ? gpuDriver->reset() : false; }
    inline bool beginRender() override { return gpuDriver ? gpuDriver->beginRender() : false; }
    inline bool endRender() override { return gpuDriver ? gpuDriver->endRender() : false; }
    inline bool shutdown() override { return 0; } //return gpuDriver ? gpuDriver->shutdown() : false; }
    inline void setWindow(WindowObject *wObj)
    {
        if (gpuDriver)
            gpuDriver->setWindow(wObj);
    }

    inline int getHorizontalRes() { return gpuDriver ? gpuDriver->getHorizontalRes(): false; }
    inline int getVerticalRes() { return gpuDriver ? gpuDriver->getVerticalRes(): false; }
    inline int getRefreshRate() { return gpuDriver ? gpuDriver->getRefreshRate(): false; }
    inline Ratio getAspectRatio() { return gpuDriver ? gpuDriver->getAspectRatio() : Ratio(0, 0); }
    inline const char *getVideoModeName() { return gpuDriver ? gpuDriver->getVideoModeName() : nullptr; }
    inline const char *getMonitorName() { return gpuDriver ? gpuDriver->getMonitorName() : nullptr; }
    inline int getDPI() { return gpuDriver ? gpuDriver->getDPI(): false; }
    inline virtual void getMonitorInfo(Monitor &m) const { if(gpuDriver) gpuDriver->getMonitorInfo(m); }

    IVideo *getVideoService() { return gpuDriver; }

    inline int getSupportedResolutions(VideoModeList &list){
        return gpuDriver ? gpuDriver->getSupportedResolutions(list) : false;
    }

    inline int setResolution(int w, int h, bool updateWindow = true) override
    {
        return gpuDriver ? gpuDriver->setResolution(w, h, updateWindow) : false;
    }

    inline bool setFullscreen(FullscreenMode mode, int w = 0, int h = 0) override
    {
        return gpuDriver ? gpuDriver->setFullscreen(mode, w, h) : false;
    }

    inline FullscreenMode getFullscreenMode()
    {
        return gpuDriver ? gpuDriver->getFullscreenMode() : Video::Windowed;
    }

    inline void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const override
    {
        gpuDriver->drawAlpha(x, y, w, h, sx, sy, a);
    };

    inline void drawLine(int x1, int y1, int x2, int y2, int width, Color color) override
    {
        gpuDriver->drawLine(x1, y1, x2, y2, width, color);
    }

    inline void drawRect(int x, int y, int w, int h, Color c) override
    {
        gpuDriver->drawRect(x, y, w, h, c);
    }
    inline void drawGradientRectH(int x, int y, int w, int h, Color left, Color right) override
    {
        gpuDriver->drawGradientRectH(x, y, w, h, left, right);
    }
    void drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom) override
    {
        gpuDriver->drawGradientRectV(x, y, w, h, top, bottom);
    }
    void drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom) override
    {
        gpuDriver->drawGradientRectD(x, y, w, h, top, middle, bottom);
    }

    void drawGradientRect(int x, int y, int w, int h, GPUGradientMode m) override
    {
        gpuDriver->drawGradientRect(x, y, w, h, m);
    }
    void drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) override
    {
        gpuDriver->drawGradientRectHVar(x, y, w, h, left, right, startPoint, endPoint);
    }
    void drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) override
    {
        gpuDriver->drawGradientRectVVar(x, y, w, h, top, bottom, startPoint, endPoint);
    }

    void drawText(const char *str, int len, int x, int y, int w, int h, Color color) override
    {
        gpuDriver->drawText(str, len, x, y, w, h, color);
    }
};