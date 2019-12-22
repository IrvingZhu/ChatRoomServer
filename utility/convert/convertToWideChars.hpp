#pragma once
#include "./convert.hpp"
#include <string>

wchar_t* convertToWideChars(char *preString)
{
// if you want to use this function to get a memory,
// you must need to clear when you exit before.

	wchar_t *afterString = new wchar_t[128];
	memset(afterString, 0, wcslen(afterString));

	std::string srcChars(preString);
	auto res = convertToWString(srcChars);
	wcscpy(afterString, res.c_str());

	if (wcslen(afterString) > 0)
		return afterString;
	else
		return nullptr;
}