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
using namespace std;

namespace UnitTest
{
	TEST_CLASS(UnitTest_DimCoordinate)
	{
	public:
		TEST_METHOD(TestMethod_ReaderException)
		{
			class CTestStreamImp :public libCZI::IStream
			{
			private:
				std::string exceptionText;
				std::error_code code;
			public:
				CTestStreamImp(const std::string& exceptionText, std::error_code code) :exceptionText(exceptionText), code(code) {}

				virtual void Read(std::uint64_t offset, void *pv, std::uint64_t size, std::uint64_t* ptrBytesRead) override
				{
					throw std::ios_base::failure(this->exceptionText, this->code);
				}
			};

			static const char* ExceptionText = "Test-1";
			static std::error_code ErrorCode = error_code(42, generic_category());
			auto stream = std::make_shared<CTestStreamImp>(ExceptionText, ErrorCode);
			auto spReader = libCZI::CreateCZIReader();
			bool exceptionCorrect = false;
			try
			{
				spReader->Open(stream);
			}
			catch (LibCZIIOException& excp)
			{
				try
				{
					excp.rethrow_nested();
				}
				catch (std::ios_base::failure& innerExcp)
				{
					// according to standard, the content of the what()-test is implementation-specific,
					// so it is not suited for checking - but it seems that the code goes unaltered
					const char* errorText = innerExcp.what();
					error_code errCode = innerExcp.code();
					if (errCode == ErrorCode)
					{
						exceptionCorrect = true;
					}
				}
			}

			Assert::IsTrue(exceptionCorrect, L"Incorrect result", LINE_INFO());
		}

		TEST_METHOD(TestMethod_ReaderException2)
		{
			class CTestStreamImp :public libCZI::IStream
			{
			public:
				virtual void Read(std::uint64_t offset, void *pv, std::uint64_t size, std::uint64_t* ptrBytesRead) override
				{
					if (offset != 0 || size < 16)
					{
						throw std::ios_base::failure("UNEXPECTED");
					}

					static const std::uint8_t FILEHDRMAGIC[16] = { 'Z','I','S','R','A','W','F','I','L' + 1,'E','\0','\0','\0','\0','\0','\0' };
					memset(pv, 0, (size_t)size);
					memcpy(pv, FILEHDRMAGIC, 16);
					*ptrBytesRead = size;
				}
			};

			static const char* ExceptionText = "Test-1";
			static std::error_code ErrorCode = error_code(42, generic_category());
			auto stream = std::make_shared<CTestStreamImp>();
			auto spReader = libCZI::CreateCZIReader();
			bool exceptionCorrect = false;
			try
			{
				spReader->Open(stream);
			}
			catch (LibCZICZIParseException& excp)
			{
				auto errCode = excp.GetErrorCode();
				if (errCode == LibCZICZIParseException::ErrorCode::CorruptedData)
				{
					exceptionCorrect = true;
				}
			}

			Assert::IsTrue(exceptionCorrect, L"Incorrect result", LINE_INFO());
		}

		TEST_METHOD(TestMethod_ReaderStateException)
		{
			bool expectedExceptionCaught = false;
			auto spReader = libCZI::CreateCZIReader();
			try
			{
				spReader->GetStatistics();
			}
			catch (logic_error)
			{
				expectedExceptionCaught = true;
			}

			Assert::IsTrue(expectedExceptionCaught == true, L"Incorrect behavior", LINE_INFO());
		}
	};
}