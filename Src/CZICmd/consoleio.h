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

#pragma once


class ILog
{
public:
	virtual void WriteLineStdOut(const char* sz) = 0;
	virtual void WriteLineStdOut(const wchar_t* sz) = 0;
	virtual void WriteLineStdErr(const char* sz) = 0;
	virtual void WriteLineStdErr(const wchar_t* sz) = 0;

	virtual void WriteStdOut(const char* sz) = 0;
	virtual void WriteStdOut(const wchar_t* sz) = 0;
	virtual void WriteStdErr(const char* sz) = 0;
	virtual void WriteStdErr(const wchar_t* sz) = 0;

	void WriteLineStdOut(const std::string& str)
	{
		this->WriteLineStdOut(str.c_str());
	}

	void WriteLineStdOut(const std::wstring& str)
	{
		this->WriteLineStdOut(str.c_str());
	}

	void WriteLineStdErr(const std::string& str)
	{
		this->WriteLineStdErr(str.c_str());
	}

	void WriteLineStdErr(const std::wstring& str)
	{
		this->WriteLineStdErr(str.c_str());
	}

	void WriteStdOut(const std::string& str)
	{
		this->WriteStdOut(str.c_str());
	}

	void WriteStdOut(const std::wstring& str)
	{
		this->WriteStdOut(str.c_str());
	}

	void WriteStdErr(const std::string& str)
	{
		this->WriteStdErr(str.c_str());
	}

	void WriteStdErr(const std::wstring& str)
	{
		this->WriteStdErr(str.c_str());
	}
	
	virtual ~ILog() = default;
};

class CConsoleLog : public ILog
{
public:
	static std::shared_ptr<ILog> CreateInstance();

	void WriteLineStdOut(const char* sz) override;
	void WriteLineStdOut(const wchar_t* sz) override;
	void WriteLineStdErr(const char* sz) override;
	void WriteLineStdErr(const wchar_t* sz) override;

	void WriteStdOut(const char* sz) override;
	void WriteStdOut(const wchar_t* sz) override;
	void WriteStdErr(const char* sz) override;
	void WriteStdErr(const wchar_t* sz) override;
};