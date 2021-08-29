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
#include "CziMetadataBuilder.h"
#include "utilities.h"
#include <regex>
#include <codecvt>
#include <iomanip>
#include "CziUtils.h"
#include "XmlNodeWrapper.h"

using namespace pugi;
using namespace libCZI;
using namespace std;

/*static*/void CNodeWrapper::MetadataBuilderXmlNodeWrapperThrowExcp::ThrowInvalidPath()
{
	throw libCZI::LibCZIMetadataBuilderException("invalid path", libCZI::LibCZIMetadataBuilderException::ErrorType::InvalidPath);
}

/*virtual*/std::shared_ptr<libCZI::IXmlNodeRw> CNodeWrapper::AppendChildNode(const char* name)
{
	auto node = this->node.append_child(xml_node_type::node_element);
	node.set_name(Utilities::convertUtf8ToWchar_t(name).c_str());
	return make_shared<CNodeWrapper>(this->builderRef, node.internal_object());
}

/*virtual*/bool CNodeWrapper::TryGetAttribute(const wchar_t* attributeName, std::wstring* attribValue) const
{
	auto attrib = this->node.find_attribute([&](const xml_attribute & a)
	{
		return wcscmp(a.name(), attributeName) == 0;
	});

	if (!attrib)
	{
		return false;
	}

	if (attribValue != nullptr)
	{
		*attribValue = attrib.value();
	}

	return true;
}

/*virtual*/void CNodeWrapper::EnumAttributes(const std::function<bool(const std::wstring& attribName, const std::wstring& attribValue)>& enumFunc) const
{
	for (pugi::xml_attribute_iterator ait = this->node.attributes_begin(); ait != this->node.attributes_end(); ++ait)
	{
		if (!enumFunc(wstring(ait->name()), wstring(ait->value())))
		{
			break;
		}
	}
}

/*virtual*/bool CNodeWrapper::TryGetValue(std::wstring* value) const
{
	if (this->node.first_child().type() == xml_node_type::node_pcdata)
	{
		if (value != nullptr)
		{
			*value = this->node.child_value();
		}

		return true;
	}

	return false;
}

/*virtual*/std::shared_ptr<IXmlNodeRead> CNodeWrapper::GetChildNodeReadonly(const char* path)
{
	auto p = Utilities::convertUtf8ToWchar_t(path);
	vector<std::wstring> tokens;
	Utilities::Tokenize(p, tokens, L"/");
	if (tokens.empty())
	{
		throw LibCZIMetadataBuilderException("invalid path", LibCZIMetadataBuilderException::ErrorType::InvalidPath);
	}

	if (any_of(tokens.cbegin(), tokens.cend(), [](const wstring & str) {return str.empty(); }))
	{
		throw LibCZIMetadataBuilderException("invalid path", LibCZIMetadataBuilderException::ErrorType::InvalidPath);
	}

	auto node = this->GetOrCreateChildElementNodeWithAttributes(tokens[0], false);
	if (!node)
	{
		return nullptr;
	}

	for (size_t i = 1; i < tokens.size(); ++i)
	{
		node = CNodeWrapper::GetOrCreateChildElementNodeWithAttributes(node, tokens[i].c_str(), false);
		if (!node)
		{
			return nullptr;
		}
	}

	return std::make_shared<XmlNodeWrapperReadonly<CCZiMetadataBuilder, MetadataBuilderXmlNodeWrapperThrowExcp> >(this->builderRef, node.internal_object());
}

/*virtual*/void CNodeWrapper::EnumChildren(std::function<bool(std::shared_ptr<IXmlNodeRead>)> enumChildren)
{
	for (pugi::xml_node childNode = this->node.first_child(); childNode; childNode = childNode.next_sibling())
	{
		if (childNode.type() == pugi::xml_node_type::node_element)
		{
			bool b = enumChildren(
				std::make_shared<XmlNodeWrapperReadonly<CCZiMetadataBuilder, MetadataBuilderXmlNodeWrapperThrowExcp> >(this->builderRef, childNode.internal_object()));
			if (!b)
			{
				break;
			}
		}
	}
}

