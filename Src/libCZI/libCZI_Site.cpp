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
#include "decoder.h"
#include "decoder_zstd.h"
#include <mutex>
#include "bitmapData.h"
#include "decoder_wic.h"

using namespace libCZI;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////

class CSiteImpBase : public ISite
{
public:
		virtual bool IsEnabled(int logLevel)
		{
				return false;
		}

		virtual void Log(int level, const char* szMsg)
		{
		}

		virtual std::shared_ptr<libCZI::IBitmapData> CreateBitmap(libCZI::PixelType pixeltype, std::uint32_t width, std::uint32_t height, std::uint32_t stride, std::uint32_t extraRows, std::uint32_t extraColumns)
		{
				return CStdBitmapData::Create(pixeltype, width, height, stride, extraRows, extraColumns);
		}

		virtual std::shared_ptr<IDecoder> GetDecoder(ImageDecoderType type, const char* arguments)
		{
				throw std::runtime_error("must not be called...");
		}
};

#if defined(_WIN32)
class CSiteImpWic : public CSiteImpBase
{
private:
		std::once_flag	jxrDecoderInitialized;
		std::shared_ptr<IDecoder> jpgXrdecoder;
		std::once_flag	zstd0DecoderInitialized;
		std::shared_ptr<IDecoder> zstd0decoder;
		std::once_flag	zstd1DecoderInitialized;
		std::shared_ptr<IDecoder> zstd1decoder;
public:
		virtual std::shared_ptr<IDecoder> GetDecoder(ImageDecoderType type, const char* arguments)
		{
				switch (type)
				{
				case ImageDecoderType::JPXR_JxrLib:
				{
						std::call_once(jxrDecoderInitialized,
								[this]()
								{
										this->jpgXrdecoder = CWicJpgxrDecoder::Create();
								});

						return this->jpgXrdecoder;
				}
				case ImageDecoderType::ZStd0:
				{
						std::call_once(zstd0DecoderInitialized,
								[this]()
								{
										this->zstd0decoder = CZstd0Decoder::Create();
								});

						return this->zstd0decoder;
				}
				case ImageDecoderType::ZStd1:
				{
						std::call_once(zstd1DecoderInitialized,
								[this]()
								{
										this->zstd1decoder = CZstd1Decoder::Create();
								});

						return this->zstd1decoder;
				}
				}

				return shared_ptr<IDecoder>();
		}
};
#endif

class CSiteImpJxrLib : public CSiteImpBase
{
private:
		std::once_flag	jxrDecoderInitialized;
		std::shared_ptr<IDecoder> jpgXrdecoder;
		std::once_flag	zstd0DecoderInitialized;
		std::shared_ptr<IDecoder> zstd0decoder;
		std::once_flag	zstd1DecoderInitialized;
		std::shared_ptr<IDecoder> zstd1decoder;
public:
		virtual std::shared_ptr<IDecoder> GetDecoder(ImageDecoderType type, const char* arguments)
		{
				switch (type)
				{
				case ImageDecoderType::JPXR_JxrLib:
				{
						std::call_once(jxrDecoderInitialized,
								[this]()
								{
										this->jpgXrdecoder = CJxrLibDecoder::Create();
								});

						return this->jpgXrdecoder;
				}
				case ImageDecoderType::ZStd0:
				{
						std::call_once(zstd0DecoderInitialized,
								[this]()
								{
										this->zstd0decoder = CZstd0Decoder::Create();
								});

						return this->zstd0decoder;
				}
				case ImageDecoderType::ZStd1:
				{
						std::call_once(zstd1DecoderInitialized,
								[this]()
								{
										this->zstd1decoder = CZstd1Decoder::Create();
								});

						return this->zstd1decoder;
				}
				}

				return shared_ptr<IDecoder>();
		}
};

#if defined(_WIN32)
static CSiteImpWic theWicSite;
#endif
static CSiteImpJxrLib theJxrLibSite;


///////////////////////////////////////////////////////////////////////////////////////////

static std::once_flag gSite_init;
static ISite* g_site = nullptr;

libCZI::ISite* GetSite()
{
		std::call_once(gSite_init,
				[]()
				{
						if (g_site == nullptr)
						{
								g_site = libCZI::GetDefaultSiteObject(SiteObjectType::Default);
						}
				});

		return g_site;
}

libCZI::ISite* libCZI::GetDefaultSiteObject(SiteObjectType type)
{
		switch (type)
		{
#if defined(_WIN32)
		case SiteObjectType::WithWICDecoder:
				return &theWicSite;
#endif
		case SiteObjectType::WithJxrDecoder:
		case SiteObjectType::Default:
				return &theJxrLibSite;
		default:
				return nullptr;
		}
}

void libCZI::SetSiteObject(libCZI::ISite* pSite)
{
		if (g_site != nullptr)
		{
				throw std::logic_error("Site was already initialized");
		}

		g_site = pSite;
}
