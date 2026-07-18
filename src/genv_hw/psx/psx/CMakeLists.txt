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

add_subdirectory(../common ../common)

add_library(genv_hw_psx STATIC)

target_sources(genv_hw_psx PUBLIC
    system.cpp
	psx_cdrom.cpp
)

target_compile_definitions(
	genv_hw_psx PRIVATE
	GENV_PS1_VRAM_SIZE=1	# I know this is a problem as the 573 overwrites it or tries to.
)

target_include_directories(
    genv_hw_psx PUBLIC
	${CMAKE_SOURCE_DIR}/src/hw/psx/psx
	${CMAKE_SOURCE_DIR}/src/genv_common
	${LIBC_INCLUDE}
)

target_link_libraries(genv_hw_psx PRIVATE genv_psx_core genv_common genv_util genv_terminal genv_halt_screen genv_font_glyph)