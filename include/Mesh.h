#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include "Rasterizer.h"
#include "float4x4.h"

class Mesh {
public:
    Mesh(Buffer& buffer, float4x4 transform) : buffer(buffer), transform_(transform) {}

    virtual void Generate() = 0;
    virtual void Draw(Rasterizer& r, unsigned int color1, unsigned int color2, unsigned int color3) const = 0;

protected:
    Buffer& buffer;
    float4x4 transform_;

    struct Triangle {
        int a, b, c;
    };

    std::vector<Point> vertices_;
    std::vector<Triangle> indices_;

    virtual void ApplyVertexShader() {
        vertexShader(vertices_, transform_);
    }

    static void vertexShader(std::vector<Point>& vertices, const float4x4& transform) {
        for (auto& vertex : vertices) {
            Vector4 vertexVec(vertex.x, vertex.y, vertex.z, 1.0f);
            vertexVec = transform * vertexVec;
            vertex.x = vertexVec.x;
            vertex.y = vertexVec.y;
            vertex.z = vertexVec.z;
        }
    }

};