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
#include "StreamImpl.h"
#include <cerrno>
#include <sstream>
#include <codecvt>
#include <iomanip>

#if LIBCZI_USE_PREADPWRITEBASED_STREAMIMPL
#include <fcntl.h>
#include <unistd.h>
#endif

using namespace std;

CSimpleStreamImpl::CSimpleStreamImpl(const wchar_t* filename)
{
#if defined(_WIN32)
	errno_t err = _wfopen_s(&this->fp, filename, L"rb");
#else
	int /*error_t*/ err = 0;

	// convert the wchar_t to an UTF8-string
	size_t requiredSize = std::wcstombs(nullptr, filename, 0);
	std::string conv(requiredSize, 0);
	conv.resize(std::wcstombs(&conv[0], filename, requiredSize));

	this->fp = fopen(conv.c_str(), "rb");
	if (!this->fp)
	{
		err = errno;
	}
#endif
	if (err != 0)
	{
		std::stringstream ss;
#if (_WIN32)
		char errMsg[100];
		strerror_s(errMsg, err);
		wstring_convert<codecvt_utf8<wchar_t>> utf8_conv;
		ss << "Error opening the file \"" << utf8_conv.to_bytes(filename) << "\" -> errno=" << err << " (" << errMsg << ")";
#else
		ss << "Error opening the file \"" << conv << "\" -> errno=" << err << " (" << strerror(err) << ")";
#endif
		throw std::runtime_error(ss.str());
	}
}

CSimpleStreamImpl::~CSimpleStreamImpl()
{
	fclose(this->fp);
}

/*virtual*/void CSimpleStreamImpl::Read(std::uint64_t offset, void *pv, std::uint64_t size, std::uint64_t* ptrBytesRead)
{
#if defined(_WIN32)
	int r = _fseeki64(this->fp, offset, SEEK_SET);
#else
	int r = fseeko(this->fp, offset, SEEK_SET);
#endif

	std::uint64_t bytesRead = fread(pv, 1, (size_t)size, this->fp);
	if (ptrBytesRead != nullptr)
	{
		*ptrBytesRead = bytesRead;
	}
}

//----------------------------------------------------------------------------


CSimpleStreamImplCppStreams::CSimpleStreamImplCppStreams(const wchar_t* filename)
{
	this->infile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
#if defined(_WIN32)
	this->infile.open(filename, ios::binary | ios::in);
#else
	// convert the wchar_t to an UTF8-string
	size_t requiredSize = std::wcstombs(nullptr, filename, 0);
	std::string conv(requiredSize, 0);
	conv.resize(std::wcstombs(&conv[0], filename, requiredSize));
	this->infile.open(conv.c_str(), ios::binary | ios::in);
#endif
}

CSimpleStreamImplCppStreams::~CSimpleStreamImplCppStreams()
{
	this->infile.close();
}

void CSimpleStreamImplCppStreams::Read(std::uint64_t offset, void *pv, std::uint64_t size, std::uint64_t* ptrBytesRead)
{
	this->infile.seekg(offset, ios::beg);
	this->infile.read((char*)pv, size);
	if (ptrBytesRead != nullptr)
	{
		*ptrBytesRead = this->infile.gcount();
	}
}

//----------------------------------------------------------------------------

#if LIBCZI_USE_PREADPWRITEBASED_STREAMIMPL
CStreamImplPread::CStreamImplPread(const wchar_t* filename)
	: fileDescriptor(0)
{
	size_t requiredSize = std::wcstombs(nullptr, filename, 0);
	std::string conv(requiredSize, 0);
	conv.resize(std::wcstombs(&conv[0], filename, requiredSize));
	this->fileDescriptor = open(conv.c_str(), O_RDONLY);
	if (this->fileDescriptor < 0)
	{
		auto err = errno;
		std::stringstream ss;
		ss << "Error opening the file \"" << conv << "\" -> errno=" << err << " (" << strerror(err) << ")";
		throw std::runtime_error(ss.str());
	}
}

CStreamImplPread::~CStreamImplPread()
{
	if (this->fileDescriptor != 0)
	{
		close(this->fileDescriptor);
	}
}

