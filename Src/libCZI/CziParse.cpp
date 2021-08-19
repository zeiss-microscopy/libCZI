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
#include "libCZI.h"
#include "CziParse.h"
#include <assert.h>
#include "Site.h"

using namespace std;
using namespace libCZI;

/*static*/const std::uint8_t CCZIParse::FILEHDRMAGIC[16] = { 'Z','I','S','R','A','W','F','I','L','E','\0','\0','\0','\0','\0','\0' };
/*static*/const std::uint8_t CCZIParse::SUBBLKDIRMAGIC[16] = { 'Z','I','S','R','A','W','D','I','R','E','C','T','O','R','Y','\0' };
/*static*/const std::uint8_t CCZIParse::SUBBLKMAGIC[16] = { 'Z','I','S','R','A','W','S','U','B','B','L','O','C','K' ,'\0','\0' };
/*static*/const std::uint8_t CCZIParse::METADATASEGMENTMAGIC[16] = { 'Z','I','S','R','A','W','M','E','T','A','D','A','T','A' ,'\0','\0' };
/*static*/const std::uint8_t CCZIParse::ATTACHMENTSDIRMAGC[16] = { 'Z','I','S','R','A','W','A','T','T','D','I','R','\0','\0' ,'\0','\0' };
/*static*/const std::uint8_t CCZIParse::ATTACHMENTBLKMAGIC[16] = { 'Z','I','S','R','A','W','A','T','T','A','C','H','\0','\0' ,'\0','\0' };
/*static*/const std::uint8_t CCZIParse::DELETEDSEGMENTMAGIC[16] = { 'D','E','L','E','T','E','D','\0','\0','\0' ,'\0','\0','\0','\0' ,'\0','\0' };

