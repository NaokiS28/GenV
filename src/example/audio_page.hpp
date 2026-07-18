/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * input_page.hpp - Created on 02-05-2026
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

#pragma once

#include "demo_page.hpp"

#include "common/services/io/player.hpp"
#include <genv.hpp>

using namespace Apps;

class GenV_Audio : public DemoPage
{
private:
    static constexpr AppInfo appInfo = makeAppInfo(
        "GenV Audio Test",  // name
        "NaokisRC",         // maker
        AppVersion(0, 0, 1) // version
    );

public:
    const AppInfo &info() const override
    {
        return appInfo;
    }

    int init() override;
    int update() override;
    void render() override;
    void reload() override;
};

extern GenV_Audio genv_demo_audioPage;