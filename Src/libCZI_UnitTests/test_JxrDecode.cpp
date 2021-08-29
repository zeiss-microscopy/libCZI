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
#include "inc_libCZI.h"
#include "testImage.h"
#include "../libCZI/decoder.h"

using namespace libCZI;
using namespace std;

TEST(JxrDecode, Decode1)
{
		auto dec = CJxrLibDecoder::Create();
		size_t sizeEncoded; int expectedWidth, expectedHeight;
		auto ptrEncodedData = CTestImage::GetJpgXrCompressedImage(&sizeEncoded, &expectedWidth, &expectedHeight);
		auto bmDecoded = dec->Decode(ptrEncodedData, sizeEncoded, libCZI::PixelType::Bgr24, expectedWidth, expectedHeight);
		EXPECT_EQ((uint32_t)expectedWidth, bmDecoded->GetWidth()) << "Width is expected to be equal";
		EXPECT_EQ((uint32_t)expectedHeight, bmDecoded->GetHeight()) << "Height is expected to be equal";
		EXPECT_EQ(bmDecoded->GetPixelType(), PixelType::Bgr24) << "Not the correct pixeltype.";

		uint8_t hash[16] = { 0 };
		Utils::CalcMd5SumHash(bmDecoded.get(), hash, sizeof(hash));

		static const uint8_t expectedResult[16] = { 0x04,0x77,0x2f,0x32,0x2f,0x94,0x9b,0x07,0x0d,0x53,0xa5,0x24,0xea,0x64,0x5a,0x1a };
		EXPECT_TRUE(memcmp(hash, expectedResult, 16) == 0) << "Incorrect result";
}
