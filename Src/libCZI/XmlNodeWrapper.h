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

#include "libCZI_Metadata.h"
#include <regex>
#include <exception>
#include "utilities.h"
#include "pugixml.hpp"
#include "libCZI_exceptions.h"

/// This implements a wrapper around a pugi-node struct (implementing the IXmlNodeRead-interface). It is initialized
/// with a raw pointer (to the pugi-xml-node-structure), and we rely on that pointer being valid for the lifetime
/// of this object. In order to achieve this, we take another shared_ptr and keep a reference to it, and this
/// object is assumed to be the (sole) owner of the pugi-xml-document.
/// We provide two constructors, where one takes the mentioned shared_ptr and the other doesn't. If using the one without
/// the shared_ptr, then it must be ensured that the lifetime of this object is the same as the underlying pugi-xml-document
/// (e.g. if this class is a member of the class owning the pugi-xml-document, in which case we would create a circular
/// reference). In that case, you need to use only the "GetChildNodeReadonly"-overload which takes an additional shared_ptr
/// (which is to be owner of the pugi-xml-document).
/// \tparam t Generic type parameter (of the object owning the pugi-xml-document).
template<typename t, typename tExcp>
class XmlNodeWrapperReadonly : public libCZI::IXmlNodeRead
{
private:
	pugi::xml_node node;
	std::shared_ptr<t> parentRef;
public:
	XmlNodeWrapperReadonly(std::shared_ptr<t> parentRef, pugi::xml_node_struct* node_struct) :node(node_struct), parentRef(std::move(parentRef)) {};
	XmlNodeWrapperReadonly(pugi::xml_node_struct* node_struct) :node(node_struct) {};
public:	// interface IXmlNodeRead
	std::wstring Name() const override
	{
		return this->node.name();
	}