/*virtual*/std::wstring CNodeWrapper::Name() const
{
	return this->node.name();
}

/*virtual*/std::shared_ptr<IXmlNodeRw> CNodeWrapper::GetOrCreateChildNode(const char* path)
{
	return this->GetOrCreateChildNode(path, true);
}

/*virtual*/std::shared_ptr<IXmlNodeRw> CNodeWrapper::GetChildNode(const char* path)
{
	return this->GetOrCreateChildNode(path, false);
}

/*virtual*/void CNodeWrapper::SetValue(const char* str)
{
	this->ThrowIfCannotSetValue();
	auto pcdata = this->GetOrCreatePcDataChild();
	pcdata.set_value(Utilities::convertUtf8ToWchar_t(str).c_str());
}

/*virtual*/void CNodeWrapper::SetValue(const wchar_t* str)
{
	this->ThrowIfCannotSetValue();
	auto pcdata = this->GetOrCreatePcDataChild();
	pcdata.set_value(str);
}

/*virtual*/void CNodeWrapper::SetValueI32(int value)
{
	this->SetValueT<int>(value);
}

/*virtual*/void CNodeWrapper::SetValueUI32(unsigned int value)
{
	this->SetValueT<unsigned int>(value);
}

/*virtual*/void CNodeWrapper::SetValueI64(long long value)
{
	this->SetValueT<long long>(value);
}

/*virtual*/void CNodeWrapper::SetValueUI64(unsigned long long value)
{
	this->SetValueT<unsigned long long>(value);
}

/*virtual*/void CNodeWrapper::SetValueBool(bool value)
{
	this->SetValue(value == true ? "true" : "false");
}

/*virtual*/void CNodeWrapper::SetValueDbl(double value)
{
	this->ThrowIfCannotSetValue();
	wstringstream ss;
	ss << std::setprecision(numeric_limits<double>::digits10) << value;
	auto pcdata = this->GetOrCreatePcDataChild();
	pcdata.set_value(ss.str().c_str());
}

/*virtual*/void CNodeWrapper::SetValueFlt(float value)
{
	this->ThrowIfCannotSetValue();
	wstringstream ss;
	ss << std::setprecision(numeric_limits<float>::digits10) << value;
	auto pcdata = this->GetOrCreatePcDataChild();
	pcdata.set_value(ss.str().c_str());
}

//--------------------------------------------------------------------------------------

std::shared_ptr<IXmlNodeRw> CNodeWrapper::GetOrCreateChildNode(const char* path, bool allowCreation)
{
	auto p = Utilities::convertUtf8ToWchar_t(path);

	vector<std::wstring> tokens;
	Utilities::Tokenize(p, tokens, L"/");
	if (tokens.empty())
	{
		throw LibCZIMetadataBuilderException("invalid path", LibCZIMetadataBuilderException::ErrorType::InvalidPath);
	}

	if (any_of(tokens.cbegin(), tokens.cend(), [](const wstring & str) {return str.empty(); }))
	{
		throw LibCZIMetadataBuilderException("invalid path", LibCZIMetadataBuilderException::ErrorType::InvalidPath);
	}

	auto node = this->GetOrCreateChildElementNodeWithAttributes(tokens[0], allowCreation);
	if (!node)
	{
		return nullptr;
	}

	for (size_t i = 1; i < tokens.size(); ++i)
	{
		node = CNodeWrapper::GetOrCreateChildElementNodeWithAttributes(node, tokens[i].c_str(), allowCreation);
		if (!node)
		{
			return nullptr;
		}
	}

	return std::make_shared<CNodeWrapper>(this->builderRef, node.internal_object());
}

void CNodeWrapper::ThrowIfCannotSetValue()
{
	auto node = this->node.find_child([=](const xml_node & n) {return n.type() == xml_node_type::node_element; });
	if (!node)
	{
		return;
	}

	throw LibCZIMetadataBuilderException("child-nodes present, cannot set value", LibCZIMetadataBuilderException::ErrorType::CannotSetValueToNode);
}

