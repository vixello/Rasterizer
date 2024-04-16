#pragma once
#include "Buffer.h"
#include "Vector3.h"
//#include "flt.h"
#include <iostream>
#include <algorithm>



class Rasterizer {
public:
    Rasterizer();
    static void DrawTriangle(Buffer& buffer, const std::vector<Point>& vertices, unsigned int color1, unsigned int color2, unsigned int color3, bool isGouraud);
    static float edgeFunction(const Point& a, const Point& b, const Point& c);


private:
    //Buffer& buffer;
    static unsigned int interpolateColor(unsigned int color1, unsigned int color2, unsigned int color3, float b1, float b2, float b3);


};


