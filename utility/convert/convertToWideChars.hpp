#pragma once
#include "./convert.hpp"
#include <string>

int convertToWideChars(char *preString, wchar_t *&afterString) {
	std::string srcChars(preString);
	auto res=convertToWString(srcChars);
	wcscpy(afterString,res.c_str());
	if (wcslen(afterString) > 0)
		return 1;
	else
		return 0;
}