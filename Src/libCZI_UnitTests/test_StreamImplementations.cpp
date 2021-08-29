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

TEST(StreamImplementations, StreamInMemory1)
{
	std::uint8_t* buffer = (std::uint8_t*)malloc(10);
	for (int i = 0; i < 10; ++i)
	{
		buffer[i] = i + 1;
	}

	std::shared_ptr<const void> spBuffer(buffer, [](const void* p)->void {free(const_cast<void*>(p)); });
	auto stream = CreateStreamFromMemory(spBuffer, 10);

	std::uint8_t* bufferForRead = (std::uint8_t*)malloc(10);
	memset(bufferForRead, 0, 10);
	std::unique_ptr<std::uint8_t, void(*)(std::uint8_t*)> upBufferForRead(bufferForRead, [](std::uint8_t* p)->void {free(p); });
	std::uint64_t bytesRead;

	stream->Read(0, bufferForRead, 10, &bytesRead);

	EXPECT_EQ(bytesRead, 10) << "incorrect number of bytes read";
	bool correct = memcmp(buffer, bufferForRead, 10) == 0;
	EXPECT_TRUE(correct) << "incorrect result";
}

TEST(StreamImplementations, StreamInMemory2)
{
	std::uint8_t* buffer = (std::uint8_t*)malloc(10);
	for (int i = 0; i < 10; ++i)
	{
		buffer[i] = i + 1;
	}

	std::shared_ptr<const void> spBuffer(buffer, [](const void* p)->void {free(const_cast<void*>(p)); });
	auto stream = CreateStreamFromMemory(spBuffer, 10);

	std::uint8_t* bufferForRead = (std::uint8_t*)malloc(15);
	memset(bufferForRead, 0, 15);
	std::unique_ptr<std::uint8_t, void(*)(std::uint8_t*)> upBufferForRead(bufferForRead, [](std::uint8_t* p)->void {free(p); });
	std::uint64_t bytesRead;

	stream->Read(0, bufferForRead, 15, &bytesRead);

	EXPECT_EQ(bytesRead, 10) << "incorrect number of bytes read";
	bool correct = memcmp(buffer, bufferForRead, 10) == 0;
	EXPECT_TRUE(correct) << "incorrect result";

	EXPECT_TRUE(bufferForRead[10] == 0 && bufferForRead[11] == 0 && bufferForRead[12] == 0 &&
		bufferForRead[13] == 0 && bufferForRead[14] == 0) << "incorrect result";
}

TEST(StreamImplementations, StreamInMemory3)
{
	std::uint8_t* buffer = (std::uint8_t*)malloc(10);
	for (int i = 0; i < 10; ++i)
	{
		buffer[i] = i + 1;
	}

	std::shared_ptr<const void> spBuffer(buffer, [](const void* p)->void {free(const_cast<void*>(p)); });
	auto stream = CreateStreamFromMemory(spBuffer, 10);

	std::uint8_t* bufferForRead = (std::uint8_t*)malloc(15);
	memset(bufferForRead, 0, 15);
	std::unique_ptr<std::uint8_t, void(*)(std::uint8_t*)> upBufferForRead(bufferForRead, [](std::uint8_t* p)->void {free(p); });
	std::uint64_t bytesRead;

	stream->Read(9, bufferForRead, 3, &bytesRead);

	EXPECT_EQ(bytesRead, 1) << "incorrect number of bytes read";
	bool correct = memcmp(buffer + 9, bufferForRead, 1) == 0;
	EXPECT_TRUE(correct) << "incorrect result";

	EXPECT_TRUE(bufferForRead[1] == 0 && bufferForRead[2] == 0) << "incorrect result";
}
