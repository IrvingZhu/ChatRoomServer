#pragma once

#include <string>
#include <cstdlib>
#include <iostream>
#include <cstring>

std::string convertToString(const std::wstring &ws)
{
    // std::string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
    // setlocale(LC_ALL, "chs");                        //中文地区
    const wchar_t *_Source = ws.c_str();
    size_t _Dsize = ws.size() + 1; //比要转换的宽字符个数大1
    char *_Dest = new char[_Dsize];
    memset(_Dest, 0, _Dsize); //初始化缓冲区
    size_t i;
    wcstombs_s(&i, _Dest, _Dsize, _Source, ws.size());
    std::cout << "i:" << i << std::endl;
    std::string result = _Dest;
    delete[] _Dest;
    // setlocale(LC_ALL, curLocale.c_str()); //设置回原来的locale
    return result;
}


std::wstring convertToWString(const std::string &s)
{
    // std::string curlLocale = setlocale(LC_ALL, NULL);
    // setlocale(LC_ALL, "chs");
    const char *_Source = s.c_str();
    size_t _Dsize = s.size() + 1;

    wchar_t *_Dest = new wchar_t[_Dsize];
    size_t i;
    mbstowcs_s(&i, _Dest, _Dsize, _Source, s.size());
    std::wstring result = _Dest;
    delete[] _Dest;
    // setlocale(LC_ALL, curlLocale.c_str());
    return result;
}