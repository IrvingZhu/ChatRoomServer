#pragma once
#include "./convert.hpp"
#include <string>

char* convertToNarrowChars(wchar_t *preChars)
{
// if you want to use this function to get a memory,
// you must need to clear when you exit before.

	std::wstring srcChars(preChars);
	auto res = convertToString(srcChars);

	char *afterChars = new char[128];
	memset(afterChars, 0, strlen(afterChars));

	strcpy(afterChars, res.c_str());
	if (strlen(afterChars) > 0)
		return afterChars;
	else
		return nullptr;
}