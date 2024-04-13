#pragma once
#include "Mesh.h"

class SphereMesh: public Mesh {
public:
    SphereMesh(int vert, int horiz, float radius, Buffer& buffer, float4x4 transform) 
        : Mesh(buffer, transform), vert_(vert), horiz_(horiz), radius_(radius){}

    void Generate() override{
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
            std::cout << "Triangle in sphere" << triangle.a<<" "<< triangle.b << " " << triangle.c << " has been drawn" << std::endl;
        }
    }

private:
    int vert_; // Number of vertices vertically
    int horiz_; // Number of vertices horizontally
    float radius_; // Radius of the grid
    //Buffer& buffer;
    //float4x4 transform_;

    struct int3 {
        int a, b, c;
    };

    std::vector<Point> vertices_{};
    std::vector<int3> indices_;

    void GenerateVertices() {
        vertices_.clear();
        for (int yy = 0; yy <= horiz_ + 1; ++yy) {
            float y = std::cos(yy * M_PI / (horiz_ + 1.f)); // Calculate y-coordinate
            float r = std::sqrt(1 - y * y); // Calculate radius based on y
            for (int rr = 0; rr < vert_; ++rr) {
                float x = r * std::cos(2 * M_PI * rr / vert_); // Calculate x-coordinate
                float z = r * std::sin(2 * M_PI * rr / vert_); // Calculate z-coordinate
                vertices_.emplace_back(x * radius_, y * radius_, z * radius_); // Add vertex to the vector
            }
        }
    }

    void GenerateIndices() {
        indices_.clear();
        for (int yy = 0; yy < horiz_; ++yy) {
            for (int rr = 0; rr < vert_; ++rr) {
                indices_.push_back({ (rr + 1) % vert_ + yy * vert_,
                                     rr + vert_ + yy * vert_,
                                     (rr + 1) % vert_ + vert_ + yy * vert_ });

                indices_.push_back({ rr + vert_ + yy * vert_,
                                     rr + 2 * vert_ + yy * vert_,
                                     (rr + 1) % vert_ + vert_ + yy * vert_ });
            }
        }
    }

    void ApplyVertexShader() {
        vertexShader(vertices_, transform_);
    }

};
