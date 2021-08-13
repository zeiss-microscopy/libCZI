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

#include "stdafx.h"
#include "decoder_zstd.h"
#include <zstd.h>
#include <common/zstd_errors.h>
#include "bitmapData.h"
#include "libCZI_Utilities.h"

using namespace std;
using namespace libCZI;

struct ZStd1HeaderParsingResult
{
		/// Size of the header in bytes. If this is zero, the header did not parse correctly.
		size_t headerSize;

        /// A boolean indicating whether a hi-lo-byte packing is to be done.
		bool hiLoByteUnpackPreprocessing;
};

static bool LoHiBytePack_C(const void* ptrSrc, void* ptrDst, size_t length);
static ZStd1HeaderParsingResult ParseZStd1Header(const void* ptrData, size_t size);
static shared_ptr<libCZI::IBitmapData> DecodeAndProcess(const void* pData, size_t size, libCZI::PixelType pixelType, uint32_t width, uint32_t height, bool doHiLoByteUnpacking);
static shared_ptr<libCZI::IBitmapData> DecodeAndProcessNoHiLoByteUnpacking(const void* ptrData, size_t size, libCZI::PixelType pixelType, uint32_t width, uint32_t height);
static shared_ptr<libCZI::IBitmapData> DecodeAndProcessWithHiLoByteUnpacking(const void* ptrData, size_t size, libCZI::PixelType pixelType, uint32_t width, uint32_t height);
static void ZstdDecompressAndThrowIfError(const void* ptrData, size_t size, void* ptrDst, size_t dstSize);

/*static*/std::shared_ptr<CZstd0Decoder> CZstd0Decoder::Create()
{
		return make_shared<CZstd0Decoder>();
}

/*virtual*/std::shared_ptr<libCZI::IBitmapData> CZstd0Decoder::Decode(const void* ptrData, size_t size, libCZI::PixelType pixelType, uint32_t width, uint32_t height)
{
		return DecodeAndProcessNoHiLoByteUnpacking(ptrData, size, pixelType, width, height);
}

/*static*/std::shared_ptr<CZstd1Decoder> CZstd1Decoder::Create()
{
		return make_shared<CZstd1Decoder>();
}

/*virtual*/std::shared_ptr<libCZI::IBitmapData> CZstd1Decoder::Decode(const void* ptrData, size_t size, libCZI::PixelType pixelType, uint32_t width, std::uint32_t height)
{
		ZStd1HeaderParsingResult zStd1Header = ParseZStd1Header(ptrData, size);
		if (zStd1Header.headerSize == 0)
		{
				if (GetSite()->IsEnabled(LOGLEVEL_ERROR))
				{
						GetSite()->Log(LOGLEVEL_ERROR, "Exception 'runtime_error' caught from ZStd1-decoder-invocation -> \"Error in ZSTD1 header\".");
				}

				throw std::runtime_error("The zstd1-header is invalid.");
		}

		if (zStd1Header.headerSize >= size)
		{
				throw std::runtime_error("Zstd1-compressed data is invalid.");
		}

		return DecodeAndProcess(((char*)ptrData) + zStd1Header.headerSize, size - zStd1Header.headerSize, pixelType, width, height, zStd1Header.hiLoByteUnpackPreprocessing);
}

ZStd1HeaderParsingResult ParseZStd1Header(const void* ptrData, size_t size)
{
		// set the out-parameter to the default value (which is false)
		ZStd1HeaderParsingResult retVal{ 0, false };

		if (size < 1)
		{
				// 1 byte is the absolute minimum of data we need
				return retVal;
		}

		// the only possible values currently are: either 1 (i. e. not chunk) or 3 (so we expect the only existing chunk-type "1", which has
		//  a fixed size of 2 bytes
		if ((*(char*)(ptrData)) == 1)
		{
				// this is valid, and it means that the size of the header is 1 byte
				retVal.headerSize = 1;
				return retVal;
		}

		if ((*(char*)(ptrData)) == 3)
		{
				// in this case... the size must be at least 3 
				if (size < 3)
				{
						return retVal;
				}
		}

		// is "chunk type 1" next?
		if (*((char*)(ptrData)+1) == 1)
		{
				// yes, so now the LSB gives the information about "hiLoByteUnpackPreprocessing"
				retVal.hiLoByteUnpackPreprocessing = ((*((char*)(ptrData)+2)) & 1) == 1;

				retVal.headerSize = 3;
				return retVal;
		}

		// we currently don't have any other chunk-type, so we fall through to "error return"
		// otherwise - this is not a valid "zstd1"-header
		return retVal;
}

