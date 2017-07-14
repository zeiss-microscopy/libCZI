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
#include "SaveBitmap.h"
#include <fstream>

#if defined(WIN32ENV)
#include <atlbase.h>

#pragma comment(lib, "Windowscodecs.lib")

using namespace libCZI;

static void ThrowIfFailed(const char* function, HRESULT hr)
{
	if (FAILED(hr))
	{
		char errorMsg[255];
		_snprintf_s(errorMsg, _TRUNCATE, "COM-ERROR hr=0x%08X (%s)", hr, function);
		throw std::runtime_error(errorMsg);
	}
}

static void ThrowIfFailed(const char* function, HRESULT hr, std::function<bool(HRESULT)> checkFunc)
{
	if (checkFunc(hr) != true)
	{
		char errorMsg[255];
		_snprintf_s(errorMsg, _TRUNCATE, "COM-ERROR hr=0x%08X (%s)", hr, function);
		throw std::runtime_error(errorMsg);
	}
}



CSaveData::CSaveData(const wchar_t* fileName, SaveDataFormat dataFormat)
	: fileName(fileName), format(dataFormat)
{
}

//void CSaveData::SaveAsFloat32(std::shared_ptr<IBitmapData> bitmap)
//{
//	switch (this->format)
//	{
//	case SaveDataFormat::WDP:
//		this->SaveWithWIC(this->fileName.c_str(), GUID_ContainerFormatWmp, GUID_WICPixelFormat32bppGrayFloat, bitmap);
//		break;
//	case SaveDataFormat::CSV:
//		this->SaveAsCSV(this->fileName.c_str(), bitmap);
//		break;
//	case SaveDataFormat::BINARY:
//		this->SaveAsBINARY(this->fileName.c_str(), bitmap);
//		break;
//	default:
//		break;
//	}
//}
//
//void CSaveData::SaveAsGray16(std::shared_ptr<IBitmapData> bitmap)
//{
//	switch (this->format)
//	{
//	case SaveDataFormat::PNG:
//		this->SaveWithWIC(this->fileName.c_str(), GUID_ContainerFormatPng, GUID_WICPixelFormat16bppGray, bitmap);
//		break;
//	default:
//		break;
//	}
//}
//
//void CSaveData::SaveAsGray8(std::shared_ptr<IBitmapData> bitmap)
//{
//	switch (this->format)
//	{
//	case SaveDataFormat::PNG:
//		this->SaveWithWIC(this->fileName.c_str(), GUID_ContainerFormatPng, GUID_WICPixelFormat8bppGray, bitmap);
//		break;
//	default:
//		break;
//	}
//}

void CSaveData::Save(libCZI::IBitmapData* bitmap)
{
	switch (bitmap->GetPixelType())
	{
	case libCZI::PixelType::Bgr24:
		this->SaveWithWIC(this->fileName.c_str(), GUID_ContainerFormatPng, GUID_WICPixelFormat24bppBGR, bitmap);
		break;
	case libCZI::PixelType::Gray16:
		this->SaveWithWIC(this->fileName.c_str(), GUID_ContainerFormatPng, GUID_WICPixelFormat16bppGray, bitmap);
		break;
	}
}

void CSaveData::SaveWithWIC(const wchar_t* filename, const GUID encoder, const WICPixelFormatGUID& wicPixelFmt, IBitmapData* bitmap)
{
	CComPtr<IWICImagingFactory> cpWicImagingFactor;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&cpWicImagingFactor);
	ThrowIfFailed("Creating WICImageFactory", hr);

	CComPtr<IWICStream> stream;
	// Create a stream for the encoder
	hr = cpWicImagingFactor->CreateStream(&stream);
	ThrowIfFailed("IWICImagingFactory::CreateStream", hr);

	// Initialize the stream using the output file path
	hr = stream->InitializeFromFilename(filename, GENERIC_WRITE);
	ThrowIfFailed("IWICStream::InitializeFromFilename", hr);

	this->SaveWithWIC(cpWicImagingFactor, stream, encoder, wicPixelFmt, bitmap);

	hr = stream->Commit(STGC_DEFAULT);
	ThrowIfFailed("IWICStream::Commit", hr, [](HRESULT ec) {return SUCCEEDED(ec) || ec == E_NOTIMPL; });
}

