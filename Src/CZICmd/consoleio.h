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
	virtual void WriteStdOut(const char* sz) = 0;
	virtual void WriteStdOut(const wchar_t* sz) = 0;
	virtual void WriteStdErr(const char* sz) = 0;
	virtual void WriteStdErr(const wchar_t* sz) = 0;

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


	//virtual void WriteStdErr(const wchar_t* fmt, ...) = 0;
	//virtual void WriteStdOut(const wchar_t* fmt, ...) = 0;
	//virtual void WriteStdOutLine(const wchar_t* fmt, ...) = 0;
	//virtual bool IsInfoEnabled(int level) const = 0;
	//virtual void WriteInfo(int level, const wchar_t* fmt, ...) = 0;
	//virtual void WriteInfoString(int level, const wchar_t* fmt) = 0;

	//void WriteInfoString(int level, const std::wstring& str)
	//{
	//	this->WriteInfoString(level, str.c_str());
	//}

	//void WriteStdOutLine(void) { this->WriteStdOutLine(L""); }

	virtual ~ILog() {}
};

class CConsoleLog : public ILog
{
public:
	static std::shared_ptr<ILog> CreateInstance();

	virtual void WriteStdOut(const char* sz);
	virtual void WriteStdOut(const wchar_t* sz);
	virtual void WriteStdErr(const char* sz);
	virtual void WriteStdErr(const wchar_t* sz);
/*	virtual void WriteStdErr(const wchar_t* fmt, ...);
	virtual void WriteStdOut(const wchar_t* fmt, ...);
	virtual void WriteStdOutLine(const wchar_t* fmt, ...);
	virtual bool IsInfoEnabled(int level) const;
	virtual void WriteInfo(int level, const wchar_t* fmt, ...);
	virtual void WriteInfoString(int level, const wchar_t* fmt);*/
protected:
/*	void Write(FILE* fp, const wchar_t* fmt, va_list argptr);
	void WriteString(FILE* fp, const wchar_t* s);*/
};