shared_ptr<libCZI::IBitmapData> DecodeAndProcess(const void* ptrData, size_t size, libCZI::PixelType pixelType, uint32_t width, uint32_t height, bool doHiLoByteUnpacking)
{
		unsigned long long uncompressedSize = ZSTD_getFrameContentSize(ptrData, size);
		if (uncompressedSize == ZSTD_CONTENTSIZE_UNKNOWN)
		{
				throw std::runtime_error("The decompressed size cannot be determined.");
		}
		else if (uncompressedSize == ZSTD_CONTENTSIZE_ERROR)
		{
				throw std::runtime_error("The compressed data is not recognized.");
		}

		if (uncompressedSize != (uint64_t)height * width * Utils::GetBytesPerPixel(pixelType))
		{
				throw std::runtime_error("The compressed data is not valid.");
		}

		return doHiLoByteUnpacking ?
				DecodeAndProcessWithHiLoByteUnpacking(ptrData, size, pixelType, width, height) :
				DecodeAndProcessNoHiLoByteUnpacking(ptrData, size, pixelType, width, height);
}

shared_ptr<libCZI::IBitmapData> DecodeAndProcessWithHiLoByteUnpacking(const void* ptrData, size_t size, libCZI::PixelType pixelType, uint32_t width, uint32_t height)
{
		size_t uncompressedSize = (size_t)height * width * Utils::GetBytesPerPixel(pixelType);
		std::unique_ptr<void, void(*)(void*)> tmpBuffer(malloc(uncompressedSize), free);
		ZstdDecompressAndThrowIfError(ptrData, size, tmpBuffer.get(), uncompressedSize);
		auto bitmap = CStdBitmapData::Create(pixelType, width, height, width * Utils::GetBytesPerPixel(pixelType));
		auto bmLckInfo = libCZI::ScopedBitmapLockerSP(bitmap);
		if (!LoHiBytePack_C(tmpBuffer.get(), bmLckInfo.ptrDataRoi, uncompressedSize))
		{
				throw std::runtime_error("Error with \"LoHiByteUnpack\".");
		}

		return bitmap;
}

shared_ptr<libCZI::IBitmapData> DecodeAndProcessNoHiLoByteUnpacking(const void* ptrData, size_t size, libCZI::PixelType pixelType, uint32_t width, uint32_t height)
{
		size_t uncompressedSize = (size_t)height * width * Utils::GetBytesPerPixel(pixelType);
		auto bitmap = CStdBitmapData::Create(pixelType, width, height, width * Utils::GetBytesPerPixel(pixelType));
		auto bmLckInfo = libCZI::ScopedBitmapLockerSP(bitmap);
		ZstdDecompressAndThrowIfError(ptrData, size, bmLckInfo.ptrDataRoi, uncompressedSize);
		return bitmap;
}

void ZstdDecompressAndThrowIfError(const void* ptrData, size_t size, void* ptrDst, size_t dstSize)
{
		size_t decompressedSize = ZSTD_decompress(ptrDst, dstSize, ptrData, size);
		if (ZSTD_isError(decompressedSize))
		{
				switch (ZSTD_ErrorCode ec = ZSTD_getErrorCode(decompressedSize))
				{
				case ZSTD_error_dstSize_tooSmall:
						throw std::runtime_error("The size of the output-buffer is too small.");
				default:
						std::string errorText = "\"ZSTD_decompress\" returned with error-code " + std::to_string(ec) + ".";
						throw std::runtime_error(errorText);
				}

				throw  std::runtime_error("decompression failed...");
		}
}

bool LoHiBytePack_C(const void* ptrSrc, void* ptrDst, size_t length)
{
		if (length == 0)
		{
				return true;
		}

		if (length % 2 != 0)
		{
				return false;
		}

		const uint8_t* pSrc = static_cast<const uint8_t*>(ptrSrc);
		uint16_t* pDst = static_cast<uint16_t*>(ptrDst);

		const size_t halfLength = length / 2;
		for (size_t i = 0; i < halfLength; ++i)
		{
				const uint16_t v = *pSrc | (static_cast<uint16_t>(*(pSrc + halfLength)) << 8);
				*pDst++ = v;
				++pSrc;
		}

		return true;
}
