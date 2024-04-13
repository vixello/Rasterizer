#include "Buffer.h"
#include "SaveToTga.h"

Buffer::Buffer(int width, int height) 
	: w(width), h(height){
	minx = 0;
	maxx = width - 1;
	miny = 0;
	maxy = height - 1;
	len = width * height;
}

void Buffer::SetSize(int width, int height)
{
	w = width;
	h = height;
	len = width * height;
	color.resize(len);
	depth.resize(len);
}

void Buffer::ClearColor(unsigned int colorToClear)
{
	std::fill(color.begin(), color.end(), colorToClear);
}

void Buffer::ClearDepth(unsigned int depthToSet)
{
	std::fill(depth.begin(), depth.end(), depthToSet);
}

void Buffer::SaveToTGA(const char* filename, unsigned short* header) {
	SaveToTga::SaveImage(filename, header, w, h, color);
}