pugi::xml_node CNodeWrapper::GetOrCreatePcDataChild()
{
	auto node = this->node.find_child([=](const xml_node & n) {return n.type() == xml_node_type::node_pcdata; });
	if (!node)
	{
		node = this->node.append_child(xml_node_type::node_pcdata);
	}

	return node;
}

/*virtual*/void CNodeWrapper::SetAttribute(const char* name, const char* value)
{
	auto attr = this->node.append_attribute(Utilities::convertUtf8ToWchar_t(name).c_str());
	attr.set_value(Utilities::convertUtf8ToWchar_t(value).c_str());
}

pugi::xml_node CNodeWrapper::GetOrCreateChildElementNodeWithAttributes(const std::wstring & str, bool allowCreation)
{
	return CNodeWrapper::GetOrCreateChildElementNodeWithAttributes(this->node, str, allowCreation);
}

/*static*/pugi::xml_node CNodeWrapper::GetOrCreateChildElementNodeWithAttributes(pugi::xml_node & node, const std::wstring & str, bool allowCreation)
{
	std::wregex nodenameWihtAttribregex(LR"(([^\[\]]+)(\[([^\[\]]*)\])?)");
	std::wsmatch pieces_match;

	if (std::regex_match(str, pieces_match, nodenameWihtAttribregex))
	{
		if (pieces_match.size() == 4)
		{
			if (pieces_match[0].matched == true && pieces_match[1].matched == true)
			{
				wstring nodeName = pieces_match[1];
				if (pieces_match[2].matched == false && pieces_match[3].matched == false)
				{
					// we only got a name ( not followed by [Id=abc] )
					return GetOrCreateChildElementNode(node, nodeName.c_str(), allowCreation);
				}
				else if (pieces_match[2].matched == true && pieces_match[3].matched == true)
				{
					const auto attribValuePairs = CNodeWrapper::ParseAttributes(pieces_match[3]);
					return GetOrCreateChildElementNodeWithAttributes(node, nodeName, attribValuePairs, allowCreation);
				}
			}
		}
	}

	throw LibCZIMetadataBuilderException("invalid path", LibCZIMetadataBuilderException::ErrorType::InvalidPath);
}

pugi::xml_node CNodeWrapper::GetOrCreateChildElementNode(const wchar_t* sz, bool allowCreation)
{
	return CNodeWrapper::GetOrCreateChildElementNode(this->node, sz, allowCreation);
}

/*static*/pugi::xml_node CNodeWrapper::GetOrCreateChildElementNodeWithAttributes(pugi::xml_node & node, const std::wstring & str, const std::map<std::wstring, std::wstring> & attribs, bool allowCreation)
{
	struct find_element_node_and_attributes
	{
		const wchar_t* nodeName;
		const std::map<std::wstring, std::wstring>& attribs;

		bool operator()(pugi::xml_node node) const
		{
			if (node.type() != xml_node_type::node_element)
			{
				return false;
			}

			if (wcscmp(node.name(), this->nodeName) != 0)
			{
				return false;
			}

			for (auto it = this->attribs.cbegin(); it != this->attribs.cend(); ++it)
			{
				auto attribute = node.find_attribute([&](const xml_attribute & a)
				{
					return wcscmp(a.name(), it->first.c_str()) == 0;
				});

				if (!attribute)
				{
					return false;
				}

				if (wcscmp(attribute.value(), it->second.c_str()) != 0)
				{
					return false;
				}
			}

			return true;
		}
	};

	auto c = node.find_child(find_element_node_and_attributes{ str.c_str(), attribs });
	if (!c)
	{
		if (!allowCreation)
		{
			return xml_node();
		}

		auto newNode = node.append_child(str.c_str());
		for (auto it = attribs.cbegin(); it != attribs.cend(); ++it)
		{
			newNode.append_attribute(it->first.c_str()).set_value(it->second.c_str());
		}

		return newNode;
	}

	return c;
}

