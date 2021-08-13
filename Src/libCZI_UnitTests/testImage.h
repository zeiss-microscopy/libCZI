//******************************************************************************
// 
// libCZI is a reader for the CZI fileformat written in C++
// Copyright (C) 2017  Zeiss Microscopy GmbH
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

#include <cstdint>

class CTestImage
{
public:
		static const std::uint32_t BGR24TESTIMAGE_WIDTH = 1024;
		static const std::uint32_t BGR24TESTIMAGE_HEIGHT = 768;
		static void CopyBgr24Image(void* pDest, std::uint32_t width, std::uint32_t height, int stride);

		static const void* GetJpgXrCompressedImage(size_t* size, int* width, int* height);
		static const void* GetZStd1CompressedImage(size_t* size, int* width, int* height);
		static const void* GetZStd1CompressedImageWithHiLoPacking(size_t* size, int* width, int* height);
};

