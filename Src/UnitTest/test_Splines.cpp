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
		TEST_METHOD(TestMethod_Splines1)
		{
			static const double Points[] = { 0,0,0.362559241706161,0.876190476190476 ,0.554502369668246,0.561904761904762 ,1,1 };

			auto result = CSplines::GetSplineCoefficients(
				sizeof(Points) / sizeof(Points[0]) / 2,
				[](int idx, double* pX, double* pY)->void
			{
				idx *= 2;
				if (pX != nullptr) { *pX = Points[idx]; }
				if (pY != nullptr) { *pY = Points[idx + 1]; }
			});

			Assert::AreEqual(result.size(), (size_t)3, L"Incorrect result", LINE_INFO());

			Assert::IsTrue(IsClose(result[0].a, -11.360115103033465), L"Incorrect result", LINE_INFO());
			Assert::IsTrue(IsClose(result[0].b, 0), L"Incorrect result", LINE_INFO());
			Assert::IsTrue(IsClose(result[0].c, 3.9099603132098), L"Incorrect result", LINE_INFO());
			Assert::IsTrue(IsClose(result[0].d, 0), L"Incorrect result", LINE_INFO());

			Assert::IsTrue(IsClose(result[1].a, 35.39860240958761), L"Incorrect result", LINE_INFO());
			Assert::IsTrue(IsClose(result[1].b, -12.356144152351561), L"Incorrect result", LINE_INFO());
			Assert::IsTrue(IsClose(result[1].c, -0.5698739410787983), L"Incorrect result", LINE_INFO());
			Assert::IsTrue(IsClose(result[1].d, 0.876190476190476), L"Incorrect result", LINE_INFO());

			Assert::IsTrue(IsClose(result[2].a, -6.0063254490025031), L"Incorrect result", LINE_INFO());
			Assert::IsTrue(IsClose(result[2].b, 8.0274112635957717), L"Incorrect result", LINE_INFO());
			Assert::IsTrue(IsClose(result[2].c, -1.4007444718589364), L"Incorrect result", LINE_INFO());
			Assert::IsTrue(IsClose(result[2].d, 0.561904761904762), L"Incorrect result", LINE_INFO());
		}

	private:
		static bool IsClose(double a, double b)
		{
			if (abs(a - b) < 1e-6)
				return true;
			return false;
		}
	};
}