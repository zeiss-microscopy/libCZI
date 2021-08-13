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

#include "pch.h"
#include "utils.h"

using namespace std;
using namespace libCZI;

class CMemBitmapWrapper : public libCZI::IBitmapData
{
private:
	void* ptrData;
	libCZI::PixelType pixeltype;
	std::uint32_t width;
	std::uint32_t height;
	std::uint32_t stride;
public:
	CMemBitmapWrapper(libCZI::PixelType pixeltype, std::uint32_t width, std::uint32_t height) :pixeltype(pixeltype), width(width), height(height)
	{
		int bytesPerPel;
		switch (pixeltype)
		{
		case PixelType::Bgr24:bytesPerPel = 3; break;
		case PixelType::Bgr48:bytesPerPel = 6; break;
		case PixelType::Gray8:bytesPerPel = 1; break;
		case PixelType::Gray16:bytesPerPel = 2; break;
		case PixelType::Gray32Float:bytesPerPel = 4; break;
		default: throw runtime_error("unsupported pixeltype");
		}

		if (pixeltype == PixelType::Bgr24)
		{
			this->stride = ((width * bytesPerPel + 3) / 4) * 4;
		}
		else
		{
			this->stride = width * bytesPerPel;
		}

		size_t s = this->stride * height;
		this->ptrData = malloc(s);
	}

	virtual ~CMemBitmapWrapper()
	{
		free(this->ptrData);
	};

	virtual libCZI::PixelType GetPixelType() const
	{
		return this->pixeltype;
	}

	virtual libCZI::IntSize	GetSize() const
	{
		return libCZI::IntSize{ this->width, this->height };
	}

	virtual libCZI::BitmapLockInfo	Lock()
	{
		libCZI::BitmapLockInfo bitmapLockInfo;
		bitmapLockInfo.ptrData = this->ptrData;
		bitmapLockInfo.ptrDataRoi = this->ptrData;
		bitmapLockInfo.stride = this->stride;
		bitmapLockInfo.size = this->stride * this->height;
		return bitmapLockInfo;
	}

	virtual void Unlock()
	{
	}
};

std::shared_ptr<libCZI::IBitmapData> CreateTestBitmap(libCZI::PixelType pixeltype, std::uint32_t width, std::uint32_t height)
{
	auto bm = make_shared<CMemBitmapWrapper>(pixeltype, width, height);
	ScopedBitmapLockerSP lckBm{ bm };
	switch (bm->GetPixelType())
	{
	case PixelType::Gray8:
	case PixelType::Gray16:
	case PixelType::Bgr24:
	case PixelType::Bgr48:
	{
		uint8_t v = 0;
		for (std::uint64_t i = 0; i < lckBm.size; ++i)
		{
			((uint8_t*)lckBm.ptrDataRoi)[i] = v++;
		}

		break;
	}

	default:
		throw  std::runtime_error("Not Implemented");
	}

	return bm;
}
