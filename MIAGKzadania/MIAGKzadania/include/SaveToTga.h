#pragma once
#include <iostream>
#include <vector>

class SaveToTga
{
public:
	static void SaveImage(const char* filename, unsigned short* header, int width, int height, const std::vector<unsigned int>& buffer);
};