/*static*/pugi::xml_node CNodeWrapper::GetOrCreateChildElementNode(pugi::xml_node & node, const wchar_t* sz, bool allowCreation)
{
	struct find_element_node
	{
		const wchar_t* nodeName;

		bool operator()(pugi::xml_node node) const
		{
			if (node.type() != xml_node_type::node_element)
			{
				return false;
			}

			return wcscmp(node.name(), this->nodeName) == 0;
		}
	};

	auto c = node.find_child(find_element_node{ sz });
	if (!c)
	{
		if (!allowCreation)
		{
			return xml_node();
		}

		return node.append_child(sz);
	}

	return c;
}

/*static*/std::map<wstring, wstring> CNodeWrapper::ParseAttributes(const std::wstring & str)
{
	map<wstring, wstring> attribMap;

	std::wsmatch pieces_match;

	// now we have string of the form Id=abc,Name=abc
	vector<wstring> pairs;
	Utilities::Tokenize(str, pairs, L",;");

	std::wregex attribValuePairRegex(LR"(([^=]+)=([^,;]*))");
	for (auto it = pairs.cbegin(); it != pairs.cend(); ++it)
	{
		bool parsedOk = false;
		if (std::regex_match(*it, pieces_match, attribValuePairRegex))
		{
			if (pieces_match.size() == 3 && pieces_match[0].matched == true && pieces_match[1].matched == true && pieces_match[2].matched == true)
			{
				attribMap.insert(pair<wstring, wstring>(pieces_match[1], pieces_match[2]));
				parsedOk = true;
			}
		}

		if (!parsedOk)
		{
			throw LibCZIMetadataBuilderException("invalid path", LibCZIMetadataBuilderException::ErrorType::InvalidPath);
		}
	}

	return attribMap;
}

//----------------------------------------------------------------------------------------------

CCZiMetadataBuilder::CCZiMetadataBuilder(const wchar_t* rootNodeName)
{
	auto n = this->metadataDoc.append_child(xml_node_type::node_element);
	n.set_name(rootNodeName);
	this->rootNode = n;
}

/*virtual*/CCZiMetadataBuilder::~CCZiMetadataBuilder()
{
}

/*virtual*/std::shared_ptr<libCZI::IXmlNodeRw> CCZiMetadataBuilder::GetRootNode()
{
	return std::make_shared<CNodeWrapper>(this->shared_from_this(), this->rootNode.internal_object());
}

/*virtual*/std::string CCZiMetadataBuilder::GetXml(bool withIndent/*=false*/)
{
	struct xml_string_writer : pugi::xml_writer
	{
		std::string result;
		virtual void write(const void* data, size_t size)
		{
			result.append(static_cast<const char*>(data), size);
		}
	} writer;

	unsigned flags = format_no_declaration | (withIndent ? format_indent : format_raw);
	this->metadataDoc.save(writer, L"  ", flags, xml_encoding::encoding_utf8);
	return writer.result;
}

//**************************************************************************************************

/*static*/void CMetadataPrepareHelper::FillDimensionChannel(
	libCZI::ICziMetadataBuilder * builder,
	const libCZI::SubBlockStatistics & statistics,
	const PixelTypeForChannelIndexStatistic & pixelTypeForChannel,
	std::function<std::tuple<std::string, std::tuple<bool, std::string>>(int channelIdx)> getIdAndName)
{
	int cStartIdx, cIdxSize;
	if (statistics.dimBounds.TryGetInterval(DimensionIndex::C, &cStartIdx, &cIdxSize))
	{
		FillDimensionChannel(builder, cStartIdx, cIdxSize, pixelTypeForChannel, getIdAndName);
	}
	else
	{
		int pxlTypeInt;
		if (pixelTypeForChannel.TryGetPixelTypeForNoChannelIndex(&pxlTypeInt))
		{
			FillImagePixelType(builder, CziUtils::PixelTypeFromInt(pxlTypeInt));
		}
	}
}

