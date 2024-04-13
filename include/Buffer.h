#pragma once
#include <vector>


class Buffer {

public:
	std::vector<float> depth;
	std::vector<unsigned int> color;
	Buffer(int width, int height);
	int getWidth() { return w; };
	int getHeight() { return h; };

	void SetSize(int width, int height);
	void ClearColor(unsigned int colorToClear);
	void ClearDepth(unsigned int depthToSet);
	void SaveToTGA(const char* filename, unsigned short* header);

private:
	int w, h;
	float minx, maxx, miny, maxy, len;

};