void CSaveData::SaveWithWIC(IWICImagingFactory* pFactory, IWICStream* destStream, const GUID encoder, const WICPixelFormatGUID& wicPixelFmt, IBitmapData* spBitmap)
{
	// cf. http://msdn.microsoft.com/en-us/library/windows/desktop/ee719797(v=vs.85).aspx
	/*CComPtr<IWICImagingFactory> cpWicImagingFactor;
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (LPVOID*)&cpWicImagingFactor);
	ThrowIfFailed("Creating WICImageFactory", hr);*/

	CComPtr<IWICBitmapEncoder> wicBitmapEncoder;
	HRESULT hr = pFactory->CreateEncoder(
		/*GUID_ContainerFormatPng*//*GUID_ContainerFormatTiff*//*GUID_ContainerFormatWmp*/encoder,
		nullptr,    // No preferred codec vendor.
		&wicBitmapEncoder);
	ThrowIfFailed("Creating IWICImagingFactory::CreateEncoder", hr);

	// Create encoder to write to image file
	hr = wicBitmapEncoder->Initialize(destStream, WICBitmapEncoderNoCache);
	ThrowIfFailed("IWICBitmapEncoder::Initialize", hr);

	CComPtr<IWICBitmapFrameEncode> frameEncode;
	hr = wicBitmapEncoder->CreateNewFrame(&frameEncode, nullptr);
	ThrowIfFailed("IWICBitmapEncoder::CreateNewFrame", hr);

	hr = frameEncode->Initialize(nullptr);
	ThrowIfFailed("IWICBitmapFrameEncode::CreateNewFrame", hr);

	hr = frameEncode->SetSize(spBitmap->GetWidth(), spBitmap->GetHeight());
	ThrowIfFailed("IWICBitmapFrameEncode::SetSize", hr);

	WICPixelFormatGUID pixelFormat = wicPixelFmt;/*GUID_WICPixelFormat32bppGrayFloat;*/
	hr = frameEncode->SetPixelFormat(&pixelFormat);
	ThrowIfFailed("IWICBitmapFrameEncode::SetPixelFormat", hr);

	if (pixelFormat != wicPixelFmt)
	{
		// The encoder does not support the pixelformat we want to add, but it was as kind as
		// to give a proposal of the "closest comparable pixelformat it supports" -> so, now 
		// we better employ a converter...

		// TODO
	}

	//const char* pixelFormatActual = Utils::WICPixelFormatToInformalString(pixelFormat);

	auto bitmapData = spBitmap->Lock();
	hr = frameEncode->WritePixels(spBitmap->GetHeight(), bitmapData.stride, spBitmap->GetHeight()* bitmapData.stride, (BYTE*)bitmapData.ptrDataRoi);
	spBitmap->Unlock();
	ThrowIfFailed("IWICBitmapFrameEncode::WritePixels", hr);

	hr = frameEncode->Commit();
	ThrowIfFailed("IWICBitmapFrameEncode::Commit", hr);

	hr = wicBitmapEncoder->Commit();
	ThrowIfFailed("IWICBitmapEncoder::Commit", hr);
}

void CSaveData::SaveAsCSV(const wchar_t* filename, IBitmapData* bitmap)
{
	// Format: # WIDTH HEIGHT TYPE
	std::ofstream outputFile;
	outputFile.open(filename, std::ios::out);

	std::string pixeltypeStr;
	std::string fouriertransformDataLayout;
	CSaveData::DeterminePixeltypeString(bitmap, pixeltypeStr);
	//CSaveData::DetermineFourierTransformDataLayoutString(bitmap, fouriertransformDataLayout);

	outputFile << "# " << bitmap->GetWidth() << " " << bitmap->GetHeight() << " " << pixeltypeStr;
	/*if (!fouriertransformDataLayout.empty())
	{
	outputFile << " (" << fouriertransformDataLayout << ")" << std::endl;
	}
	else
	{
	outputFile << std::endl;
	}*/

	outputFile.flags(std::ios::scientific);
	auto bd = bitmap->Lock();
	PixelType pixelType = bitmap->GetPixelType();
	if (pixelType == libCZI::PixelType::Gray32Float)
	{
		outputFile.precision(std::numeric_limits<float>::max_digits10);
		for (uint32_t y = 0; y < bitmap->GetHeight(); ++y)
		{
			const float* ptr = (const float*)(((char*)bd.ptrDataRoi) + y*bd.stride);
			for (uint32_t x = 0; x < bitmap->GetWidth(); ++x)
			{
				outputFile << ptr[x];
				if (x + 1 != bitmap->GetWidth())
				{
					outputFile << ",  ";
				}
			}

			outputFile << std::endl;
		}
	}
	else if (pixelType == libCZI::PixelType::Gray64ComplexFloat)
	{
		outputFile.precision(std::numeric_limits<float>::max_digits10);
		for (uint32_t y = 0; y < bitmap->GetHeight(); ++y)
		{
			const double* ptr = (const double*)(((char*)bd.ptrDataRoi) + y*bd.stride);
			for (uint32_t x = 0; x < bitmap->GetWidth() * 2; ++x)
			{
				outputFile << ptr[x];
				if (x + 1 != 2 * bitmap->GetWidth())
				{
					outputFile << ", ";
				}
			}

			outputFile << std::endl;
		}
	}

	bitmap->Unlock();

	outputFile.close();
}