/*static*/void CMetadataPrepareHelper::FillDimensionChannel(
	libCZI::ICziMetadataBuilder * builder,
	int channelIdxStart, int channelIdxSize,
	const PixelTypeForChannelIndexStatistic & pixelTypeForChannel,
	std::function<std::tuple<std::string, std::tuple<bool, std::string>>(int channelIdx)> getIdAndName)
{
	auto root = builder->GetRootNode();

	PixelType pxlTypeForImagePixelType = PixelType::Invalid;
	for (int ch = channelIdxStart; ch < channelIdxStart + channelIdxSize; ++ch)
	{
		auto idAndOptionalName = getIdAndName(ch);
		string s{ "Metadata/Information/Image/Dimensions/Channels/Channel[Id=" };
		s.append(get<0>(idAndOptionalName));
		if (get<0>(get<1>(idAndOptionalName)) == true)
		{
			s.append(",Name=");
			s.append(get<1>(get<1>(idAndOptionalName)));
		}

		s.append("]");

		auto channelNode = root->GetOrCreateChildNode(s.c_str());

		auto pxlTypeIterator = pixelTypeForChannel.GetChannelIndexPixelTypeMap().find(ch);
		if (pxlTypeIterator != pixelTypeForChannel.GetChannelIndexPixelTypeMap().end())
		{
			int pxlTypeInt = pxlTypeIterator->second;
			string pixelTypeString;;
			PixelType pxlType = CziUtils::PixelTypeFromInt(pxlTypeInt);
			if (CMetadataPrepareHelper::TryConvertToXmlMetadataPixelTypeString(pxlType, pixelTypeString))
			{
				auto pixelTypeNode = channelNode->AppendChildNode("PixelType");
				pixelTypeNode->SetValue(pixelTypeString.c_str());
			}

			if (pxlType != PixelType::Invalid && pxlTypeForImagePixelType == PixelType::Invalid)
			{
				pxlTypeForImagePixelType = pxlType;
			}
		}
	}

	// determine what to write into Image/PixelType - which is vaguely defined, so for the time being, let's just
	// use the first channel
	CMetadataPrepareHelper::FillImagePixelType(builder, pxlTypeForImagePixelType);
}

/*static*/void CMetadataPrepareHelper::FillImagePixelType(libCZI::ICziMetadataBuilder * builder, libCZI::PixelType pxlType)
{
	string pixelTypeString;;
	if (CMetadataPrepareHelper::TryConvertToXmlMetadataPixelTypeString(pxlType, pixelTypeString))
	{
		auto node = builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Image/PixelType");
		node->SetValue(pixelTypeString.c_str());
	}
}

/*static*/bool CMetadataPrepareHelper::TryConvertToXmlMetadataPixelTypeString(libCZI::PixelType pxlType, std::string & str)
{
	switch (pxlType)
	{
	case PixelType::Gray8:
		str = "Gray8";
		break;
	case PixelType::Gray16:
		str = "Gray16";
		break;
	case PixelType::Gray32Float:
		str = "Gray32Float";
		break;
	case PixelType::Bgr24:
		str = "Bgr24";
		break;
	case PixelType::Bgr48:
		str = "Bgr48";
		break;
	case PixelType::Bgr96Float:
		str = "Bgr96Float";
		break;
	case PixelType::Bgra32:
		str = "Bgra32";
		break;
	case PixelType::Gray64ComplexFloat:
		str = "Gray64ComplexFloat";
		break;
	case PixelType::Bgr192ComplexFloat:
		str = "Bgr192ComplexFloat";
		break;
	default:
		return false;
	};

	return true;
}

//**************************************************************************************************

