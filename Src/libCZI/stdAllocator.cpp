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
#include <stdlib.h>
#include "libCZI_Config.h"

const int ALLOC_ALIGNMENT = 32;

void* CHeapAllocator::Allocate(std::uint64_t size)
{
	if (size > (std::numeric_limits<size_t>::max)())
	{
		throw std::out_of_range("The requested size for allocation is out-of-range.");
	}

 #if LIBCZI_HAVE_ALIGNED_ALLOC
	return aligned_alloc(ALLOC_ALIGNMENT, size);
 #elif  LIBCZI_HAVE__ALIGNED_MALLOC
	return _aligned_malloc((size_t)size, ALLOC_ALIGNMENT);
 #else
    void* p1;
    void** p2;
    int offset = ALLOC_ALIGNMENT - 1 + sizeof(void*);
    p1 = malloc(size + offset);
    if (p1 == nullptr)
    {
        return nullptr;
    }

    p2 = (void**)(((size_t)(p1)+offset) & ~(size_t)(ALLOC_ALIGNMENT - 1));
    p2[-1] = p1;
    return p2;
 #endif
}

void CHeapAllocator::Free(void* ptr)
{
 #if LIBCZI_HAVE_ALIGNED_ALLOC // aligned_alloc goes together with 'free'
    free(ptr);
 #elif LIBCZI_HAVE__ALIGNED_MALLOC
	_aligned_free(ptr);
 #else
    void* p1 = ((void**)ptr)[-1];         // get the pointer to the buffer we allocated
    free(p1);
#endif
}