/*static*/void CSaveData::DeterminePixeltypeString(IBitmapData* bitmap, std::string& str)
{
	switch (bitmap->GetPixelType())
	{
	case libCZI::PixelType::Gray64ComplexFloat:
		str = "COMPLEXDOUBLE";
		break;
	case libCZI::PixelType::Gray32Float:
		str = "FLOAT";
		break;
	default:
		break;
	}
}
#endif

#if defined(LINUXENV)
//#if !defined(EMSCRIPTEN)
using namespace libCZI;

#include <png.h>
#include "utils.h"

CSaveData::CSaveData(const wchar_t* fileName, SaveDataFormat dataFormat)
	: fileName(fileName), format(dataFormat)
{
}

void CSaveData::Save(libCZI::IBitmapData* bitmap)
{
	switch (bitmap->GetPixelType())
	{
	case libCZI::PixelType::Bgr24:
		this->SaveBgr24(bitmap);
		break;
	case libCZI::PixelType::Bgr48:
		this->SaveBgr48(bitmap);
		break;
	case libCZI::PixelType::Gray16:
		this->SaveGray16(bitmap);
		break;
	case libCZI::PixelType::Gray8:
		this->SaveGray8(bitmap);
		break;
	default:
		throw std::logic_error("pixeltype not implemented");
	}
}

void CSaveData::SaveBgr24(libCZI::IBitmapData* bitmap)
{
	this->SavePngTweakLineBeforeWritng(bitmap,8, PNG_COLOR_TYPE_RGB,
	   [](std::uint32_t width,void* ptrData)->void
	   {
			char* p = (char*)ptrData;
		   for (std::uint32_t x=0;x<width;++x)
		   {
			   std::swap(*p,*(p+2));
			   p+=3;
		   }
	   });
}

void CSaveData::SaveBgr48(libCZI::IBitmapData* bitmap)
{
	this->SavePngTweakLineBeforeWritng(bitmap,16, PNG_COLOR_TYPE_RGB,
		[](std::uint32_t width,void* ptrData)->void
		{
			unsigned short* p = (unsigned short*)ptrData;
			for (std::uint32_t x=0;x<width;++x)
			{
				std::swap(*p,*(p+2));
				p+=3;
			}
		});
}

void CSaveData::SaveGray16(libCZI::IBitmapData* bitmap)
{
	this->SavePng(bitmap, 16, PNG_COLOR_TYPE_GRAY);
}

void CSaveData::SaveGray8(libCZI::IBitmapData* bitmap)
{
	this->SavePng(bitmap, 8, PNG_COLOR_TYPE_GRAY);
}

void CSaveData::SavePngTweakLineBeforeWritng(libCZI::IBitmapData* bitmap, int bit_depth, int color_type,std::function<void(std::uint32_t,void*)> tweakLine)
{
	FILE* fp = this->OpenDestForWrite();
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);

	png_init_io(png_ptr,fp);

	png_set_IHDR(png_ptr,info_ptr,bitmap->GetWidth(),bitmap->GetHeight(),
				 bit_depth,color_type,PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);

	auto lck = bitmap->Lock();
	void* lineToTweak = malloc(lck.stride);
	for (std::uint32_t h=0;h<bitmap->GetHeight();++h)
	{
		void* ptr = (((char*)lck.ptrDataRoi)+h*lck.stride);
		memcpy(lineToTweak,ptr,lck.stride);
		tweakLine(bitmap->GetWidth(),lineToTweak);
		png_write_row(png_ptr,(png_bytep)lineToTweak);
	}

	free(lineToTweak);
	bitmap->Unlock();

	png_write_end(png_ptr,NULL);
	png_destroy_write_struct(&png_ptr,&info_ptr);
	fclose(fp);
}

void CSaveData::SavePng(libCZI::IBitmapData* bitmap, int bit_depth, int color_type)
{
	FILE* fp = this->OpenDestForWrite();
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);

	png_init_io(png_ptr,fp);

	png_set_IHDR(png_ptr,info_ptr,bitmap->GetWidth(),bitmap->GetHeight(),
				 bit_depth,color_type,PNG_INTERLACE_NONE,
				 PNG_COMPRESSION_TYPE_BASE,PNG_FILTER_TYPE_BASE);
	png_write_info(png_ptr,info_ptr);

	auto lck = bitmap->Lock();
	for (std::uint32_t h=0;h<bitmap->GetHeight();++h)
	{
		png_bytep ptr = (png_bytep)(((char*)lck.ptrDataRoi)+h*lck.stride);
		png_write_row(png_ptr,ptr);
	}

	bitmap->Unlock();

	png_write_end(png_ptr,NULL);
	png_destroy_write_struct(&png_ptr,&info_ptr);
	fclose(fp);
}

FILE* CSaveData::OpenDestForWrite()
{
	std::string fileNameUtf8 = convertToUtf8(this->fileName);
	FILE* fp = fopen(fileNameUtf8.c_str(), "wb");
	return fp;
}
//#endif
#endif
