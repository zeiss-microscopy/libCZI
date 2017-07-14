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
#include "testImage.h"

#include "inc_libCZI.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libCZI;

namespace UnitTest
{
	TEST_CLASS(UnitTest_BitmapOperations)
	{
	public:
		TEST_METHOD(TestMethod_NNResize1)
		{
			static const std::uint8_t expectedResult[16] = { 0xf2,0xa1,0x4b,0x86,0xa5,0x0e, 0xdb,0x95,0xed,0x8a,0x67,0x6c,0x69,0xab,0x2b,0x26 };

			auto srcBm = CreateTestImage();
			auto bm = CBitmapData<CHeapAllocator>::Create(srcBm->GetPixelType(), 100, 100);
			CBitmapOperations::NNResize(srcBm.get(), bm.get());
			std::uint8_t hash[16];
			CBitmapOperations::CalcMd5Sum(bm.get(), hash, sizeof(hash));
			int c = memcmp(expectedResult, hash, sizeof(hash));
			Assert::IsTrue(c==0, L"Incorrect result", LINE_INFO());
		}

	private:
		static std::shared_ptr<IBitmapData> CreateTestImage()
		{
			auto bm = CBitmapData<CHeapAllocator>::Create(PixelType::Bgr24, CTestImage::BGR24TESTIMAGE_WIDTH, CTestImage::BGR24TESTIMAGE_HEIGHT);
			ScopedBitmapLockerSP lck{ bm };
			CTestImage::CopyBgr24Image(lck.ptrDataRoi, bm->GetWidth(), bm->GetHeight(), lck.stride);
			return bm;
		}
	};
}