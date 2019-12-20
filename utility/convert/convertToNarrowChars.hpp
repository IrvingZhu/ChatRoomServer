#pragma once
#include "./convert.hpp"
#include <string>

char* convertToNarrowChars(wchar_t *preChars)
{
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