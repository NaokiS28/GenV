# GenV - Copyright (C) 2025 NaokiS, spicyjpeg
# CMakeLists.txt - Created on 24-11-2025
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

## Lib Bare Metal CXX

add_library(
	libbmcxx STATIC
	src/bad_alloc.cc
	src/charconv.cc
	src/ctype.c
	src/delete_a.cc
	src/delete_an.cc
	src/delete_as.cc
	src/delete_n.cc
	src/delete_r.cc
	src/delete_rs.cc
	src/exception.cc
	src/logic_error.cc
	src/memcmp.c
	src/memmove.c
	src/memset.c
	src/new_a.cc
	src/new_an.cc
	src/new_n.cc
	src/new_r.cc
	src/stpcpy.c
	src/strcat.c
	src/strcmp.c
	src/strcpy.c
	src/strlen.c
	src/strncmp.c
	src/strncmp.c
	src/strncpy.c
	src/terminate.cc
	src/to_string.cc
	src/to_wstring.cc
)

target_compile_definitions(
	libbmcxx PRIVATE
	BMCXX_DISABLE_EXCEPTIONS
)

target_link_libraries(libbmcxx PUBLIC genv_terminal)
target_include_directories(
    libbmcxx PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/include
)