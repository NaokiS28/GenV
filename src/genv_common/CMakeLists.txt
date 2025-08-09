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

add_library(genv_objects STATIC
	common/objects/object.cpp
	common/objects/file.cpp
	common/objects/sound.cpp
	common/objects/tile.cpp
	common/objects/sprite.cpp
	common/objects/texture.cpp
	common/objects/texture/missingtex.c
)

add_library(genv_formats STATIC
	common/formats/soundfile.cpp
	common/formats/riff.cpp
	common/formats/wav.cpp
)

# GenV shared core headers
add_library(genv_common STATIC
	common/util/hash.cpp
	common/util/ifloat.cpp
	common/util/log.cpp
	common/util/misc.cpp
	common/util/text.cpp
	common/util/tween.cpp
	common/services/genv_sys.cpp
	common/services/adminkey.cpp
	common/services/services.cpp
	common/services/storage/storage.cpp
	common/services/storage/nullstorage.cpp
	common/services/video/video.cpp
	common/services/video/nullvideo.cpp
	common/services/audio/nullaudio.cpp
	common/services/io/nullinput.cpp
	common/services/appmgr.cpp
	common/services/system/timer.cpp
	common/services/system/system.cpp
	common/services/system/arcade/arcade.cpp
	common/services/system/arcade/nvram.cpp
)

add_library(genv_app_core STATIC
	app/app.cpp
	app/builtin/errorscr/errorscr.cpp
	app/builtin/loadscr/loadscr.cpp
	app/builtin/textload/textload.cpp	
	app/builtin/tmss/tmss.cpp	
	app/builtin/arcade/testmode.cpp	
)

target_link_libraries(genv_common PUBLIC genv_app_core genv_hw_common)
target_include_directories(genv_common PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/genv_hw
)

target_link_libraries(genv_objects PUBLIC genv_common) 
target_include_directories(genv_objects PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
)

target_link_libraries(genv_formats PUBLIC genv_common) 
target_include_directories(genv_formats PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
)

target_link_libraries(genv_app_core PUBLIC genv_common genv_objects genv_formats) 
target_include_directories(genv_app_core PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/app
)