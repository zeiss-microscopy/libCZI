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
	// Compilation was failing on the original with linux distros that don't have the _ISOC11_SOURCE.
#if defined(_WIN32)
	return (void *)_aligned_malloc((size_t)size, 32);
#elif defined(_ISOC11_SOURCE)
	return aligned_alloc(32, size);
#else
	return malloc((size_t)size);
#endif
}

void CHeapAllocator::Free(void* ptr)
{
#if defined(_WIN32)
    _aligned_free(ptr);
#elif defined(_ISOC11_SOURCE)
    free(ptr);
#else
    free(ptr);
#endif
}


