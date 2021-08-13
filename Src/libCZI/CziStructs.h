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

// TODO: we need a platform-independent replacement for GUID

#if !defined(__GNUC__)
#include <pshpack2.h>
#define PACKED 
#else
#define PACKED __attribute__ ((__packed__))
#endif

/////////////////////////////////////////////////////////////////////////////////
// Enumerations
/////////////////////////////////////////////////////////////////////////////////

//enum class PixelType : std::uint8_t
//{
//    Gray8 = 0,
//    Gray16 = 1,
//    Gray32Float = 2, Bgr24 = 3,
//    Bgr48 = 4,
//    Bgr96Float = 8,
//    Bgra32 = 9,
//    Gray64ComplexFloat = 10,
//    Bgr192ComplexFloat = 11,
//    Gray32 = 12,
//    Gray64Float = 13,
//};

////////////////////////////////////////////////////////////////////
// STRUCTURES
////////////////////////////////////////////////////////////////////


typedef struct PACKED DimensionEntry
{
	char Dimension[4];
	std::int32_t Start;
	std::int32_t Size;
	float StartCoordinate;
	std::int32_t StoredSize;
} DIMENSIONENTRY;

typedef struct PACKED AttachmentInfo
{
	std::int64_t AllocatedSize;
	std::int64_t DataSize;
	std::int32_t FilePart;
	GUID ContentGuid;
	char ContentFileType[8];
	char Name[80];
	//HANDLE FileHandle;
	unsigned char spare[128];
} ATTACHMENTINFO;

typedef struct PACKED MetadataInfo
{
	std::int64_t AllocatedSize;
	std::int32_t XmlSize;
	std::int32_t BinarySize;
} METADATAINFO;

typedef struct PACKED AttachmentDirectoryInfo
{
	std::int32_t EntryCount;
	//HANDLE* attachmentHandles;
} ATTACHMENTDIRECTORYINFO;

////////////////////////////////////////////////////////////////////
// COMMON
////////////////////////////////////////////////////////////////////

struct PACKED SegmentHeader
{
	unsigned char Id[16];
	std::int64_t AllocatedSize;
	std::int64_t UsedSize;
};

// defined segment algignments (never modify this constants!)
const std::int32_t SEGMENT_ALIGN = 32;

// Sizes of segment parts (never modify this constants!)
const std::int32_t SIZE_SEGMENTHEADER = 32;
const std::int32_t SIZE_SEGMENTID = 16;
const std::int32_t SIZE_SUBBLOCKDIRECTORYENTRY_DE = 128;
const std::int32_t SIZE_ATTACHMENTENTRY = 128;
const std::int32_t SIZE_SUBBLOCKDIRECTORYENTRY_DV_FIXEDPART = 32;

// Data section within segments (never modify this constants!)
const std::int32_t SIZE_FILEHEADER_DATA = 512;
const std::int32_t SIZE_METADATA_DATA = 256;
const std::int32_t SIZE_SUBBLOCKDATA_MINIMUM = 256;
const std::int32_t SIZE_SUBBLOCKDATA_FIXEDPART = 16;
const std::int32_t SIZE_SUBBLOCKDIRECTORY_DATA = 128;
const std::int32_t SIZE_ATTACHMENTDIRECTORY_DATA = 256;
const std::int32_t SIZE_ATTACHMENT_DATA = 256;
const std::int32_t SIZE_DIMENSIONENTRYDV = 20;

// internal implementation limits (internal use of pre-allocated structures)
// re-dimension if more items needed
const int MAXDIMENSIONS = 40;
//#define MAXFILE 50000
//
//#define ATTACHMENT_SPARE 2048

////////////////////////////////////////////////////////////////////
// SCHEMAS
////////////////////////////////////////////////////////////////////

// FileHeader

struct PACKED FileHeaderSegmentData
{
	std::int32_t Major;
	std::int32_t Minor;
	std::int32_t _Reserved1;
	std::int32_t _Reserved2;
	GUID PrimaryFileGuid;
	GUID FileGuid;
	std::int32_t FilePart;
	std::int64_t SubBlockDirectoryPosition;
	std::int64_t MetadataPosition;
	std::int32_t updatePending;
	std::int64_t AttachmentDirectoryPosition;
	unsigned char _spare[SIZE_FILEHEADER_DATA - 80];  // offset 80
};

// SubBlockDirectory - Entry: DE fixed size 256 bytes

struct PACKED SubBlockDirectoryEntryDE
{
	unsigned char SchemaType[2];
	std::int32_t PixelType;
	std::int32_t SizeXStored;
	std::int32_t SizeYStored;
	unsigned char _pad[2];
	std::int32_t StartX;		// offset 16
	std::int32_t SizeX;
	std::int32_t StartY;
	std::int32_t SizeY;
	std::int32_t StartC;
	std::int32_t SizeC;
	std::int32_t StartZ;
	std::int32_t SizeZ;
	std::int32_t StartT;
	std::int32_t SizeT;
	std::int32_t StartS;
	std::int32_t StartR;
	std::int32_t StartI;
	std::int32_t StartB;
	std::int32_t Compression;
	std::int32_t StartM;
	std::int64_t FilePosition;
	std::int32_t FilePart;
	unsigned char DimensionOrder[16];
	std::int32_t StartH;
	std::int32_t Start10;
	std::int32_t Start11;
	std::int32_t Start12;
	std::int32_t Start13;
};