std::string libCZI::XmlDateTime::ToXmlString() const
{
	if (!this->IsValid())
	{
		throw std::invalid_argument("illegal date/time");
	}

	stringstream ss;
	ss << setfill('0') << setw(4) << this->year << '-' << setw(2) << this->mon << '-' << setw(2) << this->mday << 'T'
		<< setw(2) << this->hour << ':' << setw(2) << this->min << ':' << setw(2) << this->sec;
	if (this->isUTC == true)
	{
		ss << 'Z';
	}
	else if (this->HasTimeZoneOffset())
	{
		ss << ((this->offsetHours >= 0) ? "+" : "") << setw(2) << this->offsetHours << ':' << setw(2) << this->offsetMinutes;
	}

	return ss.str();
}

std::wstring libCZI::XmlDateTime::ToXmlWstring() const
{
	return Utilities::convertUtf8ToWchar_t(this->ToXmlString().c_str());
}

bool libCZI::XmlDateTime::IsValid() const
{
	return
		this->sec >= 0 && this->sec <= 60 &&
		this->min >= 0 && this->min <= 59 &&
		this->hour >= 0 && this->hour <= 23 &&
		this->year > -10000 && this->year < 10000 &&
		this->mon >= 1 && this->mon <= 12 &&
		this->mday >= 1 &&
		this->mday <= (this->mon == 2 ? (((this->year % 100 != 0 && this->year % 4 == 0) || (this->year % 400 == 0)) ? 29 : 28) :
			this->mon == 9 || this->mon == 4 || this->mon == 6 || this->mon == 11 ? 30 : 31);
}

/*static*/bool libCZI::XmlDateTime::TryParse(const wchar_t* szw, libCZI::XmlDateTime* ptrDateTime)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
	return XmlDateTime::TryParse(utf8_conv.to_bytes(szw).c_str(), ptrDateTime);
}

/*static*/bool libCZI::XmlDateTime::TryParse(const char* sz, libCZI::XmlDateTime* ptrDateTime)
{
	// cf. https://www.oreilly.com/library/view/regular-expressions-cookbook/9781449327453/ch04s07.html
	//static const char* regexStr = "^(?<year>-?(?:[1-9][0-9]*)?[0-9]{4})-(?<month>1[0-2]|0[1-9])-(?<day>3[01]|0[1-9]|[12][0-9])T(?<hour>2[0-3]|[01][0-9]):(?<minute>[0-5][0-9]):(?<second>[0-5][0-9])(?<ms>\.[0-9]+)?(?<timezone>Z|[+-](?:2[0-3]|[01][0-9]):[0-5][0-9])?$";
	static const char* regexStr = R"((-?(?:[1-9][0-9]*)?[0-9]{4})-(1[0-2]|0[1-9])-(3[01]|0[1-9]|[12][0-9])T(2[0-3]|[01][0-9]):([0-5][0-9]):([0-5][0-9])(\.[0-9]+)?(Z|[+-](?:2[0-3]|[01][0-9]):[0-5][0-9])?)";

	auto str = Utilities::Trim(sz);
	regex dateTimeRegex(regexStr);
	smatch pieces_match;
	if (regex_match(str, pieces_match, dateTimeRegex) && pieces_match.size() == 9)
	{
		XmlDateTime dateTime;
		dateTime.Clear();

		if (pieces_match[1].matched)
		{
			dateTime.year = std::stoi(pieces_match[1]);
		}

		if (pieces_match[2].matched)
		{
			dateTime.mon = std::stoi(pieces_match[2]);
		}

		if (pieces_match[3].matched)
		{
			dateTime.mday = std::stoi(pieces_match[3]);
		}

		if (pieces_match[4].matched)
		{
			dateTime.hour = std::stoi(pieces_match[4]);
		}

		if (pieces_match[5].matched)
		{
			dateTime.min = std::stoi(pieces_match[5]);
		}

		if (pieces_match[6].matched)
		{
			dateTime.sec = std::stoi(pieces_match[6]);
		}

		// note that we skip the decimal places of "second"...

		if (pieces_match[8].matched)
		{
			auto str = pieces_match[8].str();
			char _1stChar = str[0];
			if (_1stChar == 'Z')
			{
				dateTime.isUTC = true;
			}
			else if ((_1stChar == '+' || _1stChar == '-') && str.length() >= 6)
			{
				dateTime.offsetHours = std::stoi(str.substr(0, 3));
				dateTime.offsetMinutes = std::stoi(str.substr(4, 2));
			}
		}

		if (dateTime.IsValid())
		{
			if (ptrDateTime != nullptr)
			{
				*ptrDateTime = dateTime;
			}

			return true;
		}
	}

	return false;
}