/*virtual*/void CStreamImplPread::Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead)
{
	ssize_t bytesRead = pread(this->fileDescriptor, pv, size, offset);
	if (bytesRead < 0)
	{
		auto err = errno;
		std::stringstream ss;
		ss << "Error reading from file (errno=" << err << " -> " << strerror(err) << ")";
		throw std::runtime_error(ss.str());
	}

	if (ptrBytesRead != nullptr)
	{
		*ptrBytesRead = bytesRead;
	}
}

//----------------------------------------------------------------------------

COutputStreamImplPwrite::COutputStreamImplPwrite(const wchar_t* filename, bool overwriteExisting)
	: fileDescriptor(0)
{
	size_t requiredSize = std::wcstombs(nullptr, filename, 0);
	std::string conv(requiredSize, 0);
	conv.resize(std::wcstombs(&conv[0], filename, requiredSize));

	int flags = O_WRONLY | O_CREAT | O_TRUNC;
	if (!overwriteExisting)
	{
		// If filename already exists, open will fail with EEXIST
		flags |= O_EXCL;
	}

	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	this->fileDescriptor = open(conv.c_str(), flags, mode);
	if (this->fileDescriptor < 0)
	{
		auto err = errno;
		std::stringstream ss;
		ss << "Error opening the file \"" << conv << "\" -> errno=" << err << " (" << strerror(err) << ")";
		throw std::runtime_error(ss.str());
	}
}

COutputStreamImplPwrite::~COutputStreamImplPwrite()
{
	if (this->fileDescriptor != 0)
	{
		close(this->fileDescriptor);
	}
}

/*virtual*/void COutputStreamImplPwrite::Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten)
{
	ssize_t bytesWritten = pwrite(this->fileDescriptor, pv, size, offset);
	if (bytesWritten < 0)
	{
		auto err = errno;
		std::stringstream ss;
		ss << "Error reading from file (errno=" << err << " -> " << strerror(err) << ")";
		throw std::runtime_error(ss.str());
	}

	if (ptrBytesWritten != nullptr)
	{
		*ptrBytesWritten = bytesWritten;
	}
}
#endif

//----------------------------------------------------------------------------

#if defined(_WIN32)
CSimpleStreamImplWindows::CSimpleStreamImplWindows(const wchar_t* filename)
	: handle(INVALID_HANDLE_VALUE)
{
	HANDLE h = CreateFileW(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		std::stringstream ss;
		wstring_convert<codecvt_utf8<wchar_t>> utf8_conv;
		ss << "Error opening the file \"" << utf8_conv.to_bytes(filename) << "\"";
		throw std::runtime_error(ss.str());
	}

	this->handle = h;
}

CSimpleStreamImplWindows::~CSimpleStreamImplWindows()
{
	if (this->handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->handle);
	}
}

void CSimpleStreamImplWindows::Read(std::uint64_t offset, void *pv, std::uint64_t size, std::uint64_t* ptrBytesRead)
{
	OVERLAPPED ol = { 0 };
	ol.Offset = (DWORD)(offset);
	ol.OffsetHigh = (DWORD)(offset >> 32);
	DWORD bytesRead;
	BOOL B = ReadFile(this->handle, pv, (DWORD)size, &bytesRead, &ol);
	if (!B)
	{
		DWORD lastError = GetLastError();
		std::stringstream ss;
		ss << "Error reading from file (LastError=" << std::setfill('0') << std::setw(8) << std::showbase << lastError << ")";
		throw std::runtime_error(ss.str());
	}

	if (ptrBytesRead != nullptr)
	{
		*ptrBytesRead = bytesRead;
	}
}
//----------------------------------------------------------------------------

CSimpleOutputStreamImplWindows::CSimpleOutputStreamImplWindows(const wchar_t* filename, bool overwriteExisting)
	: handle(INVALID_HANDLE_VALUE)
{
	DWORD dwCreationDisposition = overwriteExisting ? CREATE_ALWAYS : CREATE_NEW;
	HANDLE h = CreateFileW(filename, GENERIC_WRITE, FILE_SHARE_READ, NULL, dwCreationDisposition, FILE_FLAG_RANDOM_ACCESS, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		std::stringstream ss;
		wstring_convert<codecvt_utf8<wchar_t>> utf8_conv;
		ss << "Error opening the file \"" << utf8_conv.to_bytes(filename) << "\" for output.";
		throw std::runtime_error(ss.str());
	}

	this->handle = h;
}

