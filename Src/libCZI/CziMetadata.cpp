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
#include "CziMetadata.h"
#include "CziMetadataDocumentInfo.h"

using namespace pugi;
using namespace libCZI;
using namespace std;

CCziMetadata::CCziMetadata(libCZI::IMetadataSegment* pMdSeg)
{
	const void* ptrData; size_t size;
	pMdSeg->DangerousGetRawData(IMetadataSegment::MemBlkType::XmlMetadata, ptrData, size);
	this->parseResult = this->doc.load_buffer(ptrData, size, pugi::parse_default, encoding_utf8);
	if (this->parseResult)
	{
		this->wrapper = std::unique_ptr<XmlNodeWrapperReadonly<CCziMetadata, XmlNodeWrapperThrowExcp>>(
			new XmlNodeWrapperReadonly<CCziMetadata, XmlNodeWrapperThrowExcp>(this->doc.internal_object()));
	}
}

const pugi::xml_document& CCziMetadata::GetXmlDoc() const
{
	this->ThrowIfXmlInvalid();
	return this->doc;
}

/*virtual*/std::string CCziMetadata::GetXml()
{
	static pugi::char_t Indent[] = PUGIXML_TEXT(" ");

	this->ThrowIfXmlInvalid();

	std::ostringstream stream;
	xml_writer_stream writer(stream);
	this->doc.save(writer, Indent, format_default, encoding_utf8);
	stream.flush();
	return stream.str();
}

/*virtual*/bool CCziMetadata::IsXmlValid() const
{
	return this->parseResult;// .status == xml_parse_status::status_ok;
}

/*virtual*/std::shared_ptr<ICziMultiDimensionDocumentInfo> CCziMetadata::GetDocumentInfo()
{
	this->ThrowIfXmlInvalid();
	return std::make_shared<CCziMetadataDocumentInfo>(this->shared_from_this());
}

// interface IXmlNodeRead
/*virtual*/bool CCziMetadata::TryGetAttribute(const wchar_t* attributeName, std::wstring* attribValue) const
{
	this->ThrowIfXmlInvalid();
	return this->wrapper->TryGetAttribute(attributeName, attribValue);
}
/*virtual*/void CCziMetadata::EnumAttributes(const std::function<bool(const std::wstring& attribName, const std::wstring& attribValue)>& enumFunc) const
{
	this->ThrowIfXmlInvalid();
	this->wrapper->EnumAttributes(enumFunc);
}
/*virtual*/bool CCziMetadata::TryGetValue(std::wstring* value) const
{
	this->ThrowIfXmlInvalid();
	return this->wrapper->TryGetValue(value);
}
/*virtual*/std::shared_ptr<IXmlNodeRead> CCziMetadata::GetChildNodeReadonly(const char* path)
{
	this->ThrowIfXmlInvalid();
	return this->wrapper->GetChildNodeReadonly(path, this->shared_from_this());
}
/*virtual*/std::wstring CCziMetadata::Name() const
{
	this->ThrowIfXmlInvalid();
	return this->wrapper->Name();
}
void CCziMetadata::EnumChildren(std::function<bool(std::shared_ptr<IXmlNodeRead>)> enumChildren)
{
	this->ThrowIfXmlInvalid();
	return this->wrapper->EnumChildren(enumChildren, this->shared_from_this());
}

void CCziMetadata::ThrowIfXmlInvalid() const
{
	if (!this->IsXmlValid())
	{
		stringstream ss;
		ss << "Error parsing XML [offset " << this->parseResult.offset << "]: " << this->parseResult.description();
		throw LibCZIXmlParseException(ss.str().c_str());
	}
}

//---------------------------------------------------------------------------------

template<typename t>
static bool TryConvert(IXmlNodeRead* node, typename t::type* p)
{
	wstring str;
	if (node->TryGetValue(&str))
	{
		std::size_t posFirstCharNotParsed;
		typename t::type i;
		try
		{
			i = t::convert(str, &posFirstCharNotParsed);

			// where we stopped parsing, we expect to find only spaces - otherwise we have a parse-error and return false
			while (posFirstCharNotParsed < str.length())
			{
				if (!isspace(str[posFirstCharNotParsed]))
				{
					return false;
				}
			}
		}
		catch (invalid_argument&)
		{
			return false;
		}
		catch (out_of_range&)
		{
			return false;
		}

		if (p != nullptr)
		{
			*p = i;
		}

		return true;
	}

	return false;
}

bool IXmlNodeRead::TryGetValueAsDouble(double* p)
{
	struct conv
	{
		typedef double type;
		static double convert(const wstring& s, size_t* idx)
		{
			wchar_t* endPtr;
			auto d = wcstod(s.c_str(), &endPtr);
			*idx = endPtr - s.c_str();
			return d;
		}
	};

	return TryConvert<conv>(this, p);
}

bool IXmlNodeRead::TryGetValueAsFloat(float* p)
{
	double d;
	const bool b = this->TryGetValueAsDouble(&d);
	if (b == false)
	{
		return false;
	}

	if (p != nullptr)
	{
		*p = (float)d;
	}

	return true;
}

bool IXmlNodeRead::TryGetValueAsUInt32(std::uint32_t* p)
{
	struct conv
	{
		typedef uint32_t type;
		static uint32_t convert(const wstring& s, size_t* idx)
		{
			// unfortunately, on some platforms (Linux in this case) "stoul" does not seem to throw an out_of_range-exception,
			//  therefore we have this work-around
			uint64_t v = stoull(s, idx);
			if (v > (numeric_limits<uint32_t>::max)())
			{
				throw out_of_range("value was out-of-range");
			}

			return (uint32_t)v;
		}
	};

	return TryConvert<conv>(this, p);
}

bool IXmlNodeRead::TryGetValueAsInt32(std::int32_t* p)
{
	struct conv
	{
		typedef int32_t type;
		static int32_t convert(const wstring& s, size_t* idx)
		{
			return stoi(s, idx);
		}
	};

	return TryConvert<conv>(this, p);
}

bool IXmlNodeRead::TryGetValueAsInt64(std::int64_t* p)
{
	struct conv
	{
		typedef int64_t type;
		static int64_t convert(const wstring& s, size_t* idx)
		{
			return stoll(s, idx);
		}
	};

	return TryConvert<conv>(this, p);
}

bool IXmlNodeRead::TryGetValueAsUInt64(std::uint64_t* p)
{
	struct conv
	{
		typedef uint64_t type;
		static uint64_t convert(const wstring& s, size_t* idx)
		{
			return stoull(s, idx);
		}
	};

	return TryConvert<conv>(this, p);
}

bool IXmlNodeRead::TryGetValueAsBool(bool* p)
{
	wstring str;
	if (this->TryGetValue(&str))
	{
		str = Utilities::Trim(str);
		bool b;
		if (Utilities::icasecmp(str, L"yes") || Utilities::icasecmp(str, L"true") || Utilities::icasecmp(str, L"1"))
		{
			b = true;
		}
		else if (Utilities::icasecmp(str, L"no") || Utilities::icasecmp(str, L"false") || Utilities::icasecmp(str, L"0"))
		{
			b = false;
		}
		else
		{
			return false;
		}

		if (p != nullptr)
		{
			*p = b;
		}

		return true;
	}

	return false;
}