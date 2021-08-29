//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017-2021  Zeiss Microscopy GmbH
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// To obtain a commercial version please contact Zeiss Microscopy GmbH.
// 
//******************************************************************************

#pragma once

// If building with CMake, there will be a file "libCZI_Config.h" created/configured by
// CMake. In order to enable building with the .sln-file in VisualStudio we use a somewhat
// lame approach - we use some preprocessor define in order to use definitions which are
// static (not generated) here.

#if defined(VISUALSTUDIO_PROJECT_BUILD)

#define LIBCZICONFIG_STRINGIFY(s) #s

#define LIBCZI_VERSION_MAJOR 0
#define LIBCZI_VERSION_MINOR 40

#define LIBCZI_ISBIGENDIANHOST 0
#define LIBCZI_HAVE_ENDIAN_H 0

#define LIBCZI_CXX_COMPILER_IDENTIFICATION  LIBCZICONFIG_STRINGIFY(_MSC_FULL_VER)

#else

#include <libCZI_Config.h>

#endif
