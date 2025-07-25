# GenV - Copyright (C) 2025 NaokiS, spicyjpeg
# mingw32.cmake - Created on 22-04-2025
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

# Toolchain for i686-w64-mingw32
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR i686)

# Adjust the path if needed
set(CMAKE_C_COMPILER "i686-w64-mingw32-gcc" CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER "i686-w64-mingw32-g++" CACHE STRING "" FORCE)
