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
#include "BitmapOperations.h"
#include "inc_libCZI_Config.h"
#include "CziSubBlock.h"

using namespace libCZI;

static std::shared_ptr<libCZI::IBitmapData> CreateBitmapFromSubBlock_JpgXr(ISubBlock* subBlk)
{
		auto dec = GetSite()->GetDecoder(ImageDecoderType::JPXR_JxrLib, nullptr);
		const void* ptr; size_t size;
		subBlk->DangerousGetRawData(ISubBlock::MemBlkType::Data, ptr, size);
		SubBlockInfo subBlockInfo = subBlk->GetSubBlockInfo();
		return dec->Decode(ptr, size, subBlockInfo.pixelType, subBlockInfo.physicalSize.w, subBlockInfo.physicalSize.h);
}

static std::shared_ptr<libCZI::IBitmapData> CreateBitmapFromSubBlock_ZStd0(ISubBlock* subBlk)
{
		auto dec = GetSite()->GetDecoder(ImageDecoderType::ZStd0, nullptr);
		const void* ptr; size_t size;
		subBlk->DangerousGetRawData(ISubBlock::MemBlkType::Data, ptr, size);
		SubBlockInfo subBlockInfo = subBlk->GetSubBlockInfo();
		return dec->Decode(ptr, size, subBlockInfo.pixelType, subBlockInfo.physicalSize.w, subBlockInfo.physicalSize.h);
}

static std::shared_ptr<libCZI::IBitmapData> CreateBitmapFromSubBlock_ZStd1(ISubBlock* subBlk)
{
		auto dec = GetSite()->GetDecoder(ImageDecoderType::ZStd1, nullptr);
		const void* ptr; size_t size;
		subBlk->DangerousGetRawData(ISubBlock::MemBlkType::Data, ptr, size);
		SubBlockInfo subBlockInfo = subBlk->GetSubBlockInfo();

		return dec->Decode(ptr, size, subBlockInfo.pixelType, subBlockInfo.physicalSize.w, subBlockInfo.physicalSize.h);
}

static std::shared_ptr<libCZI::IBitmapData> CreateBitmapFromSubBlock_Uncompressed(ISubBlock* subBlk)
{
		size_t size;
		CSharedPtrAllocator sharedPtrAllocator(subBlk->GetRawData(ISubBlock::MemBlkType::Data, &size));

		const auto& sbBlkInfo = subBlk->GetSubBlockInfo();

		// TODO: How exactly shoud the stride be derived? It seems that stride must be exactly linesize.
		const std::uint32_t stride = sbBlkInfo.physicalSize.w * CziUtils::GetBytesPerPel(sbBlkInfo.pixelType);
		if ((size_t)stride * sbBlkInfo.physicalSize.h > size)
		{
				throw std::logic_error("insufficient size of subblock");
		}

		auto sb = CBitmapData<CSharedPtrAllocator>::Create(
				sharedPtrAllocator,
				sbBlkInfo.pixelType,
				sbBlkInfo.physicalSize.w,
				sbBlkInfo.physicalSize.h,
				stride);

#if LIBCZI_ISBIGENDIANHOST
		if (!CziUtils::IsPixelTypeEndianessAgnostic(subBlk->GetSubBlockInfo().pixelType))
		{
				return CBitmapOperations::ConvertToBigEndian(sb.get());
		}
#endif

		return sb;
}

std::shared_ptr<libCZI::IBitmapData> libCZI::CreateBitmapFromSubBlock(ISubBlock* subBlk)
{
		switch (subBlk->GetSubBlockInfo().GetCompressionMode())
		{
		case CompressionMode::JpgXr:
				return CreateBitmapFromSubBlock_JpgXr(subBlk);
		case CompressionMode::Zstd0:
				return CreateBitmapFromSubBlock_ZStd0(subBlk);
		case CompressionMode::Zstd1:
				return CreateBitmapFromSubBlock_ZStd1(subBlk);
		case CompressionMode::UnCompressed:
				return CreateBitmapFromSubBlock_Uncompressed(subBlk);
		default:	// silence warnings
				throw std::logic_error("The method or operation is not implemented.");
		}
}
