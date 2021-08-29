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

#include "pch.h"
#include "inc_libCZI.h"
#include "../libCZI/CziParse.h"

using namespace libCZI;

TEST(Utilities, CompareCoordinates1)
{
	CDimCoordinate a{ { libCZI::DimensionIndex::C,1 } };
	CDimCoordinate b{ { libCZI::DimensionIndex::C,2 } };

	int r = Utils::Compare(&a, &b);

	EXPECT_TRUE(r < 0) << "expecting 'a' to be less than 'b'";
}

TEST(Utilities, CompareCoordinates2)
{
	CDimCoordinate a{ { libCZI::DimensionIndex::C,1 },{ libCZI::DimensionIndex::T,1 } };
	CDimCoordinate b{ { libCZI::DimensionIndex::C,1 },{ libCZI::DimensionIndex::T,2 } };

	// coordinates are compared in the "numerical order of the dimension-enums", and C comes before T
	int r = Utils::Compare(&a, &b);

	EXPECT_TRUE(r < 0) << "expecting 'a' to be less than 'b'";
}

TEST(Utilities, CompareCoordinates3)
{
	CDimCoordinate a{ { libCZI::DimensionIndex::C,2 },{ libCZI::DimensionIndex::Z,1 } };
	CDimCoordinate b{ { libCZI::DimensionIndex::C,1 },{ libCZI::DimensionIndex::Z,2 } };

	// coordinates are compared in the "numerical order of the dimension-enums", and Z comes before C
	int r = Utils::Compare(&a, &b);

	EXPECT_TRUE(r < 0) << "expecting 'a' to be less than 'b'";
}

TEST(Utilities, CompareCoordinates4)
{
	CDimCoordinate a{ { libCZI::DimensionIndex::C,2 },{ libCZI::DimensionIndex::T,1 } };
	CDimCoordinate b{ { libCZI::DimensionIndex::C,1 },{ libCZI::DimensionIndex::T,2 } };

	// coordinates are compared in the "numerical order of the dimension-enums", and C comes before T
	int r = Utils::Compare(&a, &b);

	EXPECT_TRUE(r > 0) << "expecting 'b' to be less than 'a'";
}

TEST(Utilities, CompareCoordinates5)
{
	CDimCoordinate a{ { libCZI::DimensionIndex::T,1 } };
	CDimCoordinate b{ { libCZI::DimensionIndex::C,1 },{ libCZI::DimensionIndex::T,2 } };

	// coordinates are compared in the "numerical order of the dimension-enums", and C comes before T, so
	// we first try to compare DimensionIndex::C (which is invalid for a and valid for b), so a is less than b
	int r = Utils::Compare(&a, &b);

	EXPECT_TRUE(r < 0) << "expecting 'a' to be less than 'b'";
}

TEST(Utilities, CompareCoordinates6)
{
	CDimCoordinate a{ { libCZI::DimensionIndex::C,1 },{ libCZI::DimensionIndex::T,2 },{ libCZI::DimensionIndex::Z,5 } };
	CDimCoordinate b{ { libCZI::DimensionIndex::C,1 },{ libCZI::DimensionIndex::T,2 },{ libCZI::DimensionIndex::Z,5 } };

	// those are obviously equal
	int r = Utils::Compare(&a, &b);

	EXPECT_TRUE(r == 0) << "expecting 'a' to be equal to 'b'";
}

class BitmapLockTestMock : public IBitmapData
{
private:
	int lckCnt = 0;
public:
	virtual PixelType		GetPixelType() const
	{
		throw std::runtime_error("not implemented");
	}
	virtual IntSize			GetSize() const
	{
		throw std::runtime_error("not implemented");
	}
	virtual BitmapLockInfo	Lock()
	{
		this->lckCnt++;
		return BitmapLockInfo();
	}
	virtual void			Unlock()
	{
		this->lckCnt--;
	}
	int GetLockCnt() const
	{
		return this->lckCnt;
	}
};

TEST(Utilities, ScopedBitmapLocker1)
{
	auto bitmap = std::make_shared<BitmapLockTestMock>();

	{
		ScopedBitmapLockerSP locker1{ bitmap };
		EXPECT_EQ(bitmap->GetLockCnt(), 1) << "expecting a lock-count of '1'";

		{
			ScopedBitmapLockerSP locker2{ bitmap };
			EXPECT_EQ(bitmap->GetLockCnt(), 2) << "expecting a lock-count of '2'";
		}

		EXPECT_EQ(bitmap->GetLockCnt(), 1) << "expecting a lock-count of '1'";
	}

	EXPECT_EQ(bitmap->GetLockCnt(), 0) << "expecting a lock-count of zero";
}

TEST(Utilities, ScopedBitmapLocker2)
{
	auto bitmap = std::make_shared<BitmapLockTestMock>();

	// check that the scoped-bitmap-locker also works as expected when it is copied/moved
	{
		std::vector<ScopedBitmapLockerSP> vec;
		vec.push_back(ScopedBitmapLockerSP{ bitmap });
		vec.push_back(ScopedBitmapLockerSP{ bitmap });
		EXPECT_EQ(bitmap->GetLockCnt(), 2) << "expecting a lock-count of '2'";
	}

	EXPECT_EQ(bitmap->GetLockCnt(), 0) << "expecting a lock-count of zero";
}

TEST(Utilities, ScopedBitmapLocker3)
{
	auto bitmap = std::make_shared<BitmapLockTestMock>();

	// check that the scoped-bitmap-locker also works as expected when it is copied/moved
	{
		std::vector<ScopedBitmapLockerSP> vec;
		vec.emplace_back(ScopedBitmapLockerSP{ bitmap });
		vec.emplace_back(ScopedBitmapLockerSP{ bitmap });
		EXPECT_EQ(bitmap->GetLockCnt(), 2) << "expecting a lock-count of '2'";
	}

	EXPECT_EQ(bitmap->GetLockCnt(), 0) << "expecting a lock-count of zero";
}

// test-fixture, cf. https://stackoverflow.com/questions/47354280/what-is-the-best-way-of-testing-private-methods-with-googletest
class CCZIParseTest : public ::testing::Test
{
public:
	static libCZI::DimensionIndex DimensionCharToDimensionIndex(const char* ptr, size_t size)
	{
		return CCZIParse::DimensionCharToDimensionIndex(ptr, size);
	}
};

TEST(Utilities, DimensionCharToDimensionIndex1)
{
	auto dimIdx = CCZIParseTest::DimensionCharToDimensionIndex("C", 1);
	EXPECT_EQ(dimIdx, DimensionIndex::C) << "not the expected result";

	dimIdx = CCZIParseTest::DimensionCharToDimensionIndex("V", 1);
	EXPECT_EQ(dimIdx, DimensionIndex::V) << "not the expected result";

	dimIdx = CCZIParseTest::DimensionCharToDimensionIndex("t", 1);
	EXPECT_EQ(dimIdx, DimensionIndex::T) << "not the expected result";

	dimIdx = CCZIParseTest::DimensionCharToDimensionIndex("B", 1);
	EXPECT_EQ(dimIdx, DimensionIndex::B) << "not the expected result";

	dimIdx = CCZIParseTest::DimensionCharToDimensionIndex("Z", 1);
	EXPECT_EQ(dimIdx, DimensionIndex::Z) << "not the expected result";
}

TEST(Utilities, DimensionCharToDimensionIndex2)
{
	ASSERT_THROW(
		CCZIParseTest::DimensionCharToDimensionIndex("X", 1),
		LibCZICZIParseException) << "was expecting an exception here";
}