CSimpleOutputStreamImplWindows::~CSimpleOutputStreamImplWindows()
{
	if (this->handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->handle);
		this->handle = INVALID_HANDLE_VALUE;
	}
}

/*virtual*/void CSimpleOutputStreamImplWindows::Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten)
{
	OVERLAPPED ol = { 0 };
	ol.Offset = (DWORD)(offset);
	ol.OffsetHigh = (DWORD)(offset >> 32);
	DWORD bytesWritten;
	BOOL B = WriteFile(this->handle, pv, (DWORD)size, &bytesWritten, &ol);
	if (!B)
	{
		DWORD lastError = GetLastError();
		std::stringstream ss;
		ss << "Error writing to file (LastError=" << std::setfill('0') << std::setw(8) << std::showbase << lastError << ")";
		throw std::runtime_error(ss.str());
	}

	if (ptrBytesWritten != nullptr)
	{
		*ptrBytesWritten = bytesWritten;
	}
}
#endif

//----------------------------------------------------------------------------
CStreamImplInMemory::CStreamImplInMemory(std::shared_ptr<const void> ptr, std::size_t dataSize)
	: rawData(ptr), dataBufferSize(dataSize)
{
}

CStreamImplInMemory::CStreamImplInMemory(libCZI::IAttachment* attachement)
{
	this->rawData = attachement->GetRawData(&this->dataBufferSize);
}

/*virtual*/void CStreamImplInMemory::Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead)
{
	if (pv == nullptr)
	{
		throw std::invalid_argument("Pointer cannot be null");
	}

	if (offset >= this->dataBufferSize)
	{
		std::stringstream ss;
		ss << "Error reading from memory at offset " << offset << " -> requested size: " << size << " bytes, which exceeds actual data size " << this->dataBufferSize << " bytes.";
		throw std::runtime_error(ss.str());
	}

	// Read only to the end of buffer size
	size_t sizeToCopy = (std::min)((size_t)size, (size_t)(this->dataBufferSize - offset));

	std::memcpy(pv, static_cast<const char*>(rawData.get()) + offset, sizeToCopy);
	if (ptrBytesRead != nullptr)
	{
		*ptrBytesRead = sizeToCopy;
	}
}

//----------------------------------------------------------------------------
CSimpleOutputStreamStreams::CSimpleOutputStreamStreams(const wchar_t* filename, bool overwriteExisting) : fp(nullptr)
{
	// TODO: check if file exists -> x subspecifier? http://www.cplusplus.com/reference/cstdio/fopen/ or stat -> https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c-cross-platform/230070#230070
#if defined(_WIN32)
	if (overwriteExisting == false)
	{
		FILE* testFp = nullptr;
		errno_t err = _wfopen_s(&testFp, filename, L"rb");
		if (err == 0)
		{
			if (testFp != nullptr) { fclose(testFp); }
			std::stringstream ss;
			wstring_convert<codecvt_utf8<wchar_t>> utf8_conv;
			ss << "Error opening the file \"" << utf8_conv.to_bytes(filename) << "\" for writing because it already exists.";
			throw std::runtime_error(ss.str());
		}
	}

	errno_t err = _wfopen_s(&this->fp, filename, L"wb");
#else
	int /*error_t*/ err = 0;

	// convert the wchar_t to an UTF8-string
	size_t requiredSize = std::wcstombs(nullptr, filename, 0);
	std::string conv(requiredSize, 0);
	conv.resize(std::wcstombs(&conv[0], filename, requiredSize));

	if (overwriteExisting == false)
	{
		FILE* testFp = fopen(conv.c_str(), "rb");
		if (testFp != nullptr)
		{
			fclose(testFp);
			std::stringstream ss;
			ss << "Error opening the file \"" << conv << "\" for writing because it already exists.";
			throw std::runtime_error(ss.str());
		}
	}

	this->fp = fopen(conv.c_str(), "wb");
	if (!this->fp)
	{
		err = errno;
	}
#endif
	if (err != 0)
	{
		std::stringstream ss;
#if (_WIN32)
		char errMsg[100];
		strerror_s(errMsg, err);
		wstring_convert<codecvt_utf8<wchar_t>> utf8_conv;
		ss << "Error opening the file \"" << utf8_conv.to_bytes(filename) << "\" -> errno=" << err << " (" << errMsg << ")";
#else
		ss << "Error opening the file \"" << conv << "\" -> errno=" << err << " (" << strerror(err) << ")";
#endif
		throw std::runtime_error(ss.str());
	}
}

