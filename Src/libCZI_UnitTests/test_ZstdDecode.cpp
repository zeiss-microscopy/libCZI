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
#include "../libCZI/decoder_zstd.h"

using namespace libCZI;
using namespace std;

TEST(ZstdDecode, Decode1)
{
		auto dec = CZstd1Decoder::Create();
		size_t sizeEncoded; int expectedWidth, expectedHeight;
		auto ptrEncodedData = CTestImage::GetZStd1CompressedImage(&sizeEncoded, &expectedWidth, &expectedHeight);
		auto bmDecoded = dec->Decode(ptrEncodedData, sizeEncoded, PixelType::Gray8, 1024, 1024);
		EXPECT_EQ((uint32_t)expectedWidth, bmDecoded->GetWidth()) << "Width is expected to be equal";
		EXPECT_EQ((uint32_t)expectedHeight, bmDecoded->GetHeight()) << "Height is expected to be equal";
		EXPECT_EQ(bmDecoded->GetPixelType(), PixelType::Gray8) << "Not the correct pixeltype.";

		uint8_t hash[16] = { 0 };
		Utils::CalcMd5SumHash(bmDecoded.get(), hash, sizeof(hash));

		static const uint8_t expectedResult[16] = { 0xCF, 0x93, 0xFC, 0xB8, 0xEB, 0x24, 0xA1, 0x6F, 0xAE, 0x76, 0xA6, 0x26, 0xB2, 0x70, 0x95, 0x16 };
		EXPECT_TRUE(memcmp(hash, expectedResult, 16) == 0) << "Incorrect result";
}

TEST(ZstdDecode, TryDecodeInvalidData)
{
		auto dec = CZstd1Decoder::Create();
		static const uint8_t invalidData[] = { 10,20,30,40,50,60,70,80,90,100 };
		EXPECT_THROW(
				auto bmDecoded = dec->Decode(invalidData, sizeof(invalidData), libCZI::PixelType::Gray8, 10, 10),
				exception);
}

TEST(ZstdDecode, DecodeAndHiLoBytePacking)
{
		auto dec = CZstd1Decoder::Create();
		size_t sizeEncoded; int expectedWidth, expectedHeight;
		auto ptrEncodedData = CTestImage::GetZStd1CompressedImageWithHiLoPacking(&sizeEncoded, &expectedWidth, &expectedHeight);
		auto bmDecoded = dec->Decode(ptrEncodedData, sizeEncoded, PixelType::Gray16, 32, 32);
		EXPECT_EQ((uint32_t)expectedWidth, bmDecoded->GetWidth()) << "Width is expected to be equal";
		EXPECT_EQ((uint32_t)expectedHeight, bmDecoded->GetHeight()) << "Height is expected to be equal";
		EXPECT_EQ(bmDecoded->GetPixelType(), PixelType::Gray16) << "Not the correct pixeltype.";

		uint8_t hash[16] = { 0 };
		Utils::CalcMd5SumHash(bmDecoded.get(), hash, sizeof(hash));

		static const uint8_t expectedResult[16] = { 0x9b, 0xe3, 0x65, 0x99, 0x9a, 0xce, 0xcc, 0xb8, 0xa7, 0xc6, 0x2f, 0x0b, 0xa4, 0xd9, 0xce, 0x90 };
		EXPECT_TRUE(memcmp(hash, expectedResult, 16) == 0) << "Incorrect result";
}
