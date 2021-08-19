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

#include "stdafx.h"
#include "CziStructs.h"
#include "utilities.h"
#include "inc_libCZI_Config.h"

/*static*/void ConvertToHostByteOrder::Convert(SegmentHeader* p)
{
#if LIBCZI_ISBIGENDIANHOST
	Utilities::ConvertInt64ToHostByteOrder(&p->AllocatedSize);
	Utilities::ConvertInt64ToHostByteOrder(&p->UsedSize);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(FileHeaderSegmentData* p)
{
#if LIBCZI_ISBIGENDIANHOST
	Utilities::ConvertInt32ToHostByteOrder(&p->Major);
	Utilities::ConvertInt32ToHostByteOrder(&p->Minor);
	Utilities::ConvertInt32ToHostByteOrder(&p->_Reserved1);
	Utilities::ConvertInt32ToHostByteOrder(&p->_Reserved2);
	Utilities::ConvertGuidToHostByteOrder(&p->PrimaryFileGuid);
	Utilities::ConvertGuidToHostByteOrder(&p->FileGuid);
	Utilities::ConvertInt32ToHostByteOrder(&p->FilePart);
	Utilities::ConvertInt64ToHostByteOrder(&p->SubBlockDirectoryPosition);
	Utilities::ConvertInt64ToHostByteOrder(&p->MetadataPosition);
	Utilities::ConvertInt32ToHostByteOrder(&p->updatePending);
	Utilities::ConvertInt64ToHostByteOrder(&p->AttachmentDirectoryPosition);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(AttachmentEntryA1* p)
{
#if LIBCZI_ISBIGENDIANHOST
	Utilities::ConvertInt64ToHostByteOrder(&p->FilePosition);
	Utilities::ConvertInt32ToHostByteOrder(&p->FilePart);
	Utilities::ConvertGuidToHostByteOrder(&p->ContentGuid);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(SubBlockDirectoryEntryDV* p)
{
#if LIBCZI_ISBIGENDIANHOST
	Utilities::ConvertInt32ToHostByteOrder(&p->PixelType);
	Utilities::ConvertInt64ToHostByteOrder(&p->FilePosition);
	Utilities::ConvertInt32ToHostByteOrder(&p->FilePart);
	Utilities::ConvertInt32ToHostByteOrder(&p->Compression);
	Utilities::ConvertInt32ToHostByteOrder(&p->DimensionCount);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(DimensionEntryDV* p, int count)
{
#if LIBCZI_ISBIGENDIANHOST
	for (int i = 0; i < count; ++i)
	{
		Utilities::ConvertInt32ToHostByteOrder(&(p + i)->Start);
		Utilities::ConvertInt32ToHostByteOrder(&(p + i)->Size);
		// TODO: convert float &p->DimensionEntries[i].StartCoordinate
		Utilities::ConvertInt32ToHostByteOrder(&(p + i)->StoredSize);
	}
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(SubBlockDirectoryEntryDE * p)
{
#if LIBCZI_ISBIGENDIANHOST
	Utilities::ConvertInt32ToHostByteOrder(&p->PixelType);
	Utilities::ConvertInt32ToHostByteOrder(&p->SizeXStored);
	Utilities::ConvertInt32ToHostByteOrder(&p->SizeYStored);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartX);
	Utilities::ConvertInt32ToHostByteOrder(&p->SizeX);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartY);
	Utilities::ConvertInt32ToHostByteOrder(&p->SizeY);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartC);
	Utilities::ConvertInt32ToHostByteOrder(&p->SizeC);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartZ);
	Utilities::ConvertInt32ToHostByteOrder(&p->SizeZ);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartT);
	Utilities::ConvertInt32ToHostByteOrder(&p->SizeT);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartS);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartR);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartI);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartB);
	Utilities::ConvertInt32ToHostByteOrder(&p->Compression);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartM);
	Utilities::ConvertInt64ToHostByteOrder(&p->FilePosition);
	Utilities::ConvertInt32ToHostByteOrder(&p->FilePart);
	Utilities::ConvertInt32ToHostByteOrder(&p->StartH);
	Utilities::ConvertInt32ToHostByteOrder(&p->Start10);
	Utilities::ConvertInt32ToHostByteOrder(&p->Start11);
	Utilities::ConvertInt32ToHostByteOrder(&p->Start12);
	Utilities::ConvertInt32ToHostByteOrder(&p->Start13);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(FileHeaderSegment * p)
{
#if LIBCZI_ISBIGENDIANHOST
	ConvertToHostByteOrder::Convert(&p->header);
	ConvertToHostByteOrder::Convert(&p->data);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(AttachmentSegment * p)
{
#if LIBCZI_ISBIGENDIANHOST
	ConvertToHostByteOrder::Convert(&p->header);
	Utilities::ConvertInt64ToHostByteOrder(&p->data.DataSize);
	Utilities::ConvertInt64ToHostByteOrder(&p->data.entry.FilePosition);
	Utilities::ConvertInt32ToHostByteOrder(&p->data.entry.FilePart);
	Utilities::ConvertGuidToHostByteOrder(&p->data.entry.ContentGuid);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(AttachmentDirectorySegment * p)
{
#if LIBCZI_ISBIGENDIANHOST
	ConvertToHostByteOrder::Convert(&p->header);
	Utilities::ConvertInt32ToHostByteOrder(&p->data.EntryCount);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(MetadataSegment * p)
{
#if LIBCZI_ISBIGENDIANHOST
	ConvertToHostByteOrder::Convert(&p->header);
	Utilities::ConvertInt32ToHostByteOrder(&p->data.XmlSize);
	Utilities::ConvertInt32ToHostByteOrder(&p->data.AttachmentSize);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(SubBlockDirectorySegment * p)
{
#if LIBCZI_ISBIGENDIANHOST
	ConvertToHostByteOrder::Convert(&p->header);
	Utilities::ConvertInt32ToHostByteOrder(&p->data.EntryCount);
#endif
}

/*static*/void ConvertToHostByteOrder::Convert(SubBlockSegment * p)
{
#if LIBCZI_ISBIGENDIANHOST
	ConvertToHostByteOrder::Convert(&p->header);
	Utilities::ConvertInt32ToHostByteOrder(&p->data.MetadataSize);
	Utilities::ConvertInt32ToHostByteOrder(&p->data.AttachmentSize);
	Utilities::ConvertInt64ToHostByteOrder(&p->data.DataSize);
#endif
}

/*static*/void ConvertToHostByteOrder::ConvertAndAllSubBlkEntries(SubBlockSegment * p)
{
#if LIBCZI_ISBIGENDIANHOST
	// we need to store the "EntryCount" before shuffling the properties
	uint32_t entryCnt = p->data.entryDV.DimensionCount;
	ConvertToHostByteOrder::Convert(p);
	if (p->data.entrySchema[0] == 'D' && p->data.entrySchema[1] == 'V')
	{
		ConvertToHostByteOrder::Convert(&(p->data.entryDV));
		ConvertToHostByteOrder::Convert(p->data.entryDV.DimensionEntries, entryCnt);
	}
#endif
}

/*static*/void ConvertToHostByteOrder::ConvertAndAllSubBlkDirEntries(SubBlockDirectorySegment * p)
{
#if LIBCZI_ISBIGENDIANHOST
	// we need to store the "EntryCount" before shuffling the properties
	auto entryCnt = p->data.EntryCount;
	ConvertToHostByteOrder::Convert(p);

	SubBlockDirectoryEntryDV* pSubBlkEntry = (SubBlockDirectoryEntryDV*)(((char*)p) + sizeof(SubBlockDirectorySegment));
	for (decltype(entryCnt) i = 0; i < entryCnt; ++i)
	{
		auto dimCnt = pSubBlkEntry->DimensionCount;
		ConvertToHostByteOrder::Convert(pSubBlkEntry);
		ConvertToHostByteOrder::Convert(pSubBlkEntry->DimensionEntries, dimCnt);
		pSubBlkEntry = (SubBlockDirectoryEntryDV*)(((char*)pSubBlkEntry) + 32 + dimCnt * 20);
	}
#endif
}
