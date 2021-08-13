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
#include "utilities.h"
#include <locale>
#include <codecvt>
#if !defined(_WIN32)
#include <random>
#endif
#include "inc_libCZI_Config.h"
#if LIBCZI_HAVE_ENDIAN_H
 #include "endian.h"
#endif

using namespace std;

/*static*/std::uint8_t Utilities::HexCharToInt(char c)
{
    switch (c)
    {
    case '0':return 0;
    case '1':return 1;
    case '2':return 2;
    case '3':return 3;
    case '4':return 4;
    case '5':return 5;
    case '6':return 6;
    case '7':return 7;
    case '8':return 8;
    case '9':return 9;
    case 'A':case 'a':return 10;
    case 'B':case 'b':return 11;
    case 'C':case 'c':return 12;
    case 'D':case 'd':return 13;
    case 'E':case 'e':return 14;
    case 'F':case 'f':return 15;
    }

    return 0xff;
}

template <class tString>
tString trimImpl(const tString& str, const tString& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == tString::npos)
        return tString(); // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

/*static*/std::string Utilities::Trim(const std::string& str, const std::string& whitespace /*= " \t"*/)
{
    return trimImpl(str, whitespace);
}

/*static*/std::wstring Utilities::Trim(const std::wstring& str, const std::wstring& whitespace /*= L" \t"*/)
{
    return trimImpl(str, whitespace);
}


/*static*/bool Utilities::icasecmp(const std::string& l, const std::string& r)
{
    return l.size() == r.size()
        && equal(l.cbegin(), l.cend(), r.cbegin(),
            [](std::string::value_type l1, std::string::value_type r1)
            { return toupper(l1) == toupper(r1); });
}

/*static*/bool Utilities::icasecmp(const std::wstring& l, const std::wstring& r)
{
    return l.size() == r.size()
        && equal(l.cbegin(), l.cend(), r.cbegin(),
            [](std::wstring::value_type l1, std::wstring::value_type r1)
            { return towupper(l1) == towupper(r1); });
}

/*static*/std::wstring Utilities::convertUtf8ToWchar_t(const char* sz)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8conv;
    std::wstring conv = utf8conv.from_bytes(sz);
    return conv;
}

