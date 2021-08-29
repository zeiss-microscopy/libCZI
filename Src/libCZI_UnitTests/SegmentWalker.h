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

#pragma once

#include "inc_libCZI.h"

/// This class allows to "walk" the segments of a CZI-file.
/// Note that for sake of simplicity it is implemented independently of libCZI.
class CSegmentWalker
{
public:
	static void Walk(libCZI::IStream* stream, std::function<bool(int cnt, const std::string& id, std::int64_t allocatedSize, std::int64_t usedSize)> func);

	struct ExpectedSegment
	{
		int cnt;
		const char* segmentId;
	};

	static bool CheckSegments(libCZI::IStream* stream, const CSegmentWalker::ExpectedSegment* expectedSegments, size_t countExpectedSegments);
};
