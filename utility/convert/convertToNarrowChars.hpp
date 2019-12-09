#pragma once
#include "./convert.hpp"
#include <string>

int convertToNarrowChars(wchar_t *preChars, char *&afterChars)
{
	std::wstring srcChars(preChars);
	auto res = convertToString(srcChars);
	strcpy(afterChars, res.c_str());
	if (strlen(afterChars) > 0)
		return 1;
	else
		return 0;
}