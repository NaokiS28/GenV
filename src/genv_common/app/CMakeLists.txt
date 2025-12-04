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

add_subdirectory(builtin)

add_library(genv_app_core STATIC
	app.cpp
)
target_link_libraries(genv_app_core PUBLIC genv_common genv_services genv_logger) 
target_include_directories(genv_app_core PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/app
	${LIBC_INCLUDE}

)

add_library(genv_app_manager STATIC
	appmgr.cpp
)
target_link_libraries(genv_app_manager PUBLIC game_code genv_services genv_builtin_apps genv_app_core genv_logger) 
target_include_directories(genv_app_manager PUBLIC
    ${CMAKE_SOURCE_DIR}/src/genv_common
    ${CMAKE_SOURCE_DIR}/src/app
	${LIBC_INCLUDE}

)