/*static*/void Utilities::Tokenize(const std::wstring& str, std::vector<std::wstring>& tokens, const std::wstring& delimiters)
{
    // Skip delimiters at beginning.
    wstring::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    wstring::size_type pos = str.find_first_of(delimiters, lastPos);

    while (wstring::npos != pos || wstring::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/*static*/GUID Utilities::GenerateNewGuid()
{
#if defined(_WIN32)
    GUID guid;
    CoCreateGuid(&guid);
    return guid;
#else
    std::mt19937 rng;
    rng.seed(std::random_device()());
    uniform_int_distribution<uint32_t> distu32;
    GUID g;
    g.Data1 = distu32(rng);
    auto r = distu32(rng);
    g.Data2 = (uint16_t)r;
    g.Data3 = (uint16_t)(r >> 16);
    r = distu32(rng);
    for (int i = 0; i < 8; ++i)
    {
        g.Data4[0] = (uint8_t)r;
        r >>= 8;
    }

    return g;
#endif
}

/*static*/bool Utilities::TryGetRgb8ColorFromString(const std::wstring& strXml, libCZI::Rgb8Color& color)
{
    auto str = Utilities::Trim(strXml);
    if (str.size() > 1 && str[0] == '#')
    {
        // TODO: do we have to deal with shorter string (e.g. without alpha, or just '#FF')?
        std::uint8_t r, g, b, a;
        r = g = b = a = 0;
        for (size_t i = 1; i < (std::max)((size_t)9, (size_t)str.size()); ++i)
        {
            if (!isxdigit(str[i]))
            {
                return false;
            }

            switch (i)
            {
            case 1: a = Utilities::HexCharToInt((char)str[i]); break;
            case 2: a = (a << 4) + Utilities::HexCharToInt((char)str[i]); break;
            case 3: r = Utilities::HexCharToInt((char)str[i]); break;
            case 4: r = (r << 4) + Utilities::HexCharToInt((char)str[i]); break;
            case 5: g = Utilities::HexCharToInt((char)str[i]); break;
            case 6: g = (g << 4) + Utilities::HexCharToInt((char)str[i]); break;
            case 7: b = Utilities::HexCharToInt((char)str[i]); break;
            case 8: b = (b << 4) + Utilities::HexCharToInt((char)str[i]); break;
            }
        }

        color = libCZI::Rgb8Color{ r,g,b };
        return true;
    }

    return false;
}

/*static*/bool Utilities::IsGuidNull(const GUID& g)
{
    return g.Data1 == 0 && g.Data2 == 0 && g.Data3 == 0 &&
        g.Data4[0] == 0 && g.Data4[1] == 0 && g.Data4[2] == 0 && g.Data4[3] == 0 &&
        g.Data4[4] == 0 && g.Data4[5] == 0 && g.Data4[6] == 0 && g.Data4[7] == 0;
}

/*static*/void Utilities::ConvertInt16ToHostByteOrder(std::int16_t* p)
{
#if LIBCZI_ISBIGENDIANHOST
#if LIBCZI_HAVE_ENDIAN_H
#if LIBCZI_SIGBUS_ON_UNALIGNEDINTEGERS
    int16_t x;
    memcpy(&x, p, sizeof(int16_t));
    x = le16toh(x);
    memcpy(p, &x, sizeof(int16_t));
#else
    * p = le16toh(*p);
#endif
#else
#if LIBCZI_SIGBUS_ON_UNALIGNEDINTEGERS
    int16_t v;
    memcpy(&v, p, sizeof(int16_t));
#else
    int16_t v = *p;
#endif
    v = ((v & 0x00ff) << 8) | ((v & 0xff00) >> 8);
#if LIBCZI_SIGBUS_ON_UNALIGNEDINTEGERS
    memcpy(p, &v, sizeof(int16_t));
#else
    * p = v;
#endif
#endif
#endif
}

/*static*/void Utilities::ConvertInt32ToHostByteOrder(std::int32_t* p)
{
#if LIBCZI_ISBIGENDIANHOST
#if LIBCZI_HAVE_ENDIAN_H
#if LIBCZI_SIGBUS_ON_UNALIGNEDINTEGERS
    int32_t x;
    memcpy(&x, p, sizeof(int32_t));
    x = le32toh(x);
    memcpy(p, &x, sizeof(int32_t));
#else
    * p = le32toh(*p);
#endif
#else
#if LIBCZI_SIGBUS_ON_UNALIGNEDINTEGERS
    int32_t v;
    memcpy(&v, p, sizeof(int32_t));
#else
    int32_t v = *p;
#endif
    v = ((((v) & 0xff000000u) >> 24) | (((v) & 0x00ff0000u) >> 8)
        | (((v) & 0x0000ff00u) << 8) | (((v) & 0x000000ffu) << 24));
#if LIBCZI_SIGBUS_ON_UNALIGNEDINTEGERS
    memcpy(p, &v, sizeof(int32_t));
#else
    * p = v;
#endif
#endif
#endif
}

/*static*/void Utilities::ConvertInt64ToHostByteOrder(std::int64_t* p)
{
#if LIBCZI_ISBIGENDIANHOST
#if LIBCZI_HAVE_ENDIAN_H
#if LIBCZI_SIGBUS_ON_UNALIGNEDINTEGERS
    int64_t x;
    memcpy(&x, p, sizeof(int64_t));
    x = le64toh(x);
    memcpy(p, &x, sizeof(int64_t));
#else
    * p = le64toh(*p);
#endif
#else
#if LIBCZI_SIGBUS_ON_UNALIGNEDINTEGERS
    uint64_t v;
    memcpy(&v, p, sizeof(uint64_t));
#else
    uint64_t v = *((uint64_t*)p);
#endif
    v = ((((v) & 0xff00000000000000ull) >> 56)
        | (((v) & 0x00ff000000000000ull) >> 40)
        | (((v) & 0x0000ff0000000000ull) >> 24)
        | (((v) & 0x000000ff00000000ull) >> 8)
        | (((v) & 0x00000000ff000000ull) << 8)
        | (((v) & 0x0000000000ff0000ull) << 24)
        | (((v) & 0x000000000000ff00ull) << 40)
        | (((v) & 0x00000000000000ffull) << 56));
    *p = (int64_t)v;
#if LIBCZI_SIGBUS_ON_UNALIGNEDINTEGERS
    memcpy(p, &v, sizeof(uint64_t));
#else
    * p = (int64_t)v;
#endif
#endif
#endif
}

/*static*/void Utilities::ConvertGuidToHostByteOrder(GUID* p)
{
#if LIBCZI_ISBIGENDIANHOST
    Utilities::ConvertInt32ToHostByteOrder((int32_t*)&(p->Data1));
    Utilities::ConvertInt16ToHostByteOrder((int16_t*)&(p->Data2));
    Utilities::ConvertInt16ToHostByteOrder((int16_t*)&(p->Data3));
#endif
}
