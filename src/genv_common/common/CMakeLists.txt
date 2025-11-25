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

add_library(genv_common STATIC
	objects/object.cpp
	objects/file.cpp
	objects/sound.cpp
	objects/tile.cpp
	objects/sprite.cpp
	objects/texture.cpp
	objects/texture/missingtex.c
	objects/data.cpp
	objects/font.cpp
	formats/sound_file.cpp
	formats/image_file.cpp
	formats/riff.cpp
	formats/wav.cpp
)
target_link_libraries(genv_common PUBLIC genv_services genv_util)
target_include_directories(genv_common PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/genv_hw
)


add_library(genv_util STATIC
	util/hash.cpp
	util/ifloat.cpp
	util/misc.cpp
	util/tween.cpp
	util/time.cpp
	util/date.cpp
	util/string.cpp
	vendor/lodepng.cpp
	vendor/gifn/gifn.cpp
	#vendor/bmp/bmp.c
)
target_include_directories(genv_util PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/genv_hw
)

add_library(genv_services STATIC
	services/genv_sys.cpp
	services/perfmon.cpp
	services/services.cpp
	services/storage/storage.cpp
	services/storage/storeman.cpp
	services/video/fontman.cpp
	services/video/video.cpp
	services/video/nullvideo.cpp
	services/audio/nullaudio.cpp
	services/io/inputman.cpp
	services/system/timer.cpp
	services/system/system.cpp
	services/system/rtc/soft_rtc.cpp
	services/system/arcade/arcade.cpp
	services/system/arcade/nvram.cpp
	${CMAKE_SOURCE_DIR}/src/genv_hw/hardware.cpp
)
target_link_libraries(genv_services PUBLIC genv_hw_${GENV_SUBPLATFORM} genv_logger genv_builtin_fonts)
target_include_directories(genv_services PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/genv_hw
)

add_library(genv_logger STATIC
	logger/log.cpp	
)
target_link_libraries(genv_logger PUBLIC genv_terminal)
target_include_directories(genv_logger PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
)

string(TIMESTAMP BUILD_DATE "%d-%m-%Y")
string(TIMESTAMP BUILD_TIME %H%M%S)
string(TIMESTAMP BUILD_DAY %m%d)
set(BUILD_NUM "${PROJECT_VERSION}.${BUILD_DAY}${BUILD_TIME}")

target_compile_definitions(
	genv_services PRIVATE
	GENV_BUILD="${BUILD_NUM}"
    GENV_BDATE="${BUILD_DATE}"
    GENV_VERSION="${PROJECT_VERSION}"
)
