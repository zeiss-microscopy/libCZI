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
#include "stdAllocator.h"
#include <limits>

void* CHeapAllocator::Allocate(std::uint64_t size)
{
	if (size > (std::numeric_limits<size_t>::max)())
	{
		throw std::out_of_range("The requested size for allocation is out-of-range.");
	}
#if defined(__EMSCRIPTEN__)||defined(__APPLE__)
	//return malloc((size_t)size);
	void* pv = malloc((size_t)size);
	//if (pv) memset(pv, 0x7f, size);
	//if (pv) memset(pv, 0xcf, size);
	if (pv) memset(pv, 0xdf, size);
	//if (pv) memset(pv, 0xef, size);
	return pv;
#else
#if defined(__GNUC__)
	//return aligned_alloc(32, size);
	void* pv = aligned_alloc(32, size);
	//if (pv) memset(pv, 0x7f, size);
	//if (pv) memset(pv, 0xcf, size);
	if (pv) memset(pv, 0xdf, size);
	//if (pv) memset(pv, 0xef, size);
	// TODO(Leo) Find where pv is returned to, try to do the memset() up the chain rather than here.
	return pv;
#else
	void* pv = _aligned_malloc((size_t)size, 32);
	//if (pv) memset(pv, 0x7f, size);
	//if (pv) memset(pv, 0xcf, size);
	if (pv) memset(pv, 0xdf, size);
	//if (pv) memset(pv, 0xef, size);
	return pv;
#endif
#endif
}

void CHeapAllocator::Free(void* ptr)
{
#if defined(__GNUC__)||defined(__EMSCRIPTEN__)
	free(ptr);
#else
	_aligned_free(ptr);
#endif
}


