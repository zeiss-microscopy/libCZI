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

#include "stdafx.h"
#include "CppUnitTest.h"

#include "inc_libCZI.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libCZI;

namespace UnitTest
{
	TEST_CLASS(UnitTest_McComposite)
	{
	public:
		TEST_METHOD(TestMethod_McComposite_Test1)
		{
			auto bm = CBitmapData<CHeapAllocator>::Create(PixelType::Gray8, 1, 1);
			ScopedBitmapLockerSP lck{ bm };
			*((std::uint8_t*)(lck.ptrDataRoi)) = 59;
			uint8_t lut[256];
			memset(lut, 0, sizeof(lut));
			lut[59] = 88;

			auto bmDst = CBitmapData<CHeapAllocator>::Create(PixelType::Bgr24, bm->GetWidth(), bm->GetHeight());

			Compositors::ChannelInfo chinfo;
			chinfo.weight = 1;
			chinfo.enableTinting = false;
			chinfo.blackPoint = 0;
			chinfo.whitePoint = 1;
			chinfo.lookUpTableElementCount = sizeof(lut);
			chinfo.ptrLookUpTable = lut;

			IBitmapData* srcs[1];
			srcs[0] = bm.get();
			Compositors::ComposeMultiChannel_Bgr24(bmDst.get(), 1, srcs, &chinfo);

			ScopedBitmapLockerSP lckDst{ bmDst };
			uint8_t b = *((const std::uint8_t*)(lckDst.ptrDataRoi));
			uint8_t g = *(((const std::uint8_t*)(lckDst.ptrDataRoi)) + 1);
			uint8_t r = *(((const std::uint8_t*)(lckDst.ptrDataRoi)) + 2);
			Assert::IsTrue(r == 88 && g == 88 && b == 88, L"Incorrect result", LINE_INFO());
		}

		TEST_METHOD(TestMethod_McComposite_Test2)
		{
			auto bm = CBitmapData<CHeapAllocator>::Create(PixelType::Gray8, 256, 1);
			ScopedBitmapLockerSP lck{ bm };
			for (int i = 0; i < 256; ++i)
			{
				*(((std::uint8_t*)(lck.ptrDataRoi)) + i) = (uint8_t)i;
			}

			uint8_t lut[256];
			for (int i = 0; i < 256; ++i)
			{
				lut[i] = (uint8_t)(255 - i);
			}

			auto bmDst = CBitmapData<CHeapAllocator>::Create(PixelType::Bgr24, bm->GetWidth(), bm->GetHeight());

			Compositors::ChannelInfo chinfo;
			chinfo.weight = 1;
			chinfo.enableTinting = false;
			chinfo.blackPoint = 0;
			chinfo.whitePoint = 1;
			chinfo.lookUpTableElementCount = sizeof(lut);
			chinfo.ptrLookUpTable = lut;

			IBitmapData* srcs[1];
			srcs[0] = bm.get();
			Compositors::ComposeMultiChannel_Bgr24(bmDst.get(), 1, srcs, &chinfo);

			ScopedBitmapLockerSP lckDst{ bmDst };

			for (int i = 0; i < 256; ++i)
			{
				const uint8_t* ptr = ((const uint8_t*)lckDst.ptrDataRoi) + (i * 3);
				uint8_t b = ptr[0];
				uint8_t g = ptr[1];
				uint8_t r = ptr[2];
				uint8_t sb = (uint8_t)(255 - i);
				Assert::IsTrue(r == sb && g == sb && b == sb, L"Incorrect result", LINE_INFO());
			}
		}

		TEST_METHOD(TestMethod_McComposite_Test3)
		{
			auto bm = CBitmapData<CHeapAllocator>::Create(PixelType::Gray16, 256 * 256, 1);
			ScopedBitmapLockerSP lck{ bm };
			for (int i = 0; i < 256 * 256; ++i)
			{
				*(((std::uint16_t*)(lck.ptrDataRoi)) + i) = (uint16_t)i;
			}

			uint8_t lut[256 * 256];
			for (int i = 0; i < 256 * 256; ++i)
			{
				lut[i] = (uint8_t)(i & 255);
			}

			auto bmDst = CBitmapData<CHeapAllocator>::Create(PixelType::Bgr24, bm->GetWidth(), bm->GetHeight());

			Compositors::ChannelInfo chinfo;
			chinfo.weight = 1;
			chinfo.enableTinting = false;
			chinfo.blackPoint = 0;
			chinfo.whitePoint = 1;
			chinfo.lookUpTableElementCount = sizeof(lut);
			chinfo.ptrLookUpTable = lut;

			IBitmapData* srcs[1];
			srcs[0] = bm.get();
			Compositors::ComposeMultiChannel_Bgr24(bmDst.get(), 1, srcs, &chinfo);

			ScopedBitmapLockerSP lckDst{ bmDst };

			for (int i = 0; i < 256 * 256; ++i)
			{
				const uint8_t* ptr = ((const uint8_t*)lckDst.ptrDataRoi) + (i * 3);
				uint8_t b = ptr[0];
				uint8_t g = ptr[1];
				uint8_t r = ptr[2];
				uint8_t sb = (uint8_t)(i & 255);
				Assert::IsTrue(r == sb && g == sb && b == sb, L"Incorrect result", LINE_INFO());
			}
		}
	};
}