#include "Tools.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <Windows.h>

bool skipSpace(std::string &line, size_t &offset)
{
	while (line[offset] == ' ' || line[offset] == '\t')
	{
		offset++;
		if (offset >= line.size())
			return false;
	}
	return true;
}

bool getWord(std::string &line, size_t &offset, std::string &outWord)
{
	size_t tmpOffset = offset;
	while (line[tmpOffset] != ' ' && line[tmpOffset] != '\t' && tmpOffset < line.size())
	{
		tmpOffset++;
	}
	const size_t count = tmpOffset - offset;
	if (count == 0)
		return false;
	outWord = line.substr(offset, count);
	offset = tmpOffset;
	if (offset >= line.size())
		return false;
	return true;
}

float getFloat(std::string &line, size_t &offset)
{
	size_t tmpOffset = offset;
	while (line[tmpOffset] != ' ' && line[tmpOffset] != '\t' && tmpOffset < line.size())
	{
		tmpOffset++;
	}
	const size_t count = tmpOffset - offset;
	if (count == 0)
		return 0.0f;
	if (offset >= line.size())
		return 0.0f;
	std::string substr = line.substr(offset, count);
	offset = tmpOffset;
	return stof(substr);
}