/*static*/void libCZI::MetadataUtils::WriteImageSizeInformation(libCZI::ICziMetadataBuilder * builder, int width, int height)
{
	auto root = builder->GetRootNode();
	root->GetOrCreateChildNode("Metadata/Information/Image/SizeX")->SetValueI32(width);
	root->GetOrCreateChildNode("Metadata/Information/Image/SizeY")->SetValueI32(height);
}

/*static*/void libCZI::MetadataUtils::WriteMIndexSizeInformation(libCZI::ICziMetadataBuilder * builder, int mSize)
{
	auto root = builder->GetRootNode();
	root->GetOrCreateChildNode("Metadata/Information/Image/SizeM")->SetValueI32(mSize);
}

/*static*/void libCZI::MetadataUtils::WriteDimensionSize(libCZI::ICziMetadataBuilder * builder, libCZI::DimensionIndex dim, int size)
{
	string s{ "Metadata/Information/Image/Size_" };
	*(s.end() - 1) = Utils::DimensionToChar(dim);
	auto root = builder->GetRootNode();
	root->GetOrCreateChildNode(s.c_str())->SetValueI32(size);
}

/*static*/void libCZI::MetadataUtils::WriteFillWithSubBlockStatistics(libCZI::ICziMetadataBuilder * builder, const libCZI::SubBlockStatistics & statistics)
{
	MetadataUtils::WriteImageSizeInformation(builder, statistics.boundingBox.w, statistics.boundingBox.h);
	statistics.dimBounds.EnumValidDimensions(
		[&](libCZI::DimensionIndex dim, int start, int size)->bool
	{
		MetadataUtils::WriteDimensionSize(builder, dim, size);
		return true;
	});

	if (statistics.IsMIndexValid())
	{
		MetadataUtils::WriteMIndexSizeInformation(builder, statistics.maxMindex - statistics.minMindex + 1);
	}
}

/*static*/void libCZI::MetadataUtils::WriteDimInfoT_Interval(ICziMetadataBuilder * builder, const XmlDateTime * startTime, double startOffSet, double increment)
{
	if (startTime != nullptr)
	{
		auto s = startTime->ToXmlString();
		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/T/StartTime")->SetValue(s.c_str());
	}

	if (!isnan(startOffSet) && !isinf(startOffSet))
	{
		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/T/Positions/Interval/Start")->SetValueDbl(startOffSet);
	}

	if (!isnan(increment) && !isinf(increment))
	{
		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/T/Positions/Interval/Increment")->SetValueDbl(increment);
	}
}

/*static*/void libCZI::MetadataUtils::WriteDimInfoT_List(ICziMetadataBuilder * builder, const XmlDateTime * startTime, std::function<double(int)> funcGetOffsets)
{
	if (startTime != nullptr)
	{
		auto s = startTime->ToXmlString();
		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/T/StartTime")->SetValue(s.c_str());
	}

	if (funcGetOffsets)
	{
		stringstream ss;
		for (int i = 0;; ++i)
		{
			double v = funcGetOffsets(i);
			if (isnan(v) || isinf(v))
			{
				break;
			}

			if (i > 0)
			{
				ss << ' ';
			}

			ss << std::setprecision(numeric_limits<double>::digits10) << v;
		}

		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/T/Positions/List/Offsets")->SetValue(ss.str().c_str());
	}
}

