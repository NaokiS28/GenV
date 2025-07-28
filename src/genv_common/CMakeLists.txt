# GenV - Copyright (C) 2025 NaokiS, spicyjpeg
# CMakeLists.txt - Created on 21-04-2025
# 
# GenV is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
# 
# GenV is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with
# GenV. If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.25)

project(genv_common LANGUAGES C CXX)

# GenV shared core headers
add_library(genv_common STATIC
	common/util/hash.cpp
	common/util/ifloat.cpp
	common/util/log.cpp
	common/util/misc.cpp
	common/util/text.cpp
	common/util/tween.cpp
	common/objects/file.cpp
	common/objects/sound.cpp
	common/objects/tile.cpp
	common/objects/sprite.cpp
	common/objects/texture.cpp
	common/objects/texture/missingtex.c
	common/formats/soundfile.cpp
	common/formats/riff.cpp
	common/formats/wav.cpp
	common/services/services.cpp
	common/services/storage.cpp
	common/services/appmgr.cpp
	common/services/sys/timer.cpp
	app/app.cpp
	app/builtin/errorscr/errorscr.cpp
	app/builtin/loadscr/loadscr.cpp
	app/builtin/textload/textload.cpp	
	app/builtin/tmss/tmss.cpp	
)

target_include_directories(genv_common PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/genv_hw
)