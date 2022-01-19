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

#pragma once

#include "libCZI.h"
#include "inc_libCZI_Config.h"
#include <fstream>

/// <summary>	A simplistic stream implementation (based on C-runtime fopen). Note that this implementation is NOT thread-safe.</summary>
class CSimpleStreamImpl : public libCZI::IStream
{
private:
	FILE* fp;
public:
	CSimpleStreamImpl() = delete;
	CSimpleStreamImpl(const wchar_t* filename);
	~CSimpleStreamImpl();
public:	// interface libCZI::IStream
	virtual void Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead);
};

/// <summary>	A simplistic output-stream implementation (based on C-runtime fopen). Note that this implementation is NOT thread-safe.</summary>
class CSimpleOutputStreamStreams : public libCZI::IOutputStream
{
private:
	FILE * fp;
public:
	CSimpleOutputStreamStreams() = delete;
	CSimpleOutputStreamStreams(const wchar_t* filename, bool overwriteExisting);
	~CSimpleOutputStreamStreams();
public:	// interface libCZI::IOutputStream
	virtual void Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten);
};

/// <summary>	A simplistic stream implementation (based on C++ streams). Note that this implementation is NOT thread-safe.</summary>
class CSimpleStreamImplCppStreams : public libCZI::IStream
{
private:
	std::ifstream infile;
public:
	CSimpleStreamImplCppStreams() = delete;
	CSimpleStreamImplCppStreams(const wchar_t* filename);
	~CSimpleStreamImplCppStreams();
public:	// interface libCZI::IStream
	virtual void Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead);
};

#if LIBCZI_USE_PREADPWRITEBASED_STREAMIMPL
/// <summary>	An input-stream implementation (based on open and pread). This implementation is thread-safe.</summary>
class CStreamImplPread : public libCZI::IStream
{
private:
	int fileDescriptor;
public:
	CStreamImplPread() = delete;
	CStreamImplPread(const wchar_t* filename);
	~CStreamImplPread();
public:	// interface libCZI::IStream
	virtual void Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead);
};

/// <summary>	An output-stream implementation (based on open and pwrite). This implementation is thread-safe.</summary>
class COutputStreamImplPwrite : public libCZI::IOutputStream
{
private:
	int fileDescriptor;
public:
	COutputStreamImplPwrite() = delete;
	COutputStreamImplPwrite(const wchar_t* filename, bool overwriteExisting);
	~COutputStreamImplPwrite();
public:	// interface libCZI::IOutputStream
	virtual void Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten);
};
#endif

#if defined(_WIN32)
class CSimpleStreamImplWindows : public libCZI::IStream
{
private:
	HANDLE handle;
public:
	CSimpleStreamImplWindows() = delete;
	CSimpleStreamImplWindows(const wchar_t* filename);
	~CSimpleStreamImplWindows();
public:	// interface libCZI::IStream
	virtual void Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead);
};

class CSimpleOutputStreamImplWindows : public libCZI::IOutputStream
{
private:
	HANDLE handle;
public:
	CSimpleOutputStreamImplWindows() = delete;
	CSimpleOutputStreamImplWindows(const wchar_t* filename, bool overwriteExisting);
	~CSimpleOutputStreamImplWindows();
public:	// interface libCZI::IOutputStream
	virtual void Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten);
};
#endif

/// <summary>	A stream implementation (based on a memory-block). </summary>
class CStreamImplInMemory : public libCZI::IStream
{
private:
	std::shared_ptr<const void> rawData;
	size_t dataBufferSize;
public:
	CStreamImplInMemory() = delete;
	CStreamImplInMemory(std::shared_ptr<const void> ptr, std::size_t dataSize);
	CStreamImplInMemory(libCZI::IAttachment* attachement);
public:	// interface libCZI::IStream
	virtual void Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead);
};

#if LIBCZI_USE_PREADPWRITEBASED_STREAMIMPL
class CInputOutputStreamImplPreadPwrite : public libCZI::IInputOutputStream
{
private:
	int fileDescriptor;
public:
	CInputOutputStreamImplPreadPwrite() = delete;
	CInputOutputStreamImplPreadPwrite(const wchar_t* filename);
	virtual ~CInputOutputStreamImplPreadPwrite();
public:
	virtual void Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead);
	virtual void Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten);
};
#endif

#if defined(_WIN32)
class CSimpleInputOutputStreamImplWindows : public libCZI::IInputOutputStream
{
private:
	HANDLE handle;
public:
	CSimpleInputOutputStreamImplWindows() = delete;
	CSimpleInputOutputStreamImplWindows(const wchar_t* filename);
	virtual ~CSimpleInputOutputStreamImplWindows();
public:
	virtual void Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead);
	virtual void Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten);
};
#endif

class CSimpleInputOutputStreamImpl : public libCZI::IInputOutputStream
{
private:
	FILE * fp;
public:
	CSimpleInputOutputStreamImpl() = delete;
	CSimpleInputOutputStreamImpl(const wchar_t* filename);
	virtual ~CSimpleInputOutputStreamImpl();
public:
	virtual void Read(std::uint64_t offset, void* pv, std::uint64_t size, std::uint64_t* ptrBytesRead);
	virtual void Write(std::uint64_t offset, const void* pv, std::uint64_t size, std::uint64_t* ptrBytesWritten);
};
