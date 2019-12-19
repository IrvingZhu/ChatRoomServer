#pragma once
#include "./convert.hpp"
#include <string>

int convertToNarrowChars(wchar_t *preChars, char *&afterChars)
{
	std::wstring srcChars(preChars);
	auto res = convertToString(srcChars);
	if (afterChars == nullptr)
	{
		afterChars = new char[128];
		memset(afterChars, 0, strlen(afterChars));
	}
	strcpy(afterChars, res.c_str()); // has a fault
	if (strlen(afterChars) > 0)
		return 1;
	else
		return 0;
}