/*static*/FileHeaderSegmentData CCZIParse::ReadFileHeaderSegment(libCZI::IStream* str)
{
	FileHeaderSegment fileHeaderSegment;

	std::uint64_t bytesRead;
	try
	{
		str->Read(0, &fileHeaderSegment, sizeof(fileHeaderSegment), &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading FileHeaderSegement", 0, sizeof(fileHeaderSegment)));
	}

	if (bytesRead != sizeof(fileHeaderSegment))
	{
		CCZIParse::ThrowNotEnoughDataRead(0, sizeof(fileHeaderSegment), bytesRead);
	}

	ConvertToHostByteOrder::Convert(&fileHeaderSegment);

	if (memcmp(fileHeaderSegment.header.Id, CCZIParse::FILEHDRMAGIC, 16) != 0)
	{
		CCZIParse::ThrowIllegalData(0, "Invalid FileHdr-magic");
	}

	return fileHeaderSegment.data;
}

/*static*/CFileHeaderSegmentData CCZIParse::ReadFileHeaderSegmentData(libCZI::IStream* str)
{
	auto fileHeaderSegment = CCZIParse::ReadFileHeaderSegment(str);
	CFileHeaderSegmentData fileHdr{ &fileHeaderSegment };
	return fileHdr;
}

/*static*/CCziSubBlockDirectory CCZIParse::ReadSubBlockDirectory(libCZI::IStream* str, std::uint64_t offset)
{
	CCziSubBlockDirectory subBlkDir;
	CCZIParse::ReadSubBlockDirectory(str, offset, subBlkDir);
	subBlkDir.AddingFinished();
	return subBlkDir;
}

/*static*/void CCZIParse::ReadSubBlockDirectory(libCZI::IStream* str, std::uint64_t offset, const std::function<void(const CCziSubBlockDirectoryBase::SubBlkEntry&)>& addFunc, SegmentSizes* segmentSizes /*= nullptr*/)
{
	SubBlockDirectorySegment subBlckDirSegment;
	std::uint64_t bytesRead;
	try
	{
		str->Read(offset, &subBlckDirSegment, sizeof(subBlckDirSegment), &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading SubBlkDirectorySegement", offset, sizeof(subBlckDirSegment)));
	}

	if (bytesRead != sizeof(subBlckDirSegment))
	{
		CCZIParse::ThrowNotEnoughDataRead(offset, sizeof(subBlckDirSegment), bytesRead);
	}

	ConvertToHostByteOrder::Convert(&subBlckDirSegment);

	if (memcmp(subBlckDirSegment.header.Id, CCZIParse::SUBBLKDIRMAGIC, 16) != 0)
	{
		CCZIParse::ThrowIllegalData(offset, "Invalid SubBlkDirectory-magic");
	}

	// TODO: possible consistency check ->
	// subBlckDirSegment.header.UsedSize <= subBlckDirSegment.header.AllocatedSize

	std::uint64_t subBlkDirSize = subBlckDirSegment.header.UsedSize;
	if (subBlkDirSize == 0)
	{
		// allegedly, "UsedSize" may not be valid in early versions
		subBlkDirSize = subBlckDirSegment.header.AllocatedSize;
	}

	if (subBlkDirSize < sizeof(SubBlockDirectorySegmentData))
	{
		CCZIParse::ThrowIllegalData(offset, "Invalid SubBlkDirectory-Allocated-Size");
	}

	subBlkDirSize -= sizeof(SubBlockDirectorySegmentData);

	if (segmentSizes != nullptr)
	{
		segmentSizes->AllocatedSize = subBlckDirSegment.header.AllocatedSize;
		segmentSizes->UsedSize = subBlckDirSegment.header.UsedSize;
	}

	// now read the used-size from stream
	std::unique_ptr<void, decltype(free)*> pBuffer(malloc((size_t)subBlkDirSize), free);
	try
	{
		str->Read(offset + sizeof(subBlckDirSegment), pBuffer.get(), subBlkDirSize, &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading FileHeaderSegement", offset + sizeof(subBlckDirSegment), subBlkDirSize));
	}

	if (bytesRead != subBlkDirSize)
	{
		CCZIParse::ThrowNotEnoughDataRead(offset + sizeof(subBlckDirSegment), subBlkDirSize, bytesRead);
	}

	int currentOffset = 0;
	CCZIParse::ParseThroughDirectoryEntries(
		subBlckDirSegment.data.EntryCount,
		[&](int numberOfBytes, void* ptr)->void
	{
		if (currentOffset + numberOfBytes <= subBlkDirSize)
		{
			memcpy(ptr, ((char*)pBuffer.get()) + currentOffset, numberOfBytes);
			currentOffset += numberOfBytes;
		}
		else
		{
			CCZIParse::ThrowIllegalData(offset + sizeof(subBlckDirSegment) + currentOffset, "SubBlockDirectory data too small");
		}
	},
		[&](const SubBlockDirectoryEntryDE* subBlkDirDE, const SubBlockDirectoryEntryDV* subBlkDirDV)->void
	{
		if (subBlkDirDE != nullptr)
		{
			CCZIParse::AddEntryToSubBlockDirectory(subBlkDirDE, addFunc);
		}
		else if (subBlkDirDV != nullptr)
		{
			CCZIParse::AddEntryToSubBlockDirectory(subBlkDirDV, addFunc);
		}
	});
}

/*static*/void CCZIParse::ReadSubBlockDirectory(libCZI::IStream* str, std::uint64_t offset, CCziSubBlockDirectory& subBlkDir)
{
	CCZIParse::ReadSubBlockDirectory(str, offset, [&](const CCziSubBlockDirectoryBase::SubBlkEntry& e)->void {subBlkDir.AddSubBlock(e); });
}

/*static*/CCziAttachmentsDirectory CCZIParse::ReadAttachmentsDirectory(libCZI::IStream* str, std::uint64_t offset)
{
	CCziAttachmentsDirectory attDir;
	CCZIParse::ReadAttachmentsDirectory(str, offset, [&](const CCziAttachmentsDirectoryBase::AttachmentEntry& ae)->void {attDir.AddAttachmentEntry(ae); });
	return attDir;
}

/*static*/void CCZIParse::ReadAttachmentsDirectory(libCZI::IStream* str, std::uint64_t offset, const std::function<void(const CCziAttachmentsDirectoryBase::AttachmentEntry&)>& addFunc, SegmentSizes* segmentSizes/*= nullptr*/)
{
	AttachmentDirectorySegment attachmentDirSegment;
	std::uint64_t bytesRead;
	try
	{
		str->Read(offset, &attachmentDirSegment, sizeof(attachmentDirSegment), &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading AttachmentDirectorySegment", offset, sizeof(attachmentDirSegment)));
	}

	if (bytesRead != sizeof(attachmentDirSegment))
	{
		CCZIParse::ThrowNotEnoughDataRead(offset, sizeof(attachmentDirSegment), bytesRead);
	}

	ConvertToHostByteOrder::Convert(&attachmentDirSegment);

	if (memcmp(attachmentDirSegment.header.Id, CCZIParse::ATTACHMENTSDIRMAGC, 16) != 0)
	{
		CCZIParse::ThrowIllegalData(offset, "Invalid AttachmentDirectory-magic");
	}

	if (segmentSizes != nullptr)
	{
		segmentSizes->UsedSize = attachmentDirSegment.header.UsedSize;
		segmentSizes->AllocatedSize = attachmentDirSegment.header.AllocatedSize;
	}

	// TODO: we can add a couple of consistency checks here

	// now read the AttachmentEntries
	std::uint64_t attachmentEntriesSize = ((std::uint64_t)attachmentDirSegment.data.EntryCount) * sizeof(AttachmentEntryA1);

	std::unique_ptr<AttachmentEntryA1, decltype(free)*> pBuffer((AttachmentEntryA1*)malloc((size_t)attachmentEntriesSize), free);

	// now read the attachment-entries
	try
	{
		str->Read(offset + sizeof(attachmentDirSegment), pBuffer.get(), attachmentEntriesSize, &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading FileHeaderSegement", offset + sizeof(attachmentDirSegment), attachmentEntriesSize));
	}

	if (bytesRead != attachmentEntriesSize)
	{
		CCZIParse::ThrowNotEnoughDataRead(offset + sizeof(attachmentDirSegment), attachmentEntriesSize, bytesRead);
	}

	for (int i = 0; i < attachmentDirSegment.data.EntryCount; ++i)
	{
		ConvertToHostByteOrder::Convert(pBuffer.get() + i);

		const AttachmentEntryA1* pSrc = pBuffer.get() + i;
		CCziAttachmentsDirectoryBase::AttachmentEntry ae;
		bool b = CheckAttachmentSchemaType((const char*)&pSrc->SchemaType[0], 2);
		if (b == false)
		{
			// TODO: what do we do if we encounter this...?
			continue;
		}

		ae.FilePosition = pSrc->FilePosition;
		ae.ContentGuid = pSrc->ContentGuid;
		memcpy(&ae.ContentFileType[0], &pSrc->ContentFileType[0], sizeof(AttachmentEntryA1::ContentFileType));
		memcpy(&ae.Name[0], &pSrc->Name[0], sizeof(AttachmentEntryA1::Name));
		ae.Name[sizeof(AttachmentEntryA1::Name) - 1] = '\0';
		addFunc(ae);
	}
}

/*static*/CCZIParse::SubBlockData CCZIParse::ReadSubBlock(libCZI::IStream* str, std::uint64_t offset, const SubBlockStorageAllocate& allocateInfo)
{
	SubBlockSegment subBlckSegment;
	std::uint64_t bytesRead;
	try
	{
		str->Read(offset, &subBlckSegment, sizeof(subBlckSegment), &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading SubBlock-Segment", offset, sizeof(subBlckSegment)));
	}

	if (bytesRead != sizeof(subBlckSegment))
	{
		CCZIParse::ThrowNotEnoughDataRead(offset, sizeof(subBlckSegment), bytesRead);
	}

	ConvertToHostByteOrder::Convert(&subBlckSegment);

	if (memcmp(subBlckSegment.header.Id, CCZIParse::SUBBLKMAGIC, 16) != 0)
	{
		CCZIParse::ThrowIllegalData(offset, "Invalid SubBlockk-magic");
	}

	uint32_t lengthSubblockSegmentData = 0;
	SubBlockData sbd;
	if (subBlckSegment.data.entrySchema[0] == 'D' && subBlckSegment.data.entrySchema[1] == 'V')
	{
		ConvertToHostByteOrder::Convert(&subBlckSegment.data.entryDV);
		sbd.compression = subBlckSegment.data.entryDV.Compression;
		sbd.pixelType = subBlckSegment.data.entryDV.PixelType;
		sbd.mIndex = (std::numeric_limits<int>::max)();
		ConvertToHostByteOrder::Convert(subBlckSegment.data.entryDV.DimensionEntries, subBlckSegment.data.entryDV.DimensionCount);
		for (int i = 0; i < subBlckSegment.data.entryDV.DimensionCount; ++i)
		{
			const DimensionEntryDV* dimEntry = subBlckSegment.data.entryDV.DimensionEntries + i;
			if (IsMDimension(dimEntry->Dimension, sizeof(dimEntry->Dimension)))
			{
				sbd.mIndex = dimEntry->Start;
			}
			else if (IsXDimension(dimEntry->Dimension, sizeof(dimEntry->Dimension)))
			{
				sbd.logicalRect.x = dimEntry->Start;
				sbd.logicalRect.w = dimEntry->Size;
				sbd.physicalSize.w = dimEntry->StoredSize;
			}
			else if (IsYDimension(dimEntry->Dimension, sizeof(dimEntry->Dimension)))
			{
				sbd.logicalRect.y = dimEntry->Start;
				sbd.logicalRect.h = dimEntry->Size;
				sbd.physicalSize.h = dimEntry->StoredSize;
			}
			else
			{
				libCZI::DimensionIndex dim = CCZIParse::DimensionCharToDimensionIndex(dimEntry->Dimension, sizeof(dimEntry->Dimension));
				sbd.coordinate.Set(dim, dimEntry->Start);
			}
		}

		// calculate the "actual size" of the "entryDV" struct
		lengthSubblockSegmentData = 32 + subBlckSegment.data.entryDV.DimensionCount * sizeof(DimensionEntryDV);
	}
	else if (subBlckSegment.data.entrySchema[0] == 'D' && subBlckSegment.data.entrySchema[1] == 'E')
	{
		sbd.compression = subBlckSegment.data.entryDE.Compression;
		sbd.pixelType = subBlckSegment.data.entryDE.PixelType;

		lengthSubblockSegmentData = sizeof(SubBlockDirectoryEntryDE);
		// TODO...
	}
	else
	{
		CCZIParse::ThrowIllegalData(offset, "Invalid schema");
	}

	// the reserved size of the "subblock-segment-data" is given by "SIZE_SUBBLOCKDATA_MINIMUM", but the actual size of the
	//  data-structure (SubBlockDirectoryEntryDV) may be larger than that - so we need to take the max of the actual size and
	//  the reserved (minimal) size here
	lengthSubblockSegmentData = max(lengthSubblockSegmentData + SIZE_SUBBLOCKDATA_FIXEDPART, (uint32_t)SIZE_SUBBLOCKDATA_MINIMUM);

	// TODO: if subBlckSegment.data.DataSize > size_t (=4GB for 32Bit) then bail out gracefully
	auto deleter = [&](void* ptr) -> void {allocateInfo.free(ptr); };
	std::unique_ptr<void, decltype(deleter)> pMetadataBuffer(subBlckSegment.data.MetadataSize > 0 ? allocateInfo.alloc(subBlckSegment.data.MetadataSize) : nullptr, deleter);
	std::unique_ptr<void, decltype(deleter)> pDataBuffer(subBlckSegment.data.DataSize > 0 ? allocateInfo.alloc((size_t)subBlckSegment.data.DataSize) : nullptr, deleter);
	std::unique_ptr<void, decltype(deleter)> pAttachmentBuffer(subBlckSegment.data.AttachmentSize > 0 ? allocateInfo.alloc(subBlckSegment.data.AttachmentSize) : nullptr, deleter);

	// TODO: now get the information from the SubBlockDirectoryEntryDV/DE structure, and figure out their size
	// TODO: compare this information against the information from the SubBlock-directory
	if (pMetadataBuffer)
	{
		try
		{
			str->Read(offset + lengthSubblockSegmentData + sizeof(SegmentHeader), pMetadataBuffer.get(), subBlckSegment.data.MetadataSize, &bytesRead);
		}
		catch (const std::exception&)
		{
			std::throw_with_nested(LibCZIIOException("Error reading FileHeaderSegement", offset + lengthSubblockSegmentData + sizeof(SegmentHeader), subBlckSegment.data.MetadataSize));
		}

		if (bytesRead != subBlckSegment.data.MetadataSize)
		{
			CCZIParse::ThrowNotEnoughDataRead(offset + lengthSubblockSegmentData + sizeof(SegmentHeader), subBlckSegment.data.MetadataSize, bytesRead);
		}
	}

	if (pDataBuffer)
	{
		try
		{
			str->Read(offset + lengthSubblockSegmentData + sizeof(SegmentHeader) + subBlckSegment.data.MetadataSize, pDataBuffer.get(), subBlckSegment.data.DataSize, &bytesRead);
		}
		catch (const std::exception&)
		{
			std::throw_with_nested(LibCZIIOException("Error reading FileHeaderSegement", offset + lengthSubblockSegmentData + sizeof(SegmentHeader) + subBlckSegment.data.MetadataSize, subBlckSegment.data.DataSize));
		}

		if (bytesRead != subBlckSegment.data.DataSize)
		{
			CCZIParse::ThrowNotEnoughDataRead(offset + lengthSubblockSegmentData + sizeof(SegmentHeader) + subBlckSegment.data.MetadataSize, subBlckSegment.data.DataSize, bytesRead);
		}
	}

	if (pAttachmentBuffer)
	{
		try
		{
			str->Read(offset + lengthSubblockSegmentData + sizeof(SegmentHeader) + subBlckSegment.data.MetadataSize + subBlckSegment.data.DataSize, pAttachmentBuffer.get(), subBlckSegment.data.AttachmentSize, &bytesRead);
		}
		catch (const std::exception&)
		{
			std::throw_with_nested(LibCZIIOException("Error reading FileHeaderSegement", offset + lengthSubblockSegmentData + sizeof(SegmentHeader) + subBlckSegment.data.MetadataSize + subBlckSegment.data.DataSize, subBlckSegment.data.AttachmentSize));
		}

		if (bytesRead != subBlckSegment.data.AttachmentSize)
		{
			CCZIParse::ThrowNotEnoughDataRead(offset + lengthSubblockSegmentData + sizeof(SegmentHeader) + subBlckSegment.data.MetadataSize + subBlckSegment.data.DataSize, subBlckSegment.data.AttachmentSize, bytesRead);
		}
	}

	sbd.ptrData = pDataBuffer.release();
	sbd.dataSize = subBlckSegment.data.DataSize;
	sbd.ptrAttachment = pAttachmentBuffer.release();
	sbd.attachmentSize = subBlckSegment.data.AttachmentSize;
	sbd.ptrMetadata = pMetadataBuffer.release();
	sbd.metaDataSize = subBlckSegment.data.MetadataSize;
	return sbd;
}

/*static*/CCZIParse::AttachmentData CCZIParse::ReadAttachment(libCZI::IStream* str, std::uint64_t offset, const SubBlockStorageAllocate& allocateInfo)
{
	AttachmentSegment attchmntSegment;
	std::uint64_t bytesRead;
	try
	{
		str->Read(offset, &attchmntSegment, sizeof(attchmntSegment), &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading Attachment-Segment", offset, sizeof(attchmntSegment)));
	}

	if (bytesRead != sizeof(attchmntSegment))
	{
		CCZIParse::ThrowNotEnoughDataRead(offset, sizeof(attchmntSegment), bytesRead);
	}

	ConvertToHostByteOrder::Convert(&attchmntSegment);

	if (memcmp(attchmntSegment.header.Id, CCZIParse::ATTACHMENTBLKMAGIC, 16) != 0)
	{
		CCZIParse::ThrowIllegalData(offset, "Invalid Attachment-magic");
	}

	// TODO: if subBlckSegment.data.DataSize > size_t (=4GB for 32Bit) then bail out gracefully
	auto deleter = [&](void* ptr) -> void {allocateInfo.free(ptr); };
	std::unique_ptr<void, decltype(deleter)> pAttchmntBuffer(attchmntSegment.data.DataSize > 0 ? allocateInfo.alloc((size_t)attchmntSegment.data.DataSize) : nullptr, deleter);

	if (pAttchmntBuffer)
	{
		try
		{
			str->Read(offset + 256 + sizeof(SegmentHeader), pAttchmntBuffer.get(), attchmntSegment.data.DataSize, &bytesRead);
		}
		catch (const std::exception&)
		{
			std::throw_with_nested(LibCZIIOException("Error reading AttachmentSegement", offset + 256 + sizeof(SegmentHeader), attchmntSegment.data.DataSize));
		}

		if (bytesRead != attchmntSegment.data.DataSize)
		{
			CCZIParse::ThrowNotEnoughDataRead(offset + 256 + sizeof(SegmentHeader), attchmntSegment.data.DataSize, bytesRead);
		}
	}

	AttachmentData ad;
	ad.ptrData = pAttchmntBuffer.release();
	ad.dataSize = attchmntSegment.data.DataSize;
	return ad;
}

/*static*/void CCZIParse::ParseThroughDirectoryEntries(int count, std::function<void(int, void*)> funcRead, std::function<void(const SubBlockDirectoryEntryDE*, const SubBlockDirectoryEntryDV*)> funcAddEntry)
{
	for (int i = 0; i < count; ++i)
	{
		char schemaType[2];
		funcRead(2, schemaType);
		if (schemaType[0] == 'D' && schemaType[1] == 'V')
		{
			SubBlockDirectoryEntryDV dv;
			dv.SchemaType[0] = schemaType[0]; dv.SchemaType[1] = schemaType[1];
			funcRead(4 + 8 + 4 + 4 + 6 + 4, ((char*)&dv) + 2);
			ConvertToHostByteOrder::Convert(&dv);

			int sizeToRead = dv.DimensionCount * sizeof(DimensionEntryDV);
			// TODO: check for max size etc.
			funcRead(sizeToRead, &dv.DimensionEntries[0]);
			ConvertToHostByteOrder::Convert(&dv.DimensionEntries[0], dv.DimensionCount);

			funcAddEntry(nullptr, &dv);
		}
		else if (schemaType[0] == 'D' && schemaType[1] == 'E')
		{
			SubBlockDirectoryEntryDE de;
			funcRead(sizeof(de), &de);
			ConvertToHostByteOrder::Convert(&de);
			funcAddEntry(&de, nullptr);
		}
	}
}

/*static*/void CCZIParse::AddEntryToSubBlockDirectory(const SubBlockDirectoryEntryDE* subBlkDirDE, const std::function<void(const CCziSubBlockDirectoryBase::SubBlkEntry&)>& addFunc)
{
	// TODO
	throw std::logic_error("not (yet) implemented");
}

/*static*/void CCZIParse::AddEntryToSubBlockDirectory(const SubBlockDirectoryEntryDV* subBlkDirDE, const std::function<void(const CCziSubBlockDirectoryBase::SubBlkEntry&)>& addFunc)
{
	CCziSubBlockDirectory::SubBlkEntry entry;
	entry.Invalidate();

	// TODO: - add consistency checks like dimension appears twice, X and Y are not present ...
	for (int i = 0; i < subBlkDirDE->DimensionCount; ++i)
	{
		if (CCZIParse::IsXDimension(subBlkDirDE->DimensionEntries[i].Dimension, 4))
		{
			entry.x = subBlkDirDE->DimensionEntries[i].Start;
			entry.width = subBlkDirDE->DimensionEntries[i].Size;
			entry.storedWidth = subBlkDirDE->DimensionEntries[i].StoredSize;
		}
		else if (CCZIParse::IsYDimension(subBlkDirDE->DimensionEntries[i].Dimension, 4))
		{
			entry.y = subBlkDirDE->DimensionEntries[i].Start;
			entry.height = subBlkDirDE->DimensionEntries[i].Size;
			entry.storedHeight = subBlkDirDE->DimensionEntries[i].StoredSize;
		}
		else if (CCZIParse::IsMDimension(subBlkDirDE->DimensionEntries[i].Dimension, 4))
		{
			entry.mIndex = subBlkDirDE->DimensionEntries[i].Start;
		}
		else
		{
			libCZI::DimensionIndex dim = CCZIParse::DimensionCharToDimensionIndex(subBlkDirDE->DimensionEntries[i].Dimension, 4);
			entry.coordinate.Set(dim, subBlkDirDE->DimensionEntries[i].Start);
		}
	}

	entry.FilePosition = subBlkDirDE->FilePosition;
	entry.PixelType = subBlkDirDE->PixelType;
	entry.Compression = subBlkDirDE->Compression;

	addFunc(entry);
}

/*static*/CCZIParse::MetadataSegmentData CCZIParse::ReadMetadataSegment(libCZI::IStream* str, std::uint64_t offset, const SubBlockStorageAllocate& allocateInfo)
{
	MetadataSegment metadataSegment;
	std::uint64_t bytesRead;
	try
	{
		str->Read(offset, &metadataSegment, sizeof(metadataSegment), &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading MetaDataSegement", offset, sizeof(metadataSegment)));
	}

	if (bytesRead != sizeof(metadataSegment))
	{
		CCZIParse::ThrowNotEnoughDataRead(offset, sizeof(metadataSegment), bytesRead);
	}

	ConvertToHostByteOrder::Convert(&metadataSegment);

	if (memcmp(metadataSegment.header.Id, CCZIParse::METADATASEGMENTMAGIC, 16) != 0)
	{
		CCZIParse::ThrowIllegalData(offset, "Invalid MetadataSegment-magic");
	}

	// TODO: perform consistency checks...
	auto deleter = [&](void* ptr) -> void {allocateInfo.free(ptr); };
	std::unique_ptr<void, decltype(deleter)> pXmlBuffer(metadataSegment.data.XmlSize > 0 ? allocateInfo.alloc(metadataSegment.data.XmlSize) : nullptr, deleter);
	std::unique_ptr<void, decltype(deleter)> pAttachmentBuffer(metadataSegment.data.AttachmentSize > 0 ? allocateInfo.alloc((size_t)metadataSegment.data.AttachmentSize) : nullptr, deleter);
	if (pXmlBuffer)
	{
		try
		{
			str->Read(offset + sizeof(metadataSegment), pXmlBuffer.get(), metadataSegment.data.XmlSize, &bytesRead);
		}
		catch (const std::exception&)
		{
			std::throw_with_nested(LibCZIIOException("Error reading MetaDataSegement", offset + sizeof(metadataSegment), metadataSegment.data.XmlSize));
		}

		if (bytesRead != metadataSegment.data.XmlSize)
		{
			CCZIParse::ThrowNotEnoughDataRead(offset + sizeof(metadataSegment), metadataSegment.data.XmlSize, bytesRead);
		}
	}

	if (pAttachmentBuffer)
	{
		try
		{
			str->Read(offset + sizeof(metadataSegment) + metadataSegment.data.XmlSize, pAttachmentBuffer.get(), metadataSegment.data.AttachmentSize, &bytesRead);
		}
		catch (const std::exception&)
		{
			std::throw_with_nested(LibCZIIOException("Error reading MetaDataSegement", offset + sizeof(metadataSegment) + metadataSegment.data.XmlSize, metadataSegment.data.AttachmentSize));
		}

		if (bytesRead != metadataSegment.data.AttachmentSize)
		{
			CCZIParse::ThrowNotEnoughDataRead(offset + sizeof(metadataSegment) + metadataSegment.data.XmlSize, metadataSegment.data.AttachmentSize, bytesRead);
		}
	}

	MetadataSegmentData segData;
	segData.ptrXmlData = pXmlBuffer.release();
	segData.xmlDataSize = metadataSegment.data.XmlSize;
	segData.ptrAttachment = pAttachmentBuffer.release();
	segData.attachmentSize = metadataSegment.data.AttachmentSize;
	return segData;
}

/*static*/char CCZIParse::ToUpperCase(char c)
{
	if (isascii(c) != 0 && isupper(c) == 0)
	{
		return (char)toupper(c);
	}

	return c;
}

/*static*/libCZI::DimensionIndex CCZIParse::DimensionCharToDimensionIndex(const char* ptr, size_t size)
{
	if (size < 1)
	{
		CCZIParse::ThrowIllegalData("parameter 'size' is illegal");
	}

	static const struct CharAndDim
	{
		char dimChar;
		libCZI::DimensionIndex dimIndex;
	} CharAndDimList[] =
	{
		// important: this list must be sorted (ascending) for "dimChar"
		{ 'B', DimensionIndex::B },
		{ 'C', DimensionIndex::C },
		{ 'H', DimensionIndex::H },
		{ 'I', DimensionIndex::I },
		{ 'R', DimensionIndex::R },
		{ 'S', DimensionIndex::S },
		{ 'T', DimensionIndex::T },
		{ 'V', DimensionIndex::V },
		{ 'Z', DimensionIndex::Z }
	};

	const char c = CCZIParse::ToUpperCase(*ptr);
	const auto it = std::lower_bound(
		std::begin(CharAndDimList), 
		std::end(CharAndDimList), 
		c,
		[](const CharAndDim& val, char toSearch)->int {return val.dimChar < toSearch; });
	if (it != std::end(CharAndDimList) && !(c < it->dimChar))
	{
		return it->dimIndex;
	}

	CCZIParse::ThrowIllegalData("invalid dimension");
	return DimensionIndex::invalid;		// not reachable, just to silence compiler warning
}

/*static*/bool CCZIParse::IsMDimension(const char* ptr, size_t size)
{
	if (size < 1)
	{
		CCZIParse::ThrowIllegalData("parameter 'size' is illegal");
	}

	char c = CCZIParse::ToUpperCase(*ptr);
	return (c == 'M') ? true : false;
}

/*static*/bool CCZIParse::IsXDimension(const char* ptr, size_t size)
{
	if (size < 1)
	{
		CCZIParse::ThrowIllegalData("parameter 'size' is illegal");
	}

	char c = CCZIParse::ToUpperCase(*ptr);
	return (c == 'X') ? true : false;
}

/*static*/bool CCZIParse::IsYDimension(const char* ptr, size_t size)
{
	if (size < 1)
	{
		CCZIParse::ThrowIllegalData("parameter 'size' is illegal");
	}

	char c = CCZIParse::ToUpperCase(*ptr);
	return (c == 'Y') ? true : false;
}

/*static*/void CCZIParse::ThrowNotEnoughDataRead(std::uint64_t offset, std::uint64_t bytesRequested, std::uint64_t bytesActuallyRead)
{
	stringstream ss;
	ss << "Not enough data read at offset " << offset << " -> requested: " << bytesRequested << " bytes, actually got " << bytesActuallyRead << " bytes.";
	throw LibCZICZIParseException(ss.str().c_str(), LibCZICZIParseException::ErrorCode::NotEnoughData);
}

/*static*/void CCZIParse::ThrowIllegalData(std::uint64_t offset, const char* sz)
{
	stringstream ss;
	ss << "Illegal data detected at offset " << offset << " -> " << sz;
	throw LibCZICZIParseException(ss.str().c_str(), LibCZICZIParseException::ErrorCode::CorruptedData);
}

/*static*/void CCZIParse::ThrowIllegalData(const char* sz)
{
	stringstream ss;
	ss << "Illegal data detected -> " << sz;
	throw LibCZICZIParseException(ss.str().c_str(), LibCZICZIParseException::ErrorCode::CorruptedData);
}

/*static*/bool CCZIParse::CheckAttachmentSchemaType(const char* p, size_t cnt)
{
	assert(cnt >= 2);
	if (*p != 'A' || *(p + 1) != '1')
	{
		if (GetSite()->IsEnabled(LOGLEVEL_SEVEREWARNING))
		{
			GetSite()->Log(LOGLEVEL_SEVEREWARNING, "Unknown Attachment-Schema-Type encountered");
		}

		return false;
	}

	return true;
}

/*static*/CCZIParse::SegmentSizes CCZIParse::ReadSegmentHeader(CCZIParse::SegmentType type, libCZI::IStream* str, std::uint64_t pos)
{
	const std::uint8_t* pMagic = nullptr;
	switch (type)
	{
	case CCZIParse::SegmentType::SbBlkDirectory:
		pMagic = CCZIParse::SUBBLKDIRMAGIC;
		break;
	case CCZIParse::SegmentType::SbBlk:
		pMagic = CCZIParse::SUBBLKMAGIC;
		break;
	case CCZIParse::SegmentType::AttchmntDirectory:
		pMagic = CCZIParse::ATTACHMENTSDIRMAGC;
		break;
	case CCZIParse::SegmentType::Attachment:
		pMagic = CCZIParse::ATTACHMENTBLKMAGIC;
		break;
	case CCZIParse::SegmentType::Metadata:
		pMagic = CCZIParse::METADATASEGMENTMAGIC;
		break;
	default:
		throw std::logic_error("unknown SegmentType");
	}

	SegmentHeader segmentHdr;
	std::uint64_t bytesRead;
	try
	{
		str->Read(pos, &segmentHdr, sizeof(segmentHdr), &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading SegmentHeader", pos, sizeof(segmentHdr)));
	}

	if (bytesRead != sizeof(segmentHdr))
	{
		CCZIParse::ThrowNotEnoughDataRead(pos, sizeof(segmentHdr), bytesRead);
	}

	if (memcmp(segmentHdr.Id, pMagic, 16) != 0)
	{
		CCZIParse::ThrowIllegalData(pos, "Invalid SegmentHeader-magic");
	}

	ConvertToHostByteOrder::Convert(&segmentHdr);

	return SegmentSizes{ segmentHdr.AllocatedSize,segmentHdr.UsedSize };
}

/*static*/CCZIParse::SegmentSizes CCZIParse::ReadSegmentHeaderAny(libCZI::IStream* str, std::uint64_t pos)
{
	SegmentHeader segmentHdr;
	std::uint64_t bytesRead;
	try
	{
		str->Read(pos, &segmentHdr, sizeof(segmentHdr), &bytesRead);
	}
	catch (const std::exception&)
	{
		std::throw_with_nested(LibCZIIOException("Error reading SegmentHeader", pos, sizeof(segmentHdr)));
	}

	if (bytesRead != sizeof(segmentHdr))
	{
		CCZIParse::ThrowNotEnoughDataRead(pos, sizeof(segmentHdr), bytesRead);
	}

	ConvertToHostByteOrder::Convert(&segmentHdr);

	return SegmentSizes{ segmentHdr.AllocatedSize,segmentHdr.UsedSize };
}
