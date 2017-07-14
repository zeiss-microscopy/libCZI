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
	TEST_CLASS(UnitTest_IndexSet)
	{
	public:
		TEST_METHOD(TestMethod_IndexSetParse1)
		{
			auto idxSet = libCZI::Utils::IndexSetFromString(L"4-9");
			bool b = CheckIfTrue(4, 9, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfFalse(0, 3, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfFalse(10, 30, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
		}

		TEST_METHOD(TestMethod_IndexSetParse2)
		{
			auto idxSet = libCZI::Utils::IndexSetFromString(L"4-9,11-13");
			bool b = CheckIfTrue(4, 9, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfFalse(0, 3, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfFalse(10, 10, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfTrue(11, 13, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfFalse(14, 40, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
		}

		TEST_METHOD(TestMethod_IndexSetParse3)
		{
			auto idxSet = libCZI::Utils::IndexSetFromString(L"42");
			bool b = CheckIfFalse(0, 41, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfTrue(42, 42, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfFalse(43, 99, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
		}

		TEST_METHOD(TestMethod_IndexSetParse4)
		{
			auto idxSet = libCZI::Utils::IndexSetFromString(L"-inf-5");
			bool b = CheckIfTrue(-25, 5, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfFalse(6, 42, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
		}

		TEST_METHOD(TestMethod_IndexSetParse5)
		{
			auto idxSet = libCZI::Utils::IndexSetFromString(L"5-inf");
			bool b = CheckIfFalse(-25, 4, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfTrue(5, 42, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
		}

		TEST_METHOD(TestMethod_IndexSetParse6)
		{
			bool exceptionCaught = false;
			try
			{
				auto idxSet = libCZI::Utils::IndexSetFromString(L"5+-6");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::InvalidSyntax == excp.GetErrorType(), L"Not the correct result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());
		}

		TEST_METHOD(TestMethod_IndexSetParse7)
		{
			bool exceptionCaught = false;
			try
			{
				auto idxSet = libCZI::Utils::IndexSetFromString(L"6-3");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::FromGreaterThanTo == excp.GetErrorType(), L"Not the correct result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());
		}

		TEST_METHOD(TestMethod_IndexSetParse8)
		{
			bool exceptionCaught = false;
			try
			{
				auto idxSet = libCZI::Utils::IndexSetFromString(L"534823948902342346");
			}
			catch (LibCZIStringParseException& excp)
			{
				exceptionCaught = true;
				Assert::IsTrue(LibCZIStringParseException::ErrorType::InvalidSyntax == excp.GetErrorType(), L"Not the correct result", LINE_INFO());
			}

			Assert::IsTrue(exceptionCaught, L"didn't throw expected exception", LINE_INFO());
		}

		TEST_METHOD(TestMethod_IndexSetParse9)
		{
			auto idxSet = libCZI::Utils::IndexSetFromString(L"2,5,7,9,123");
			bool b = CheckIfFalse(0, 1, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfTrue(2, 2, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfTrue(5, 5, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfTrue(7, 7, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfTrue(9, 9, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfTrue(123, 123, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
			b = CheckIfFalse(124, 199, idxSet.get());
			Assert::IsTrue(b, L"wrong value", LINE_INFO());
		}
	private:
		static bool CheckIfTrue(int startIdx, int endIdx, libCZI::IIndexSet* idxSet)
		{
			return CheckIfTrueOrFalse(true, startIdx, endIdx, idxSet);
		}

		static bool CheckIfFalse(int startIdx, int endIdx, libCZI::IIndexSet* idxSet)
		{
			return CheckIfTrueOrFalse(false, startIdx, endIdx, idxSet);
		}

		static bool CheckIfTrueOrFalse(bool value, int startIdx, int endIdx, libCZI::IIndexSet* idxSet)
		{
			for (int i = startIdx; i <= endIdx; ++i)
			{
				if (value != idxSet->IsContained(i))
				{
					return false;
				}
			}

			return true;
		}
	};
}