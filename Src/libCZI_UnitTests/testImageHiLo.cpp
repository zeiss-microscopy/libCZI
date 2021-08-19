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

#include "pch.h"
#include "testImage.h"

/*static*/ const void* CTestImage::GetZStd1CompressedImageWithHiLoPacking(size_t* size, int* width, int* height)
{
		static const unsigned char rawData[70] = {
			0x03, 0x01, 0x01, 0x28, 0xB5, 0x2F, 0xFD, 0x60, 0x00, 0x07, 0xCD, 0x01,
			0x00, 0x70, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
			0xFF, 0xFF, 0x00, 0x00, 0x11, 0x00, 0xF7, 0x73, 0x05, 0x80, 0x21, 0xB0,
			0xE2, 0x82, 0x04, 0x00, 0x83, 0x0B, 0x12, 0x00, 0x0C, 0x2E, 0x48, 0x00,
			0x30, 0xB8, 0x20, 0x01, 0xC0, 0xE0, 0x42, 0x82, 0x80, 0x85, 0x81, 0x0C,
			0x29, 0xD5, 0x3F, 0x24, 0x80, 0x15, 0xD4, 0xA5, 0x85, 0x0B
		};
		if (size != nullptr)
		{
				*size = sizeof(rawData);
		}

		if (width != nullptr)
		{
				*width = 32;
		}

		if (height != nullptr)
		{
				*height = 32;
		}

		return rawData;
}
