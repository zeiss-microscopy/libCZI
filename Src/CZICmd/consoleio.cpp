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
#include <cstdarg>
#include "consoleio.h"
#include <iostream>

using namespace std;

/*static*/std::shared_ptr<ILog> CConsoleLog::CreateInstance()
{
	return std::make_shared<CConsoleLog>();
}

void CConsoleLog::WriteLineStdOut(const char* sz)
{
	std::cout << sz << endl;
}

void CConsoleLog::WriteLineStdOut(const wchar_t* sz)
{
	std::wcout << sz << endl;
}

void CConsoleLog::WriteLineStdErr(const char* sz)
{
	std::cout << sz << endl;
}

void CConsoleLog::WriteLineStdErr(const wchar_t* sz)
{
	std::wcout << sz << endl;
}

void CConsoleLog::WriteStdOut(const char* sz)
{
	std::cout << sz;
}

void CConsoleLog::WriteStdOut(const wchar_t* sz)
{
	std::wcout << sz;
}

void CConsoleLog::WriteStdErr(const char* sz)
{
	std::cout << sz;
}

void CConsoleLog::WriteStdErr(const wchar_t* sz)
{
	std::wcout << sz;
}
