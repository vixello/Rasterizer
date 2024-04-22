#pragma once
#include "Buffer.h"
#include "Vector3.h"
//#include "flt.h"
#include <iostream>
#include <algorithm>



class Rasterizer {
public:
    Rasterizer();
    static void DrawTriangle(Buffer& buffer, const std::vector<Point>& vertices, const std::vector<Vector3>& normals, unsigned int color1, 
        unsigned int color2, unsigned int color3, const std::vector<Vector2>& texCoords, unsigned int textureID, bool isGouraud, bool lightOn);
    static float edgeFunction(const Point& a, const Point& b, const Point& c);


private:
    //Buffer& buffer;
    static unsigned int interpolateColor(unsigned int color1, unsigned int color2, unsigned int color3, float b1, float b2, float b3);

    static Vector3 SampleTexture(Buffer& buffer, const Vector2& texCoord, unsigned int textureID);


};


