#pragma once
#include "Mesh.h"

class ConeMesh : public Mesh {
public:
    ConeMesh(int segments, float radius, float height, Buffer& buffer, float4x4 transform)
        : Mesh(buffer, transform), segments_(segments), radius_(radius), height_(height){}

    void Generate() override {
        GenerateVertices();
        GenerateIndices();
        ApplyVertexShader();
    }

    void Draw(Rasterizer& r, unsigned int color1, unsigned int color2, unsigned int color3) const override {
        for (const auto& triangle : indices_) {
            Point p1 = vertices_[triangle.a];
            Point p2 = vertices_[triangle.b];
            Point p3 = vertices_[triangle.c];
            std::vector<Point> vertices = { p1, p2, p3 };
            r.DrawTriangle(buffer, vertices, color1, color2, color3);
            std::cout << "Triangle in cone" << triangle.a << " " << triangle.b << " " << triangle.c << " has been drawn" << std::endl;
        }
    }

private:
    int segments_; // Number of segments forming the base circle
    float radius_; // Radius of the cone's base circle
    float height_; // Height of the cone
    //Buffer& buffer;
    //float4x4 transform_;

    struct Triangle {
        int a, b, c;
    };

    std::vector<Point> vertices_;
    std::vector<Triangle> indices_;

    void GenerateVertices() {
        // Clear the previous vertices
        vertices_.clear();

        // Apex of the cone
        vertices_.emplace_back(0, -height_, 0);

        // Base circle vertices
        for (int i = 0; i < segments_; ++i) {
            float angle = 2 * M_PI * i / segments_;
            float x = radius_ * cos(angle);
            float z = radius_ * sin(angle);
            vertices_.emplace_back(x, 0, z);
        }

        // Center of the base circle
        vertices_.emplace_back(0, 0, 0);
    }

    void GenerateIndices() {
        // Clear the previous indices
        indices_.clear();

        // Side faces
        for (int i = 0; i < segments_; ++i) {
            int nextIndex = (i + 1) % segments_;
            indices_.push_back({ 0, i + 1, nextIndex + 1 });
        }

        // Base faces
        int baseCenterIndex = segments_ + 1;
        for (int i = 0; i < segments_; ++i) {
            int nextIndex = (i + 1) % segments_;
            indices_.push_back({ baseCenterIndex, nextIndex + 1, i + 1 });
        }
    }

    //void GenerateIndices() {
    //    indices_.clear();

    //    // Create triangles connecting the apex to each pair of adjacent vertices on the base circle
    //    for (int i = 0; i < segments_; ++i) {
    //        indices_.push_back({ 0, i + 1, (i + 1) % segments_ + 1 });
    //    }
    //}

    void ApplyVertexShader() {
        vertexShader(vertices_, transform_);
    }
};
