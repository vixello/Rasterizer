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
    std::vector<Vector3> vertexNormals_; // New: store vertex normals

    virtual void ApplyVertexShader() {
        vertexShader(vertices_, transform_);
        CalculateVertexNormals();

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
    void CalculateVertexNormals() {
        vertexNormals_.clear();
        vertexNormals_.resize(vertices_.size(), Vector3(0.0f, 0.0f, 0.0f));

        for (const auto& triangle : indices_) {
            const Vector3& v0 = Vector3(vertices_[triangle.a].x, vertices_[triangle.a].y, vertices_[triangle.a].z);
            const Vector3& v1 = Vector3(vertices_[triangle.b].x, vertices_[triangle.b].y, vertices_[triangle.b].z);
            const Vector3& v2 = Vector3(vertices_[triangle.c].x, vertices_[triangle.c].y, vertices_[triangle.c].z);

            Vector3 normal = Vector3::Cross(v1 - v0, v2 - v0).Normal();

            vertexNormals_[triangle.a] += normal;
            vertexNormals_[triangle.b] += normal;
            vertexNormals_[triangle.c] += normal;
        }

        // Normalize vertex normals
        for (auto& normal : vertexNormals_) {
            normal.Normal();
        }
    }
};