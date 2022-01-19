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
#include "libCZI.h"
#include "CZIReader.h"
#include "CziMetadata.h"
#include "SingleChannelTileAccessor.h"
#include "SingleChannelPyramidLevelTileAccessor.h"
#include "SingleChannelScalingTileAccessor.h"
#include "StreamImpl.h"
#include "CziMetadataBuilder.h"
#include "inc_libCZI_Config.h"

using namespace libCZI;
using namespace std;

void libCZI::GetLibCZIVersion(int* pMajor, int* pMinor)
{
		if (pMajor != nullptr)
		{
				*pMajor = LIBCZI_VERSION_MAJOR;
		}

		if (pMinor != nullptr)
		{
				*pMinor = LIBCZI_VERSION_MINOR;
		}
}

void libCZI::GetLibCZIBuildInformation(BuildInformation& info)
{
		info.compilerIdentification = LIBCZI_CXX_COMPILER_IDENTIFICATION;
		info.repositoryUrl = LIBCZI_REPOSITORYREMOTEURL;
		info.repositoryBranch = LIBCZI_REPOSITORYBRANCH;
		info.repositoryTag = LIBCZI_REPOSITORYHASH;
}

std::shared_ptr<ICZIReader> libCZI::CreateCZIReader()
{
		return std::make_shared<CCZIReader>();
}

std::shared_ptr<libCZI::ICziMetadata> libCZI::CreateMetaFromMetadataSegment(IMetadataSegment* metadataSegment)
{
		return std::make_shared<CCziMetadata>(metadataSegment);
}

std::shared_ptr<IAccessor> libCZI::CreateAccesor(std::shared_ptr<ISubBlockRepository> repository, AccessorType accessorType)
{
		switch (accessorType)
		{
		case AccessorType::SingleChannelTileAccessor:
				return std::make_shared<CSingleChannelTileAccessor>(repository);
		case AccessorType::SingleChannelPyramidLayerTileAccessor:
				return std::make_shared<CSingleChannelPyramidLevelTileAccessor>(repository);
		case AccessorType::SingleChannelScalingTileAccessor:
				return std::make_shared<CSingleChannelScalingTileAccessor>(repository);
		}

		throw std::invalid_argument("unknown accessorType");
}

std::shared_ptr<libCZI::IStream> libCZI::CreateStreamFromFile(const wchar_t* szFilename)
{
#ifdef _WIN32
		return make_shared<CSimpleStreamImplWindows>(szFilename);
#else
	#if LIBCZI_USE_PREADPWRITEBASED_STREAMIMPL
		return make_shared<CStreamImplPread>(szFilename);
	#else
		return make_shared<CSimpleStreamImpl>(szFilename);
	#endif
#endif
}

std::shared_ptr<libCZI::IStream> libCZI::CreateStreamFromMemory(std::shared_ptr<const void> ptr, size_t dataSize)
{
		return make_shared<CStreamImplInMemory>(ptr, dataSize);
}

std::shared_ptr<libCZI::IStream> libCZI::CreateStreamFromMemory(libCZI::IAttachment* attachment)
{
		return make_shared<CStreamImplInMemory>(attachment);
}

std::shared_ptr<IOutputStream> libCZI::CreateOutputStreamForFile(const wchar_t* szFilename, bool overwriteExisting)
{
#ifdef _WIN32
		return make_shared<CSimpleOutputStreamImplWindows>(szFilename, overwriteExisting);
#else
	#if LIBCZI_USE_PREADPWRITEBASED_STREAMIMPL
		return make_shared<COutputStreamImplPwrite>(szFilename, overwriteExisting);
	#else
		return make_shared<CSimpleOutputStreamStreams>(szFilename, overwriteExisting);
	#endif
#endif
}

std::shared_ptr<IInputOutputStream> libCZI::CreateInputOutputStreamForFile(const wchar_t* szFilename)
{
#ifdef _WIN32
		return make_shared<CSimpleInputOutputStreamImplWindows>(szFilename);
#else
	#if LIBCZI_USE_PREADPWRITEBASED_STREAMIMPL
		return make_shared<CInputOutputStreamImplPreadPwrite>(szFilename);
	#else
		return make_shared<CSimpleInputOutputStreamImpl>(szFilename);
	#endif
#endif
}

std::shared_ptr<ICziMetadataBuilder> libCZI::CreateMetadataBuilder()
{
		return make_shared<CCZiMetadataBuilder>(L"ImageDocument");
}