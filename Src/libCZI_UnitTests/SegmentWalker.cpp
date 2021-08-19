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
#include "SegmentWalker.h"

#if !defined(__GNUC__)
#include <pshpack2.h>
#define PACKED 
#else
#define PACKED __attribute__ ((__packed__))
#endif

struct PACKED CziSegmentHeader
{
	unsigned char Id[16];
	std::int64_t AllocatedSize;
	std::int64_t UsedSize;
};

#if !defined(__GNUC__)
#include <poppack.h>
#else
#define PACK
#endif

static bool is_big_endian();
static void ConvertSegmentHeaderToHostByteOrder(CziSegmentHeader* p);

/*static*/void CSegmentWalker::Walk(libCZI::IStream* stream, std::function<bool(int cnt, const std::string& id, std::int64_t allocatedSize, std::int64_t usedSize)> func)
{
	std::uint64_t pos = 0;
	CziSegmentHeader hdr;
	std::uint64_t bytesRead;
	std::string str;
	for (int cnt = 0;; ++cnt)
	{
		try
		{
			stream->Read(pos, &hdr, sizeof(hdr), &bytesRead);
			if (bytesRead == 0)
			{
				return;
			}
		}
		catch (const std::runtime_error& /*e*/)
		{
			return;
		}

		ConvertSegmentHeaderToHostByteOrder(&hdr);

		int strLen = 0;
		for (; strLen < sizeof(hdr.Id); ++strLen)
		{
			if (hdr.Id[strLen] == 0)
			{
				break;
			}
		}

		str.assign((const char*)hdr.Id, strLen);
		bool b = func(cnt, str, hdr.AllocatedSize, hdr.UsedSize);
		if (b == false)
		{
			return;
		}

		pos += sizeof(CziSegmentHeader) + hdr.AllocatedSize;
	}
}

/*static*/bool CSegmentWalker::CheckSegments(libCZI::IStream* stream, const CSegmentWalker::ExpectedSegment* expectedSegments, size_t countExpectedSegments)
{
	int totalCnt = 0;
	for (size_t i = 0; i < countExpectedSegments; ++i)
	{
		totalCnt += (expectedSegments + i)->cnt;
	}

	bool tooManySegments = false;
	bool wrongId = false;
	bool allReceived = false;
	CSegmentWalker::Walk(
		stream,
		[&](int cnt, const std::string& id, std::int64_t allocatedSize, std::int64_t usedSize)->bool
	{
		const CSegmentWalker::ExpectedSegment* p = expectedSegments;
		intptr_t n = cnt;
		for (; n > 0; )
		{
			n -= p->cnt;
			if (n >= 0)
			{
				++p;
			}

			if (p >= expectedSegments + countExpectedSegments)
			{
				tooManySegments = true;
				return false;
			}
		}

		if (id != p->segmentId)
		{
			wrongId = true;
			return false;
		}

		if (cnt + 1 == totalCnt)
		{
			allReceived = true;
		}

		return true;
	});

	return !tooManySegments && !wrongId && allReceived;
}

/*static*/bool is_big_endian( )
{
	static const union {
		uint32_t i;
		char c[4];
	} bint = { 0x01020304 };

	return bint.c[0] == 1;
}

/// Convert segment header (read from file, in little-endian byte order) to the byte order of the host.
/// \param [in,out] p A pointer to the CziSegmentHeader-struct to process.
/*static*/void ConvertSegmentHeaderToHostByteOrder(CziSegmentHeader* p)
{
	// note that for sake of simplicity, we rely on a runtime-byte-order detection (instead of a compile-time detection)
	if (is_big_endian())
	{
		int64_t v = p->AllocatedSize;
		p->AllocatedSize = ((((v) & 0xff00000000000000ull) >> 56)
			| (((v) & 0x00ff000000000000ull) >> 40)
			| (((v) & 0x0000ff0000000000ull) >> 24)
			| (((v) & 0x000000ff00000000ull) >> 8)
			| (((v) & 0x00000000ff000000ull) << 8)
			| (((v) & 0x0000000000ff0000ull) << 24)
			| (((v) & 0x000000000000ff00ull) << 40)
			| (((v) & 0x00000000000000ffull) << 56));
		v = p->UsedSize;
		p->UsedSize = ((((v) & 0xff00000000000000ull) >> 56)
			| (((v) & 0x00ff000000000000ull) >> 40)
			| (((v) & 0x0000ff0000000000ull) >> 24)
			| (((v) & 0x000000ff00000000ull) >> 8)
			| (((v) & 0x00000000ff000000ull) << 8)
			| (((v) & 0x0000000000ff0000ull) << 24)
			| (((v) & 0x000000000000ff00ull) << 40)
			| (((v) & 0x00000000000000ffull) << 56));
	}
}