// SubBlockDirectory - Entry: DV variable length - mimimum of 256 bytes

// same structure for Dimension entries as used in public API
struct PACKED DimensionEntryDV : DIMENSIONENTRY
{
};

struct PACKED SubBlockDirectoryEntryDV
{
	unsigned char SchemaType[2];
	std::int32_t PixelType;
	std::int64_t FilePosition;
	std::int32_t FilePart;
	std::int32_t Compression;
	unsigned char _spare[6];
	std::int32_t DimensionCount;

	// max. allocation for ease of use (valid size = 32 + EntryCount * 20)
	struct DimensionEntryDV DimensionEntries[MAXDIMENSIONS]; // offset 32
};

struct PACKED SubBlockDirectorySegmentData
{
	std::int32_t EntryCount;
	unsigned char _spare[SIZE_SUBBLOCKDIRECTORY_DATA - 4];
	// followed by any sequence of SubBlockDirectoryEntryDE or SubBlockDirectoryEntryDV records;
};

///////////////////////////////////////////////////////////////////////////////////
// Attachment

struct PACKED AttachmentEntryA1
{
	unsigned char SchemaType[2];
	unsigned char _spare[10];
	std::int64_t FilePosition;
	std::int32_t FilePart;
	GUID ContentGuid;
	unsigned char ContentFileType[8];
	unsigned char Name[80];
};

struct PACKED AttachmentSegmentData
{
	std::int64_t DataSize;
	unsigned char _spare[8];
	union
	{
		std::uint8_t reserved[SIZE_ATTACHMENTENTRY];
		struct AttachmentEntryA1 entry;		// offset 16
	};
	unsigned char _spare2[SIZE_ATTACHMENT_DATA - SIZE_ATTACHMENTENTRY - 16];
};

struct PACKED AttachmentDirectorySegmentData
{
	std::int32_t EntryCount;
	unsigned char _spare[SIZE_ATTACHMENTDIRECTORY_DATA - 4];
	// followed by => AttachmentEntry entries[EntryCount];
};


///////////////////////////////////////////////////////////////////////////////////
// SubBlock

struct PACKED SubBlockSegmentData
{
	std::int32_t MetadataSize;
	std::int32_t AttachmentSize;
	std::int64_t DataSize;
	union PACKED
	{
		unsigned char _spare[SIZE_SUBBLOCKDATA_MINIMUM - SIZE_SUBBLOCKDATA_FIXEDPART];  // offset 16
		unsigned char entrySchema[2];
		SubBlockDirectoryEntryDV entryDV;
		SubBlockDirectoryEntryDE entryDE;
	};
};

///////////////////////////////////////////////////////////////////////////////////
// Metadata

struct PACKED MetadataSegmentData
{
	std::int32_t XmlSize;
	std::int32_t AttachmentSize;
	unsigned char _spare[SIZE_METADATA_DATA - 8];
};


////////////////////////////////////////////////////////////////////
// SEGMENTS
////////////////////////////////////////////////////////////////////

// SubBlockDirectorySegment: size = [128 bytes fixed (or variable if DV)] + MetadataSize + AttachmentSize + DataSize
struct PACKED SubBlockSegment
{
	struct SegmentHeader header;
	struct SubBlockSegmentData data;
};

// SubBlockDirectorySegment: size = 128(fixed) + EntryCount * [128 bytes fixed (or variable if DV)]
struct PACKED SubBlockDirectorySegment
{
	struct SegmentHeader header;
	struct SubBlockDirectorySegmentData data;
};

// MetdataSegment: size = 128(fixed) + dataLength
struct PACKED MetadataSegment
{
	struct SegmentHeader header;
	struct MetadataSegmentData data;
};

// AttachmentDirectorySegment: size = 256(fixed) + EntryCount * 128(fixed)
struct PACKED AttachmentDirectorySegment
{
	struct SegmentHeader header;
	struct AttachmentDirectorySegmentData data;
};

// AttachmentSegment: size = 256(fixed)
struct PACKED AttachmentSegment
{
	struct SegmentHeader header;
	struct AttachmentSegmentData data;
};

// FileHeaderSegment: size = 512(fixed)
struct PACKED FileHeaderSegment
{
	struct SegmentHeader header;
	struct FileHeaderSegmentData data;
};

#if !defined(__GNUC__)
#include <poppack.h>
#else
#define PACK
#endif

class ConvertToHostByteOrder
{
public:
	static void Convert(SegmentHeader* p);
	static void Convert(FileHeaderSegmentData* p);
	static void Convert(FileHeaderSegment* p);
	static void Convert(AttachmentEntryA1* p);
	static void Convert(SubBlockDirectoryEntryDV* p);
	static void Convert(DimensionEntryDV* p, int count);
	static void Convert(SubBlockDirectoryEntryDE* p);

	static void Convert(AttachmentSegment* p);
	static void Convert(AttachmentDirectorySegment* p);
	static void Convert(MetadataSegment* p);
	static void Convert(SubBlockDirectorySegment* p);
	static void Convert(SubBlockSegment* p);

	static void ConvertAndAllSubBlkEntries(SubBlockSegment* p);
	static void ConvertAndAllSubBlkDirEntries(SubBlockDirectorySegment* p);
};