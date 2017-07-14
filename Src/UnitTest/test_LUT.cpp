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
	TEST_CLASS(UnitTest_LUT)
	{
	public:
		TEST_METHOD(TestMethod_LUTGeneraton_Gamma1)
		{
			auto gammaLutTest = libCZI::Utils::Create8BitLookUpTableFromGamma(256, 0, 1, 0.58f);

			Assert::AreEqual(gammaLutTest.size(), (size_t)256, L"Incorrect result", LINE_INFO());

			static std::uint8_t ExpectedResult[256] = {
				0	,5	,8	,12	,15	,18	,21	,23	,26	,28	,31	,33	,35	,37	,39	,41	,43	,45	,46	,48	,50	,52	,53	,55	,57	,58	,60	,61	,63	,64	,66	,67	,69	,70	,72	,73	,74	,76	,77	,78	,80	,81	,82	,
				83	,85	,86	,87	,88	,90	,91	,92	,93	,94	,96	,97	,98	,99	,100	,101	,103	,104	,105	,106	,107	,108	,109	,110	,111	,112	,113	,114	,115	,116	,117	,118	,120	,
				121	,122	,123	,124	,124	,125	,126	,127	,128	,129	,130	,131	,132	,133	,134	,135	,136	,137	,138	,139	,140	,141	,142	,142	,143	,144	,
				145	,146	,147	,148	,149	,150	,150	,151	,152	,153	,154	,155	,156	,156	,157	,158	,159	,160	,161	,161	,162	,163	,164	,165	,
				166	,166	,167	,168	,169	,170	,170	,171	,172	,173	,174	,174	,175	,176	,177	,178	,178	,179	,180	,181	,181	,182	,183	,184	,184	,
				185	,186	,187	,187	,188	,189	,190	,190	,191	,192	,193	,193	,194	,195	,196	,196	,197	,198	,199	,199	,200	,201	,201	,202	,203	,
				204	,204	,205	,206	,206	,207	,208	,209	,209	,210	,211	,211	,212	,213	,213	,214	,215	,215	,216	,217	,217	,218	,219	,220	,220	,
				221	,222	,222	,223	,224	,224	,225	,226	,226	,227	,228	,228	,229	,229	,230	,231	,231	,232	,233	,233	,234	,235	,235	,236	,237	,
				237	,238	,239	,239	,240	,240	,241	,242	,242	,243	,244	,244	,245	,246	,246	,247	,247	,248	,249	,249	,250	,250	,251	,252	,252	,
				253	,254	,254	,255	,255 };

			// allow for a difference of 1 (rounding is not yet the same as in the original code, need to look into this...)
			bool isCorrect = CompareWithMargin(&gammaLutTest[0], ExpectedResult, 256, 1);
			Assert::IsTrue(isCorrect, L"Incorrect result", LINE_INFO());
		}

		TEST_METHOD(TestMethod_LUTGeneraton_Gamma2)
		{
			auto gammaLutTest = libCZI::Utils::Create8BitLookUpTableFromGamma(256, 0, 1, 1.58f);

			Assert::AreEqual(gammaLutTest.size(), (size_t)256, L"Incorrect result", LINE_INFO());
			static std::uint8_t ExpectedResult[256] = {
				0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 8, 8, 9,
				9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 17, 18, 18, 19, 19, 20, 21, 21, 22, 23, 23, 24, 25, 25, 26, 27,
				27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 34, 35, 36, 37, 38, 38, 39, 40, 41, 42, 42, 43, 44, 45, 46, 47, 47, 48, 49, 50, 51,
				52, 53, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
				81, 82, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 98, 99, 100, 101, 102, 103, 104, 105, 107,
				108, 109, 110, 111, 112, 114, 115, 116, 117, 118, 119, 121, 122, 123, 124, 125, 127,
				128, 129, 130, 132, 133, 134, 135, 137, 138, 139, 140, 142, 143, 144, 145, 147, 148,
				149, 151, 152, 153, 155, 156, 157, 159, 160, 161, 162, 164, 165, 167, 168, 169, 171,
				172, 173, 175, 176, 177, 179, 180, 182, 183, 184, 186, 187, 189, 190, 191, 193, 194,
				196, 197, 199, 200, 201, 203, 204, 206, 207, 209, 210, 212, 213, 215, 216, 218, 219,
				221, 222, 224, 225, 227, 228, 230, 231, 233, 234, 236, 237, 239, 240, 242, 243, 245,
				247, 248, 250, 251, 253, 254 };

			// allow for a difference of 1 (rounding is not yet the same as in the original code, need to look into this...)
			bool isCorrect = CompareWithMargin(&gammaLutTest[0], ExpectedResult, 256, 1);
			Assert::IsTrue(isCorrect, L"Incorrect result", LINE_INFO());
		}

		TEST_METHOD(TestMethod_LUTGeneraton_Splines1)
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

			std::vector<IDisplaySettings::SplineData> splineData;
			int idx = 0;
			for (const auto& i : result)
			{
				splineData.push_back({ Points[idx * 2],i });
				++idx;
			}

			auto splineLutTest = libCZI::Utils::Create8BitLookUpTableFromSplines(256, 0, 1, splineData);
			Assert::AreEqual(splineLutTest.size(), (size_t)256, L"Incorrect result", LINE_INFO());

			static const std::uint8_t ExpectedResult[256] =
			{ 0	,4	,8	,12	,16	,20	,23	,27	,31	,35	,39	,43	,47	,50	,54	,58	,62	,66	,69	,73	,77	,80	,84	,88	,91	,95	,99	,102	,106	,109	,113	,116	,119	,123,
				126	,129	,133	,136	,139	,142	,145	,148	,151	,154	,157	,160	,163	,166	,168	,171	,174	,176	,179	,181	,184	,
				186	,188	,191	,193	,195	,197	,199	,201	,203	,204	,206	,208	,209	,211	,212	,214	,215	,216	,217	,219	,220	,
				220	,221	,222	,223	,223	,224	,224	,225	,225	,225	,225	,225	,225	,225	,225	,224	,224	,223	,222	,222	,221	,
				220	,219	,218	,217	,215	,214	,213	,211	,210	,208	,207	,205	,203	,201	,200	,198	,196	,194	,192	,190	,188	,
				186	,184	,182	,180	,178	,176	,174	,172	,170	,168	,166	,164	,163	,161	,159	,157	,155	,153	,152	,150	,148	,
				147	,145	,144	,142	,141	,140	,139	,137	,136	,135	,134	,134	,133	,132	,131	,130	,130	,129	,129	,128	,128	,
				128	,127	,127	,127	,127	,127	,127	,127	,127	,127	,127	,127	,127	,128	,128	,128	,129	,129	,130	,130	,131	,
				131	,132	,133	,134	,134	,135	,136	,137	,138	,139	,140	,141	,142	,143	,144	,146	,147	,148	,149	,151	,152	,
				153	,155	,156	,158	,159	,161	,162	,164	,166	,167	,169	,170	,172	,174	,176	,177	,179	,181	,183	,185	,187	,
				188	,190	,192	,194	,196	,198	,200	,202	,204	,206	,208	,210	,212	,214	,216	,218	,221	,223	,225	,227	,229	,
				231	,233	,235	,238	,240	,242	,244	,246	,248	,251	,253 , 255 };

			// allow for a difference of 1 (rounding is not yet the same as in the original code, need to look into this...)
			bool isCorrect = CompareWithMargin(&splineLutTest[0], ExpectedResult, 256, 1);
			Assert::IsTrue(isCorrect, L"Incorrect result", LINE_INFO());
		}

		TEST_METHOD(TestMethod_LUTGeneraton_Gamma3)
		{
			auto gammaLutTest = libCZI::Utils::Create8BitLookUpTableFromGamma(256, 0, 1, 0.28218746779620463f);

			Assert::AreEqual(gammaLutTest.size(), (size_t)256, L"Incorrect result", LINE_INFO());

			static std::uint8_t ExpectedResult[256] = {
			0, 24, 35, 44, 51, 57, 62, 66, 70, 74, 77, 81, 84, 86, 89, 92, 94, 96, 99, 101, 103, 105, 107, 109, 110, 112, 114, 116, 117, 119, 120, 122, 123, 125,
			126, 128, 129, 130, 132, 133, 134, 135, 137, 138, 139, 140, 141, 142, 143, 145, 146, 147, 148, 149, 150, 151, 152, 153,
			154, 155, 156, 156, 157, 158, 159, 160, 161, 162, 163, 164, 164, 165, 166, 167, 168, 168, 169, 170, 171, 172, 172, 173,
			174, 175, 175, 176, 177, 177, 178, 179, 180, 180, 181, 182, 182, 183, 184, 184, 185, 186, 186, 187, 188, 188, 189, 189,
			190, 191, 191, 192, 193, 193, 194, 194, 195, 196, 196, 197, 197, 198, 198, 199, 200, 200, 201, 201, 202, 202, 203, 203,
			204, 204, 205, 206, 206, 207, 207, 208, 208, 209, 209, 210, 210, 211, 211, 212, 212, 213, 213, 214, 214, 215, 215, 216,
			216, 217, 217, 217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 222, 222, 223, 223, 224, 224, 225, 225, 226, 226, 226,
			227, 227, 228, 228, 229, 229, 229, 230, 230, 231, 231, 231, 232, 232, 233, 233, 233, 234, 234, 235, 235, 235, 236, 236,
			237, 237, 237, 238, 238, 239, 239, 239, 240, 240, 240, 241, 241, 242, 242, 242, 243, 243, 243, 244, 244, 245, 245, 245,
			246, 246, 246, 247, 247, 247, 248, 248, 249, 249, 249, 250, 250, 250, 251, 251, 251, 252, 252, 252, 253, 253, 253, 254,
			254, 254, 255, 255, 255, 255 };

			// allow for a difference of 1 (rounding is not yet the same as in the original code, need to look into this...)
			bool isCorrect = CompareWithMargin(&gammaLutTest[0], ExpectedResult, 256, 2);
			Assert::IsTrue(isCorrect, L"Incorrect result", LINE_INFO());
		}


	private:
		static bool CompareWithMargin(const std::uint8_t* p1, const std::uint8_t* p2, size_t elemCnt, int maxAllowedDifference)
		{
			for (size_t i = 0; i < elemCnt; ++i)
			{
				int diff = abs(int(*p1++) - int(*p2++));
				if (diff > maxAllowedDifference)
				{
					return false;
				}
			}

			return true;
		}
	};
}