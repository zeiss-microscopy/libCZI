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

#include "inc_libCZI.h"

std::shared_ptr<libCZI::IBitmapData> CreateTestBitmap(libCZI::PixelType pixeltype, std::uint32_t width, std::uint32_t height);

template<typename input_iterator>
void CalcHash(std::uint8_t* ptrHash, input_iterator begin, input_iterator end)
{
	memset(ptrHash, 0, 16);
	for (input_iterator it = begin; it != end; ++it)
	{
		std::uint8_t hash[16];
		libCZI::Utils::CalcMd5SumHash(it->c_str(), it->length(), hash, 16);
		for (uint8_t i = 0; i < 16; ++i)
		{
			*(ptrHash + i) ^= hash[i];
		}
	}
}

