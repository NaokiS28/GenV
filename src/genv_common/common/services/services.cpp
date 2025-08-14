/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * services.cpp - Created on 09-05-2025
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

#include "services.hpp"
#include "common/logger/log.hpp"

#include "hardware.hpp"

// Static member definitions
Audio::IAudio *Services::s_audio = nullptr;
Video::IVideo *Services::s_video = nullptr;
Input::IInput *Services::s_input = nullptr;
Files::IStorage *Services::s_storage = nullptr;
System::ISystem *Services::s_system = nullptr;

void Services::setVideo(Video::IVideo *video)
{
    if (!video)
        return;

    if (s_video)
    {
        s_video->shutdown();
        delete s_video;
    }

    s_video = video;
}

void Services::setAudio(Audio::IAudio *audio)
{
    if (!audio)
        return;

    if (s_audio)
    {
        s_audio->shutdown();
        delete s_audio;
    }

    s_audio = audio;
}

void Services::setInput(Input::IInput *input)
{
    if (!input)
        return;

    if (s_input)
    {
        s_input->shutdown();
        delete s_input;
    }

    s_input = input;
}

void Services::setStorage(Files::IStorage *storage)
{
    if (!storage)
        return;

    if (s_storage)
    {
        s_storage->shutdown();
        delete s_storage;
    }

    s_storage = storage;
}

void Services::destroySystem()
{
    if (!s_system)
        return;
    s_system->shutdown();
    delete s_system;
    s_system = nullptr;
}

void Services::destroyVideo()
{
    if (!s_video)
        return;
    s_video->shutdown();
    delete s_video;
    s_video = nullptr;
}

void Services::destroyAudio()
{
    if (!s_audio)
        return;
    s_audio->shutdown();
    delete s_audio;
    s_audio = nullptr;
}

void Services::destroyInput()
{
    if (!s_input)
        return;
    s_input->shutdown();
    delete s_input;
    s_input = nullptr;
}

void Services::destroyStorage()
{
    if (!s_storage)
        return;
    s_storage->shutdown();
    delete s_storage;
    s_storage = nullptr;
}