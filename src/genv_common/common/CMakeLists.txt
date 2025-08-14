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
	formats/soundfile.cpp
	formats/riff.cpp
	formats/wav.cpp
)
target_link_libraries(genv_common PUBLIC genv_services)
target_include_directories(genv_common PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/genv_hw
)


add_library(genv_util STATIC
	util/hash.cpp
	util/ifloat.cpp
	util/misc.cpp
	util/text.cpp
	util/tween.cpp
)
target_include_directories(genv_util PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/genv_hw
)

add_library(genv_services STATIC
	services/genv_sys.cpp
	services/perfmon.cpp
	services/adminkey.cpp
	services/services.cpp
	services/storage/storage.cpp
	services/storage/nullstorage.cpp
	services/video/video.cpp
	services/video/nullvideo.cpp
	services/audio/nullaudio.cpp
	services/io/nullinput.cpp
	services/system/timer.cpp
	services/system/system.cpp
	services/system/arcade/arcade.cpp
	services/system/arcade/nvram.cpp
	${CMAKE_SOURCE_DIR}/src/genv_hw/hardware.cpp
)
target_link_libraries(genv_services PUBLIC genv_hw_${GENV_PLATFORM} genv_logger)
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