/*static*/void libCZI::MetadataUtils::WriteDimInfoZ_Interval(ICziMetadataBuilder * builder, double startPos, double startOffSet, double increment)
{
	if (!isnan(startPos) && !isinf(startPos))
	{
		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/Z/StartPosition")->SetValueDbl(startPos);
	}

	if (!isnan(startOffSet) && !isinf(startOffSet))
	{
		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/Z/Positions/Interval/Start")->SetValueDbl(startOffSet);
	}

	if (!isnan(increment) && !isinf(increment))
	{
		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/Z/Positions/Interval/Increment")->SetValueDbl(increment);
	}
}

/*static*/void libCZI::MetadataUtils::WriteDimInfoZ_List(ICziMetadataBuilder * builder, double startPos, std::function<double(int)> funcGetOffsets)
{
	if (!isnan(startPos) && !isinf(startPos))
	{
		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/T/StartTime")->SetValueDbl(startPos);
	}

	if (funcGetOffsets)
	{
		stringstream ss;
		for (int i = 0;; ++i)
		{
			double v = funcGetOffsets(i);
			if (isnan(v) || isinf(v))
			{
				break;
			}

			if (i > 0)
			{
				ss << ' ';
			}

			ss << std::setprecision(numeric_limits<double>::digits10) << v;
		}

		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Dimensions/Z/Positions/List/Offsets")->SetValue(ss.str().c_str());
	}
}

/*static*/void libCZI::MetadataUtils::WriteGeneralDocumentInfo(ICziMetadataBuilder * builder, const libCZI::GeneralDocumentInfo & info)
{
	const auto setField = [=](bool isValid, const char* nodeName, const wstring & s)
	{
		if (isValid)
		{
			string n("Metadata/Information/Document/");
			n.append(nodeName);
			builder->GetRootNode()->GetOrCreateChildNode(n.c_str())->SetValue(s.c_str());
		}
	};

	setField(info.name_valid, "Name", info.name);
	setField(info.title_valid, "Title", info.title);
	setField(info.userName_valid, "UserName", info.userName);
	setField(info.description_valid, "Description", info.description);
	setField(info.comment_valid, "Comment", info.comment);
	setField(info.keywords_valid, "Keywords", info.keywords);
	setField(info.creationDateTime_valid, "CreationDate", info.creationDateTime);

	if (info.rating_valid)
	{
		builder->GetRootNode()->GetOrCreateChildNode("Metadata/Information/Document/Rating")->SetValueI32(info.rating);
	}
}

/*static*/void libCZI::MetadataUtils::WriteScalingInfo(libCZI::ICziMetadataBuilder * builder, const libCZI::ScalingInfo & scalingInfo)
{
	for (char d : { 'X', 'Y', 'Z' })
	{
		if (scalingInfo.IsScaleValid(d))
		{
			stringstream ss;
			ss << "Metadata/Scaling/Items/Distance[Id=" << d << "]/Value";
			builder->GetRootNode()->GetOrCreateChildNode(ss.str())->SetValueDbl(scalingInfo.GetScale(d));
		}
	}
}

/*static*/void libCZI::MetadataUtils::WriteScalingInfoEx(libCZI::ICziMetadataBuilder * builder, const libCZI::ScalingInfoEx & scalingInfo)
{
	for (char d : { 'X', 'Y', 'Z' })
	{
		if (scalingInfo.IsScaleValid(d) || !scalingInfo.GetDefaultUnitFormat(d).empty())
		{
			if (scalingInfo.IsScaleValid(d))
			{
				stringstream ss;
				ss << "Metadata/Scaling/Items/Distance[Id=" << d << "]/Value";
				builder->GetRootNode()->GetOrCreateChildNode(ss.str())->SetValueDbl(scalingInfo.GetScale(d));
			}

			if (!scalingInfo.GetDefaultUnitFormat(d).empty())
			{
				stringstream ss;
				ss << "Metadata/Scaling/Items/Distance[Id=" << d << "]/DefaultUnitFormat";
				builder->GetRootNode()->GetOrCreateChildNode(ss.str())->SetValue(scalingInfo.GetDefaultUnitFormat(d));
			}
		}
	}
}
