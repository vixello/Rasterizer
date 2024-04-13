#pragma once
#include "Mesh.h"


class TorusMesh : public Mesh {
public:
    TorusMesh(int radialSegments, int tubularSegments, float radius, float tubeRadius, Buffer& buffer, float4x4 transform)
        : Mesh(buffer, transform), radialSegments_(radialSegments), tubularSegments_(tubularSegments), radius_(radius), tubeRadius_(tubeRadius) {}

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
            std::cout << "Triangle in torus" << triangle.a << " " << triangle.b << " " << triangle.c << " has been drawn" << std::endl;
        }
    }
private:
    int radialSegments_; // Number of segments around the central axis of the torus
    int tubularSegments_; // Number of segments around the thickness of the torus
    float radius_; // Central radius of the torus
    float tubeRadius_; // Radius of the tube
    
    struct Triangle {
        int a, b, c;
    };

    std::vector<Point> vertices_;
    std::vector<Triangle> indices_;

    void GenerateVertices() {
        vertices_.clear();

        for (int i = 0; i <= radialSegments_; ++i) {
            // Calculate the circular angle around the torus
            float theta = 2 * M_PI * i / radialSegments_;
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            for (int j = 0; j <= tubularSegments_; ++j) {
                // Calculate the circular angle around the tube
                float phi = 2 * M_PI * j / tubularSegments_;
                float cosPhi = cos(phi);
                float sinPhi = sin(phi);

                // Calculate the vertex position
                float x = (radius_ + tubeRadius_ * cosPhi) * cosTheta;
                float y = (radius_ + tubeRadius_ * cosPhi) * sinTheta;
                float z = tubeRadius_ * sinPhi;
                
                vertices_.emplace_back(x, y, z);
            }
        }
    }

    void GenerateIndices() {
        indices_.clear();

        for (int i = 0; i < radialSegments_; ++i) {
            for (int j = 0; j < tubularSegments_; ++j) {
                int a = (i * (tubularSegments_ + 1)) + j;
                int b = ((i + 1) * (tubularSegments_ + 1)) + j;
                int c = ((i + 1) * (tubularSegments_ + 1)) + (j + 1);
                int d = (i * (tubularSegments_ + 1)) + (j + 1);

                indices_.push_back({ a, b, d });
                indices_.push_back({ b, c, d });
            }
        }
    }
    void ApplyVertexShader() {
        vertexShader(vertices_, transform_);
    }
};