CSimpleOutputStreamStreams::~CSimpleOutputStreamStreams()
{
	fclose(this->fp);
}

void CSimpleOutputStreamStreams::Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten)
{
#if defined(_WIN32)
	int r = _fseeki64(this->fp, offset, SEEK_SET);
#else
	int r = fseeko(this->fp, offset, SEEK_SET);
#endif

	if (r != 0)
	{
		stringstream ss;
		ss << "Seek to position " << offset << " failed.";
		throw std::runtime_error(ss.str());
	}

	std::uint64_t bytesRead = fwrite(pv, 1, (size_t)size, this->fp);
	if (ptrBytesWritten != nullptr)
	{
		*ptrBytesWritten = bytesRead;
	}
}

//-----------------------------------------------------------------------------

#if LIBCZI_USE_PREADPWRITEBASED_STREAMIMPL
CInputOutputStreamImplPreadPwrite::CInputOutputStreamImplPreadPwrite(const wchar_t* filename)
	: fileDescriptor(0)
{
	size_t requiredSize = std::wcstombs(nullptr, filename, 0);
	std::string conv(requiredSize, 0);
	conv.resize(std::wcstombs(&conv[0], filename, requiredSize));

	int flags = O_RDWR | O_CREAT;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	this->fileDescriptor = open(conv.c_str(), flags, mode);
	if (this->fileDescriptor < 0)
	{
		auto err = errno;
		std::stringstream ss;
		ss << "Error opening the file \"" << conv << "\" -> errno=" << err << " (" << strerror(err) << ")";
		throw std::runtime_error(ss.str());
	}
}

CInputOutputStreamImplPreadPwrite::~CInputOutputStreamImplPreadPwrite()
{
	if (this->fileDescriptor != 0)
	{
		close(this->fileDescriptor);
	}
}

/*virtual*/void CInputOutputStreamImplPreadPwrite::Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead)
{
	ssize_t bytesRead = pread(this->fileDescriptor, pv, size, offset);
	if (bytesRead < 0)
	{
		auto err = errno;
		std::stringstream ss;
		ss << "Error reading from file (errno=" << err << " -> " << strerror(err) << ")";
		throw std::runtime_error(ss.str());
	}

	if (ptrBytesRead != nullptr)
	{
		*ptrBytesRead = bytesRead;
	}
}

/*virtual*/void CInputOutputStreamImplPreadPwrite::Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten)
{
	ssize_t bytesWritten = pwrite(this->fileDescriptor, pv, size, offset);
	if (bytesWritten < 0)
	{
		auto err = errno;
		std::stringstream ss;
		ss << "Error reading from file (errno=" << err << " -> " << strerror(err) << ")";
		throw std::runtime_error(ss.str());
	}

	if (ptrBytesWritten != nullptr)
	{
		*ptrBytesWritten = bytesWritten;
	}
}
#endif

//-----------------------------------------------------------------------------

#if defined(_WIN32)
CSimpleInputOutputStreamImplWindows::CSimpleInputOutputStreamImplWindows(const wchar_t* filename)
	: handle(INVALID_HANDLE_VALUE)
{
	HANDLE h = CreateFileW(filename, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_RANDOM_ACCESS, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		std::stringstream ss;
		wstring_convert<codecvt_utf8<wchar_t>> utf8_conv;
		ss << "Error opening the file \"" << utf8_conv.to_bytes(filename) << "\"";
		throw std::runtime_error(ss.str());
	}

	this->handle = h;
}

/*virtual*/ CSimpleInputOutputStreamImplWindows::~CSimpleInputOutputStreamImplWindows()
{
	if (this->handle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(this->handle);
	}
}

