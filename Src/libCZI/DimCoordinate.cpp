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
#include <climits>
#include <regex>

using namespace libCZI;
using namespace std;

class CIntParseCoordinateBoundsString
{
public:
	static CDimCoordinate ParseCoordinate(const char* str, int& parsedUntil)
	{
		parsedUntil = 0;
		CDimCoordinate dim;
		int charsParsedOk;
		for (; *str != '\0';)
		{
			DimensionIndex dimIdx; int value;
			bool b = Int_ParseCoordinate(str, charsParsedOk, dimIdx, value);
			if (b == false)
			{
				parsedUntil += charsParsedOk;
				throw LibCZIStringParseException("Syntax error", parsedUntil, LibCZIStringParseException::ErrorType::InvalidSyntax);
			}

			if (dim.TryGetPosition(dimIdx, nullptr) == true)
			{
				throw LibCZIStringParseException("Duplicate dimension", parsedUntil, LibCZIStringParseException::ErrorType::DuplicateDimension);
			}

			dim.Set(dimIdx, value);
			str += charsParsedOk;
			parsedUntil += charsParsedOk;
		}

		return dim;
	}

	static CDimBounds ParseBounds(const char* str)
	{
		string s(str);
		regex bounds_regex(R"((?:([a-zA-Z])(?:([\+|-]?[0-9]+):([\+|-]?[0-9]+))\s*))");

		std::regex_iterator<std::string::iterator> it(s.begin(), s.end(), bounds_regex);
		std::regex_iterator<std::string::iterator> end;

		CDimBounds bounds;

		bool lastMatchHasNoSuffix = false;
		for (; it != std::regex_iterator<std::string::iterator>(); ++it)
		{
			if (it->size() == 4)
			{
				auto dimMatch = it->operator[](1);
				auto dimIdx = Utils::CharToDimension(dimMatch.str()[0]);
				if (dimIdx == DimensionIndex::invalid)
				{
					throw LibCZIStringParseException("Invalid dimension", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
				}

				int startIdx, sizeIdx;
				auto startIdxMatch = it->operator[](2);
				if (!TryParseInt(startIdxMatch.str().c_str(), &startIdx))
				{
					throw LibCZIStringParseException("Invalid start-index", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
				}

				auto sizeIdxMatch = it->operator[](3);
				if (!TryParseInt(sizeIdxMatch.str().c_str(), &sizeIdx) || sizeIdx == 0)
				{
					throw LibCZIStringParseException("Invalid end-index", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
				}

				if (bounds.IsValid(dimIdx))
				{
					throw LibCZIStringParseException("Duplicate dimension", -1, LibCZIStringParseException::ErrorType::DuplicateDimension);
				}

				bounds.Set(dimIdx, startIdx, sizeIdx);

				if (it->suffix().str().empty())
				{
					lastMatchHasNoSuffix = true;
				}
			}
		}

		if (lastMatchHasNoSuffix == false)
		{
			throw LibCZIStringParseException("Bounds-string did not parse", -1, LibCZIStringParseException::ErrorType::InvalidSyntax);
		}

		return bounds;
	}
private:
	static bool Int_ParseCoordinate(const char* str, int& charsParsedOk, DimensionIndex& dimIdx, int& value)
	{
		// a 32-bit integer can have 10 digits at most, plus one for the +/- sign , that's 11 altogehter
		// plus the trailing zero, which gives us 12
		const int MAX_CHARS_FOR_NUMBER = 11;
		char number[MAX_CHARS_FOR_NUMBER + 1];
		int numberPos = 0;
		charsParsedOk = 0;
		char c;

		// skip any number of white-spaces (which includes ',' and ';' in our case)
		for (; ; )
		{
			c = *str;
			switch (c)
			{
			case '\t':case ' ':case',':case';':
				++str; ++charsParsedOk;
				continue;
			}

			break;
		}

		// now, the next char must be one of the dimensions (we allow for upper and lower case)
		dimIdx = Utils::CharToDimension(*str);
		if (dimIdx == DimensionIndex::invalid)
		{
			return false;
		}

		++str; ++charsParsedOk;

		// now skip white-spaces (until we either find a '+', a '-' or a number)
		for (; ; )
		{
			c = *str;
			switch (c)
			{
			case '\t':case ' ':
				++str; ++charsParsedOk;
				continue;
			}

			break;
		}

		c = *str;
		if (c == '-' || c == '+')
		{
			if (c == '-') { number[numberPos++] = c; }
			++str; ++charsParsedOk;

			// again, skip white-space
			for (; ; )
			{
				c = *str;
				switch (c)
				{
				case '\t':case ' ':
					++str; ++charsParsedOk;
					continue;
				}

				break;
			}
		}

		// now, *str must be a digit or error
		int noOfZeroLeadingDigits = 0;
		bool nonZeroLeadingDigitFound = false;
		bool hasDigit = false;
		for (;;)
		{
			c = *str;
			if (isdigit(c))
			{
				if (c == '0'&&nonZeroLeadingDigitFound == false)
				{
					if (noOfZeroLeadingDigits == 0)
					{
						// store at most one leading zero
						number[numberPos++] = c;
						++noOfZeroLeadingDigits;
						hasDigit = true;
					}

					++str; ++charsParsedOk;
				}
				else
				{
					if (numberPos >= MAX_CHARS_FOR_NUMBER)
					{
						return false;
					}

					number[numberPos++] = c;
					hasDigit = true;
					nonZeroLeadingDigitFound = true;
					++str; ++charsParsedOk;
				}
			}
			else
			{
				for (; ; )
				{
					c = *str;
					switch (c)
					{
					case '\t':case ' ':case',':case';':
						++str; ++charsParsedOk;
						continue;
					}

					break;
				}

				break;
			}
		}

		number[numberPos] = '\0';

		// now we must have at least one digit in the number-string, otherwise -> error
		if (hasDigit == false)
		{
			return false;
		}

		if (!TryParseInt(number, &value))
		{
			return false;
		}

		return true;
	}

	static bool TryParseInt(const char* number, int* value)
	{
		long long liValue = strtoll(number, nullptr, 10);

		if (liValue > (std::numeric_limits<int>::max)() || liValue < (std::numeric_limits<int>::min)())
		{
			return false;
		}

		if (value != nullptr)
		{
			*value = static_cast<int>(liValue);
		}

		return true;
	}
};

/*static*/CDimCoordinate libCZI::CDimCoordinate::Parse(const char* str)
{
	int parsedChars;
	return CIntParseCoordinateBoundsString::ParseCoordinate(str, parsedChars);
}

/*static*/CDimBounds CDimBounds::Parse(const char* str)
{
	return CIntParseCoordinateBoundsString::ParseBounds(str);
}