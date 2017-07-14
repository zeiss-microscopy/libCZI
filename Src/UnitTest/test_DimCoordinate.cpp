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
#include "CppUnitTest.h"

#include "inc_libCZI.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace libCZI;

namespace UnitTest
{
	TEST_CLASS(UnitTest_DimCoordinate)
	{
	public:

		TEST_METHOD(TestMethod_Parse1)
		{
			static const DimAndExpectedResult ExpectedResult1[] = { {DimensionIndex::C,42} };
			auto coord = libCZI::CDimCoordinate::Parse("C42");
			bool b = Check(coord, &ExpectedResult1[0], sizeof(ExpectedResult1) / sizeof(ExpectedResult1[0]));
			Assert::IsTrue(b, L"wrong value", LINE_INFO());

			static const DimAndExpectedResult ExpectedResult2[] = { { DimensionIndex::C,4 },{DimensionIndex::Z,5} };
			coord = libCZI::CDimCoordinate::Parse("C4Z5");
			b = Check(coord, &ExpectedResult2[0], sizeof(ExpectedResult2) / sizeof(ExpectedResult2[0]));
			Assert::IsTrue(b, L"wrong value", LINE_INFO());

			coord = libCZI::CDimCoordinate::Parse("C4;Z5;");
			b = Check(coord, &ExpectedResult2[0], sizeof(ExpectedResult2) / sizeof(ExpectedResult2[0]));
			Assert::IsTrue(b, L"wrong value", LINE_INFO());

			coord = libCZI::CDimCoordinate::Parse("     C4        Z5         ");
			b = Check(coord, &ExpectedResult2[0], sizeof(ExpectedResult2) / sizeof(ExpectedResult2[0]));
			Assert::IsTrue(b, L"wrong value", LINE_INFO());

			static const DimAndExpectedResult ExpectedResult3[] = { { DimensionIndex::T,-2365 } };
			coord = libCZI::CDimCoordinate::Parse("T-2365");
			b = Check(coord, &ExpectedResult3[0], sizeof(ExpectedResult3) / sizeof(ExpectedResult3[0]));
			Assert::IsTrue(b, L"wrong value", LINE_INFO());

			// the largest possible number
			static const DimAndExpectedResult ExpectedResult4[] = { { DimensionIndex::T,(std::numeric_limits<int>::max)() } };
			coord = libCZI::CDimCoordinate::Parse("T2147483647");
			b = Check(coord, &ExpectedResult4[0], sizeof(ExpectedResult4) / sizeof(ExpectedResult4[0]));
			Assert::IsTrue(b, L"wrong value", LINE_INFO());

			// the smallest possible number
			static const DimAndExpectedResult ExpectedResult5[] = { { DimensionIndex::T,(std::numeric_limits<int>::min)() } };
			coord = libCZI::CDimCoordinate::Parse("T-2147483648");
			b = Check(coord, &ExpectedResult5[0], sizeof(ExpectedResult5) / sizeof(ExpectedResult5[0]));
			Assert::IsTrue(b, L"wrong value", LINE_INFO());

			static const DimAndExpectedResult ExpectedResult6[] = { { DimensionIndex::T,4 },{ DimensionIndex::C,-7 } };
			coord = libCZI::CDimCoordinate::Parse("T000000000000000000000004;C-0000000000000000000000000007");
			b = Check(coord, &ExpectedResult6[0], sizeof(ExpectedResult6) / sizeof(ExpectedResult6[0]));
			Assert::IsTrue(b, L"wrong value", LINE_INFO());

		}

		TEST_METHOD(TestMethod_ErrorParse)
		{
			// "C4Z5C1" -> invalid, because 'C' appears twice
			bool exceptionCaught = false;
			try
			{
				auto coord = CDimCoordinate::Parse("C4Z5C1");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::DuplicateDimension == excp.GetErrorType(), L"Not the corrected result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());

			// "C4Q5C1" -> invalid, because 'Q' is not a known dimension
			exceptionCaught = false;
			try
			{
				auto coord = CDimCoordinate::Parse("C4Q5C1");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::InvalidSyntax == excp.GetErrorType(), L"Not the corrected result", LINE_INFO());
				Assert::IsTrue(2 == excp.GetNumberOfCharsParsedOk(), L"Not the corrected result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());

			// "C4165561651658797" -> invalid, because number is too large
			exceptionCaught = false;
			try
			{
				auto coord = CDimCoordinate::Parse("C4165561651658797");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::InvalidSyntax == excp.GetErrorType(), L"Not the corrected result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());

			// "T2147483648" -> invalid, because number is too large (not representable by an int)
			exceptionCaught = false;
			try
			{
				auto coord = CDimCoordinate::Parse("T2147483648");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::InvalidSyntax == excp.GetErrorType(), L"Not the corrected result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());

			// "T-2147483649" -> invalid, because number is too small (not representable by an int)
			exceptionCaught = false;
			try
			{
				auto coord = CDimCoordinate::Parse("T-2147483649");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::InvalidSyntax == excp.GetErrorType(), L"Not the corrected result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());

			// "T--49" -> invalid, because we have two minus signs
			exceptionCaught = false;
			try
			{
				auto coord = CDimCoordinate::Parse("T--49");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::InvalidSyntax == excp.GetErrorType(), L"Not the corrected result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());

			// "T-+49" -> invalid, because we have two signs
			exceptionCaught = false;
			try
			{
				auto coord = CDimCoordinate::Parse("T-+49");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::InvalidSyntax == excp.GetErrorType(), L"Not the corrected result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());
		}

		TEST_METHOD(TestMethod_Initialize)
		{
			auto coord = CDimCoordinate{ { DimensionIndex::Z,8 } , {DimensionIndex::T,0}, {DimensionIndex::C,1} };
			Assert::IsTrue(coord.IsValid(DimensionIndex::Z), L"incorrect result", LINE_INFO());
			Assert::IsTrue(coord.IsValid(DimensionIndex::T), L"incorrect result", LINE_INFO());
			Assert::IsTrue(coord.IsValid(DimensionIndex::C), L"incorrect result", LINE_INFO());
			int c;
			Assert::IsTrue(coord.TryGetPosition(DimensionIndex::Z, &c), L"incorrect result", LINE_INFO());
			Assert::IsTrue(c == 8, L"incorrect result", LINE_INFO());
			Assert::IsTrue(coord.TryGetPosition(DimensionIndex::T, &c), L"incorrect result", LINE_INFO());
			Assert::IsTrue(c == 0, L"incorrect result", LINE_INFO());
			Assert::IsTrue(coord.TryGetPosition(DimensionIndex::C, &c), L"incorrect result", LINE_INFO());
			Assert::IsTrue(c == 1, L"incorrect result", LINE_INFO());

			Assert::IsFalse(coord.IsValid(DimensionIndex::I), L"incorrect result", LINE_INFO());
			Assert::IsFalse(coord.IsValid(DimensionIndex::V), L"incorrect result", LINE_INFO());
			Assert::IsFalse(coord.IsValid(DimensionIndex::H), L"incorrect result", LINE_INFO());
		}

	private:
		struct DimAndExpectedResult
		{
			DimensionIndex	dim;
			int				result;
		};

		static bool Check(const libCZI::CDimCoordinate& dimCoord, const DimAndExpectedResult* ptrResult, int count)
		{
			for (int i = 0; i < count; ++i)
			{
				int value;
				bool b = dimCoord.TryGetPosition(ptrResult->dim, &value);
				if (b == false)
				{
					return false;
				}

				if (value != ptrResult->result)
				{
					return false;
				}

				++ptrResult;
			}

			return true;
		}

	};
}