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

class MockMetadataSegment : public libCZI::IMetadataSegment
{
public:
	enum class Type
	{
		Data1,
		Data2,
		Data3,
		Data4,
		Data5,
		Data6,
		InvalidData
	};
private:
	std::string xmlData;
public:
	MockMetadataSegment(Type type = Type::Data1);
	virtual std::shared_ptr<const void> GetRawData(MemBlkType type, size_t* ptrSize);
	virtual void DangerousGetRawData(MemBlkType type, const void*& ptr, size_t& size) const;
};