/*virtual*/void CSimpleInputOutputStreamImplWindows::Read(std::uint64_t offset, void *pv, std::uint64_t size, std::uint64_t* ptrBytesRead)
{
	OVERLAPPED ol = { 0 };
	ol.Offset = (DWORD)(offset);
	ol.OffsetHigh = (DWORD)(offset >> 32);
	DWORD bytesRead;
	BOOL B = ReadFile(this->handle, pv, (DWORD)size, &bytesRead, &ol);
	if (!B)
	{
		DWORD lastError = GetLastError();
		std::stringstream ss;
		ss << "Error reading from file (LastError=" << std::setfill('0') << std::setw(8) << std::showbase << lastError << ")";
		throw std::runtime_error(ss.str());
	}

	if (ptrBytesRead != nullptr)
	{
		*ptrBytesRead = bytesRead;
	}
}

/*virtual*/void CSimpleInputOutputStreamImplWindows::Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten)
{
	OVERLAPPED ol = { 0 };
	ol.Offset = (DWORD)(offset);
	ol.OffsetHigh = (DWORD)(offset >> 32);
	DWORD bytesWritten;
	BOOL B = WriteFile(this->handle, pv, (DWORD)size, &bytesWritten, &ol);
	if (!B)
	{
		DWORD lastError = GetLastError();
		std::stringstream ss;
		ss << "Error writing to file (LastError=" << std::setfill('0') << std::setw(8) << std::showbase << lastError << ")";
		throw std::runtime_error(ss.str());
	}

	if (ptrBytesWritten != nullptr)
	{
		*ptrBytesWritten = bytesWritten;
	}
}
#endif

//-----------------------------------------------------------------------------

CSimpleInputOutputStreamImpl::CSimpleInputOutputStreamImpl(const wchar_t* filename)
{
	// TODO: check if file exists -> x subspecifier? http://www.cplusplus.com/reference/cstdio/fopen/ or stat -> https://stackoverflow.com/questions/230062/whats-the-best-way-to-check-if-a-file-exists-in-c-cross-platform/230070#230070
#if defined(_WIN32)
	errno_t err = _wfopen_s(&this->fp, filename, L"rwb");
#else
	int /*error_t*/ err = 0;

	// convert the wchar_t to an UTF8-string
	size_t requiredSize = std::wcstombs(nullptr, filename, 0);
	std::string conv(requiredSize, 0);
	conv.resize(std::wcstombs(&conv[0], filename, requiredSize));

	this->fp = fopen(conv.c_str(), "rwb");
	if (!this->fp)
	{
		err = errno;
	}
#endif
	if (err != 0)
	{
		std::stringstream ss;
#if (_WIN32)
		char errMsg[100];
		strerror_s(errMsg, err);
		wstring_convert<codecvt_utf8<wchar_t>> utf8_conv;
		ss << "Error opening the file \"" << utf8_conv.to_bytes(filename) << "\" -> errno=" << err << " (" << errMsg << ")";
#else
		ss << "Error opening the file \"" << conv << "\" -> errno=" << err << " (" << strerror(err) << ")";
#endif
		throw std::runtime_error(ss.str());
	}
}

CSimpleInputOutputStreamImpl::~CSimpleInputOutputStreamImpl()
{
	fclose(this->fp);
}

void CSimpleInputOutputStreamImpl::Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten)
{
#if defined(_WIN32)
	int r = _fseeki64(this->fp, offset, SEEK_SET);
#else
	int r = fseeko(this->fp, offset, SEEK_SET);
#endif

	if (r != 0)
	{
		stringstream ss;
		ss << "Seek to position " << offset << " failed.";
		throw std::runtime_error(ss.str());
	}

	std::uint64_t bytesRead = fwrite(pv, 1, (size_t)size, this->fp);
	if (ptrBytesWritten != nullptr)
	{
		*ptrBytesWritten = bytesRead;
	}
}

/*virtual*/void CSimpleInputOutputStreamImpl::Read(std::uint64_t offset, void *pv, std::uint64_t size, std::uint64_t* ptrBytesRead)
{
#if defined(_WIN32)
	int r = _fseeki64(this->fp, offset, SEEK_SET);
#else
	int r = fseeko(this->fp, offset, SEEK_SET);
#endif

	std::uint64_t bytesRead = fread(pv, 1, (size_t)size, this->fp);
	if (ptrBytesRead != nullptr)
	{
		*ptrBytesRead = bytesRead;
	}
}