	bool TryGetAttribute(const wchar_t* attributeName, std::wstring* attribValue) const override
	{
		auto attrib = this->node.find_attribute([&](const pugi::xml_attribute & a)
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

	void EnumAttributes(const std::function<bool(const std::wstring& attribName, const std::wstring& attribValue)>& enumFunc) const override
	{
		for (pugi::xml_attribute_iterator ait = this->node.attributes_begin(); ait != this->node.attributes_end(); ++ait)
		{
			if (!enumFunc(std::wstring(ait->name()), std::wstring(ait->value())))
			{
				break;
			}
		}
	}

	bool TryGetValue(std::wstring* value) const override
	{
		if (this->node.first_child().type() == pugi::xml_node_type::node_pcdata)
		{
			if (value != nullptr)
			{
				*value = this->node.child_value();
			}

			return true;
		}

		return false;
	}

	std::shared_ptr<IXmlNodeRead> GetChildNodeReadonly(const char* path) override
	{
		if (!this->parentRef)
		{
			// we must never get here
			throw std::logic_error("there is no valid object which is the owner of the xml-dom");
		}

		return this->GetChildNodeReadonly(path, this->parentRef);
	}

	void EnumChildren(std::function<bool(std::shared_ptr<IXmlNodeRead>)> enumChildren) override
	{
		if (!this->parentRef)
		{
			// we must never get here
			throw std::logic_error("there is no valid object which is the owner of the xml-dom");
		}

		return this->EnumChildren(enumChildren, this->parentRef);
	}

	std::shared_ptr<IXmlNodeRead> GetChildNodeReadonly(const char* path, std::shared_ptr<t> parentRef)
	{
		auto p = Utilities::convertUtf8ToWchar_t(path);
		std::vector<std::wstring> tokens;
		Utilities::Tokenize(p, tokens, L"/");
		if (tokens.empty())
		{
			tExcp::ThrowInvalidPath();
		}

		if (any_of(tokens.cbegin(), tokens.cend(), [](const std::wstring & str) {return str.empty(); }))
		{
			tExcp::ThrowInvalidPath();
		}

		auto node = GetChildElementNodeWithAttributes(this->node, tokens[0]);
		if (!node)
		{
			return nullptr;
		}

		for (size_t i = 1; i < tokens.size(); ++i)
		{
			node = GetChildElementNodeWithAttributes(node, tokens[i].c_str());
			if (!node)
			{
				return nullptr;
			}
		}

		return std::make_shared<XmlNodeWrapperReadonly>(parentRef, node.internal_object());
	}

	void EnumChildren(std::function<bool(std::shared_ptr<IXmlNodeRead>)> enumChildren, std::shared_ptr<t> parentRef)
	{
		for (pugi::xml_node childNode = this->node.first_child(); childNode; childNode = childNode.next_sibling())
		{
			if (childNode.type() == pugi::xml_node_type::node_element)
			{
				bool b = enumChildren(std::make_shared<XmlNodeWrapperReadonly>(parentRef, childNode.internal_object()));
				if (!b)
				{
					break;
				}
			}
		}
	}
private:
	static pugi::xml_node GetChildElementNodeWithAttributes(pugi::xml_node& node, const std::wstring& str)
	{
		std::wregex nodenameWihtAttribregex(LR"(([^\[\]]+)(\[([^\[\]]*)\])?)");
		std::wsmatch pieces_match;

		if (std::regex_match(str, pieces_match, nodenameWihtAttribregex))
		{
			if (pieces_match.size() == 4)
			{
				if (pieces_match[0].matched == true && pieces_match[1].matched == true)
				{
					std::wstring nodeName = pieces_match[1];
					if (pieces_match[2].matched == false && pieces_match[3].matched == false)
					{
						// we only got a name ( not followed by [Id=abc] )
						return GetChildElementNode(node, nodeName.c_str());
					}
					else if (pieces_match[2].matched == true && pieces_match[3].matched == true)
					{
						const auto attribValuePairs = ParseAttributes(pieces_match[3]);
						return GetChildElementNodeWithAttributes(node, nodeName, attribValuePairs);
					}
				}
			}
		}

		tExcp::ThrowInvalidPath();
		throw std::logic_error("Must not get here.");
	}

	static pugi::xml_node GetChildElementNodeWithAttributes(pugi::xml_node& node, const std::wstring& str, const std::map<std::wstring, std::wstring>& attribs)
	{
		struct find_element_node_and_attributes
		{
			const wchar_t* nodeName;
			const std::map<std::wstring, std::wstring>& attribs;

			bool operator()(pugi::xml_node node) const
			{
				if (node.type() != pugi::xml_node_type::node_element)
				{
					return false;
				}

				if (wcscmp(node.name(), this->nodeName) != 0)
				{
					return false;
				}

				for (auto it = this->attribs.cbegin(); it != this->attribs.cend(); ++it)
				{
					auto attribute = node.find_attribute([&](const pugi::xml_attribute & a)
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
			return pugi::xml_node();
		}

		return c;
	}

	static pugi::xml_node GetChildElementNode(pugi::xml_node& node, const wchar_t* sz)
	{
		struct find_element_node
		{
			const wchar_t* nodeName;

			bool operator()(pugi::xml_node node) const
			{
				if (node.type() != pugi::xml_node_type::node_element)
				{
					return false;
				}

				return wcscmp(node.name(), this->nodeName) == 0;
			}
		};

		auto c = node.find_child(find_element_node{ sz });
		if (!c)
		{
			return pugi::xml_node();
		}

		return c;
	}

	static std::map<std::wstring, std::wstring> ParseAttributes(const std::wstring& str)
	{
		std::map<std::wstring, std::wstring> attribMap;

		std::wsmatch pieces_match;

		// now we have string of the form Id=abc,Name=abc
		std::vector<std::wstring> pairs;
		Utilities::Tokenize(str, pairs, L",;");

		std::wregex attribValuePairRegex(LR"(([^=]+)=([^,;]*))");
		for (auto it = pairs.cbegin(); it != pairs.cend(); ++it)
		{
			bool parsedOk = false;
			if (std::regex_match(*it, pieces_match, attribValuePairRegex))
			{
				if (pieces_match.size() == 3 && pieces_match[0].matched == true && pieces_match[1].matched == true && pieces_match[2].matched == true)
				{
					attribMap.insert(std::pair<std::wstring, std::wstring>(pieces_match[1], pieces_match[2]));
					parsedOk = true;
				}
			}

			if (!parsedOk)
			{
				tExcp::ThrowInvalidPath();
			}
		}

		return attribMap;
	}
};
