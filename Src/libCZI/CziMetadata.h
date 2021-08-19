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
#include "pugixml.hpp"
#include "XmlNodeWrapper.h"
#include <memory>

class CCziMetadata : public libCZI::ICziMetadata, public std::enable_shared_from_this<CCziMetadata>
{
private:
	struct XmlNodeWrapperThrowExcp
	{
		static void ThrowInvalidPath()
		{
			throw libCZI::LibCZIMetadataException("invalid path", libCZI::LibCZIMetadataException::ErrorType::InvalidPath);
		}
	};

	pugi::xml_parse_result   parseResult;
	pugi::xml_document doc;
	std::unique_ptr<XmlNodeWrapperReadonly<CCziMetadata, XmlNodeWrapperThrowExcp> > wrapper;
public:
	CCziMetadata(libCZI::IMetadataSegment* pMdSeg);

public:
	const pugi::xml_document& GetXmlDoc() const;

public:	// interface ICziMetadata
	virtual bool IsXmlValid() const;
	virtual std::string GetXml();
	virtual std::shared_ptr<libCZI::ICziMultiDimensionDocumentInfo> GetDocumentInfo();

public: // interface IXmlNodeRead
	virtual bool TryGetAttribute(const wchar_t* attributeName, std::wstring* attribValue) const;
	virtual void EnumAttributes(const std::function<bool(const std::wstring& attribName, const std::wstring& attribValue)>& enumFunc) const;
	virtual bool TryGetValue(std::wstring* value) const;
	virtual std::shared_ptr<IXmlNodeRead> GetChildNodeReadonly(const char* path);
	virtual std::wstring Name() const;
	virtual void EnumChildren(std::function<bool(std::shared_ptr<IXmlNodeRead>)> enumChildren);
private:
	void ThrowIfXmlInvalid() const;
};
