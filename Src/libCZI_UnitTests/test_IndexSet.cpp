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

#include "pch.h"
#include "inc_libCZI.h"

using namespace libCZI;

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

static bool CheckIfTrue(int startIdx, int endIdx, libCZI::IIndexSet* idxSet)
{
	return CheckIfTrueOrFalse(true, startIdx, endIdx, idxSet);
}

static bool CheckIfFalse(int startIdx, int endIdx, libCZI::IIndexSet* idxSet)
{
	return CheckIfTrueOrFalse(false, startIdx, endIdx, idxSet);
}

TEST(IndexSet, IndexSetParse1)
{
	auto idxSet = libCZI::Utils::IndexSetFromString(L"4-9");
	bool b = CheckIfTrue(4, 9, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfFalse(0, 3, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfFalse(10, 30, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
}

TEST(IndexSet, IndexSetParse2)
{
	auto idxSet = libCZI::Utils::IndexSetFromString(L"4-9,11-13");
	bool b = CheckIfTrue(4, 9, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfFalse(0, 3, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfFalse(10, 10, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfTrue(11, 13, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfFalse(14, 40, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
}

TEST(IndexSet, IndexSetParse3)
{
	auto idxSet = libCZI::Utils::IndexSetFromString(L"42");
	bool b = CheckIfFalse(0, 41, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfTrue(42, 42, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfFalse(43, 99, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
}

TEST(IndexSet, IndexSetParse4)
{
	auto idxSet = libCZI::Utils::IndexSetFromString(L"-inf-5");
	bool b = CheckIfTrue(-25, 5, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfFalse(6, 42, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
}

TEST(IndexSet, IndexSetParse5)
{
	auto idxSet = libCZI::Utils::IndexSetFromString(L"5-inf");
	bool b = CheckIfFalse(-25, 4, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfTrue(5, 42, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
}

TEST(IndexSet, IndexSetParse6)
{
	bool exceptionCaught = false;
	try
	{
		auto idxSet = libCZI::Utils::IndexSetFromString(L"5+-6");
	}
	catch (LibCZIStringParseException& excp)
	{
		exceptionCaught = true;
		EXPECT_EQ(LibCZIStringParseException::ErrorType::InvalidSyntax, excp.GetErrorType()) << "Not the correct result";
	}

	EXPECT_TRUE(exceptionCaught) << "didn't throw expected exception";
}

TEST(IndexSet, IndexSetParse7)
{
	bool exceptionCaught = false;
	try
	{
		auto idxSet = libCZI::Utils::IndexSetFromString(L"6-3");
	}
	catch (LibCZIStringParseException& excp)
	{
		exceptionCaught = true;
		EXPECT_EQ(LibCZIStringParseException::ErrorType::FromGreaterThanTo, excp.GetErrorType()) << "Not the correct result";
	}

	EXPECT_TRUE(exceptionCaught) << "didn't throw expected exception";
}

TEST(IndexSet, IndexSetParse8)
{
	bool exceptionCaught = false;
	try
	{
		auto idxSet = libCZI::Utils::IndexSetFromString(L"534823948902342346");
	}
	catch (LibCZIStringParseException& excp)
	{
		exceptionCaught = true;
		EXPECT_EQ(LibCZIStringParseException::ErrorType::InvalidSyntax, excp.GetErrorType()) << "Not the correct result";
	}

	EXPECT_TRUE(exceptionCaught) << "didn't throw expected exception";
}

TEST(IndexSet, IndexSetParse9)
{
	auto idxSet = libCZI::Utils::IndexSetFromString(L"2,5,7,9,123");
	bool b = CheckIfFalse(0, 1, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfTrue(2, 2, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfTrue(5, 5, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfTrue(7, 7, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfTrue(9, 9, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfTrue(123, 123, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
	b = CheckIfFalse(124, 199, idxSet.get());
	EXPECT_TRUE(b) << "wrong value";
}
	