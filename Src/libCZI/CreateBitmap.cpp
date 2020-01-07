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
#include "bitmapData.h"
#include "Site.h"
#include "libCZI.h"

using namespace libCZI;

// Forward declaration for JpgXr vs. uncompressed SubBlocks workaround.
static std::shared_ptr<libCZI::IBitmapData> CreateBitmapFromSubBlock_Uncompressed(ISubBlock* subBlk);

static std::shared_ptr<libCZI::IBitmapData> CreateBitmapFromSubBlock_JpgXr(ISubBlock* subBlk)
{
	auto dec = GetSite()->GetDecoder(ImageDecoderType::JPXR_JxrLib, nullptr);
	const void* ptr; size_t size;
	subBlk->DangerousGetRawData(ISubBlock::MemBlkType::Data, ptr, size);

	// Workaround for malformed CZI files which have SubBlocks labeled
	// as JpxXr in the SubBlockDirectory (verified in the CZI binary
	// file with hexdump), but which do not have the correct JpxXr
	// header magic (hex 49 49 BC 01, hexdump verified as present in
	// wellformed JpxXr SubBlocks, missing in malformed ones), and are
	// in fact uncompressed binary image data (with size agreeing with
	// uncompressed pixel data size, and image verified by rendering
	// the malformed SubBlocks as uncompressed images).
	//
	static const unsigned char jpgxr_header_magic[] = { 0x49, 0x49, 0xbc, 0x01 };
	if (memcmp((unsigned char *) ptr, jpgxr_header_magic, 4) != 0
		&& (size == ((size_t)subBlk->GetSubBlockInfo().physicalSize.h *
					 (size_t)subBlk->GetSubBlockInfo().physicalSize.w *
					 (size_t)CziUtils::GetBytesPerPel(subBlk->GetSubBlockInfo().pixelType)))) {
		return CreateBitmapFromSubBlock_Uncompressed(subBlk);
	}

	return dec->Decode(ptr, size);
}

static std::shared_ptr<libCZI::IBitmapData> CreateBitmapFromSubBlock_Uncompressed(ISubBlock* subBlk)
{
	size_t size;
	CSharedPtrAllocator sharedPtrAllocator(subBlk->GetRawData(ISubBlock::MemBlkType::Data, &size));

	// TODO: how can the stride be derived...? Add at least some more consistency checks
	auto sb = CBitmapData<CSharedPtrAllocator>::Create(
		sharedPtrAllocator,
		subBlk->GetSubBlockInfo().pixelType,
		subBlk->GetSubBlockInfo().physicalSize.w,
		subBlk->GetSubBlockInfo().physicalSize.h,
		subBlk->GetSubBlockInfo().physicalSize.w * CziUtils::GetBytesPerPel(subBlk->GetSubBlockInfo().pixelType));

	return sb;
}

std::shared_ptr<libCZI::IBitmapData> libCZI::CreateBitmapFromSubBlock(ISubBlock* subBlk)
{
	auto sbInfo = subBlk->GetSubBlockInfo();
	switch (sbInfo.mode)
	{
	case CompressionMode::JpgXr:
		return CreateBitmapFromSubBlock_JpgXr(subBlk);
	case CompressionMode::UnCompressed:
		return CreateBitmapFromSubBlock_Uncompressed(subBlk);
	default:	// silence warnings
		throw std::logic_error("The method or operation is not implemented.");
	}
}
