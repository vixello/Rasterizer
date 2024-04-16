#pragma once
#include "Mesh.h"


class TorusMesh : public Mesh {
public:
    TorusMesh(int radialSegments, int tubularSegments, float radius, float tubeRadius, Buffer& buffer, float4x4 transform)
        : Mesh(buffer, transform), radialSegments_(radialSegments), tubularSegments_(tubularSegments), radius_(radius), tubeRadius_(tubeRadius) {}

    void Generate() override {
        GenerateVertices();
        GenerateIndices();
        CalculateVertexNormals(vertices_,indices_,vertexNormals_);
        ApplyVertexShader();
    }
    void Draw(Rasterizer& r, unsigned int color1, unsigned int color2, unsigned int color3) const override {
        for (const auto& triangle : indices_) {
            Point p1 = vertices_[triangle.a];
            Point p2 = vertices_[triangle.b];
            Point p3 = vertices_[triangle.c];
            std::vector<Point> vertices = { p1, p2, p3 };
            r.DrawTriangle(buffer, vertices, color1, color2, color3, false);
            std::cout << "Triangle in torus" << triangle.a << " " << triangle.b << " " << triangle.c << " has been drawn" << std::endl;
        }
    }

    void DrawGouraud(Rasterizer& r, unsigned int color1, unsigned int color2, unsigned int color3) const {
        // Debug output
        std::cout << "Vertex Colors size: " << vertexColors_.size() << std::endl;
        std::cout << "Vertices size: " << vertices_.size() << std::endl;

        for (const auto& triangle : indices_) {
            // Ensure indices are within bounds
            if (triangle.a < 0 || triangle.a >= vertexColors_.size() ||
                triangle.b < 0 || triangle.b >= vertexColors_.size() ||
                triangle.c < 0 || triangle.c >= vertexColors_.size() ||
                triangle.a < 0 || triangle.a >= vertices_.size() ||
                triangle.b < 0 || triangle.b >= vertices_.size() ||
                triangle.c < 0 || triangle.c >= vertices_.size()) {
                std::cerr << "Error: Triangle indices out of bounds" << std::endl;
                continue; // Skip this triangle and proceed to the next one
            }

            // Retrieve vertex colors for the triangle vertices
            Vector3 colorVec1 = vertexColors_[triangle.a];
            Vector3 colorVec2 = vertexColors_[triangle.b];
            Vector3 colorVec3 = vertexColors_[triangle.c];

            // Convert vertex colors to unsigned int
            unsigned int vertexColor1 = ColorToUnsignedInt(colorVec1);
            unsigned int vertexColor2 = ColorToUnsignedInt(colorVec2);
            unsigned int vertexColor3 = ColorToUnsignedInt(colorVec3);
            // Blend the passed colors with the calculated vertex colors
            Vector3 passedColor1 = ColorFromUnsignedInt(color1);
            Vector3 passedColor2 = ColorFromUnsignedInt(color2);
            Vector3 passedColor3 = ColorFromUnsignedInt(color3);

            Vector3 finalColor1 = passedColor1 + colorVec1;
            Vector3 finalColor2 = passedColor2 + colorVec2;
            Vector3 finalColor3 = passedColor3 + colorVec3;

            // Ensure final colors are within the range [0, 1]
            finalColor1 = finalColor1.Clamp();
            finalColor2 = finalColor2.Clamp();
            finalColor3 = finalColor3.Clamp();

            // Convert final colors back to unsigned int
            unsigned int finalVertexColor1 = ColorToUnsignedInt(finalColor1);
            unsigned int finalVertexColor2 = ColorToUnsignedInt(finalColor2);
            unsigned int finalVertexColor3 = ColorToUnsignedInt(finalColor3);
            // Retrieve vertices for the triangle
            Point p1 = vertices_[triangle.a];
            Point p2 = vertices_[triangle.b];
            Point p3 = vertices_[triangle.c];

            // Create vertices with colors
            std::vector<Point> vertices = { p1, p2, p3 };

            // Draw triangle with colors
            r.DrawTriangle(buffer, vertices, finalVertexColor1, finalVertexColor2, finalVertexColor3, true);
            std::cout << "Triangle in torus " << triangle.a << " " << triangle.b << " " << triangle.c << " has been drawn" << std::endl;
            std::cout << "Vertex Colors torus: " << vertexColor1 << "  "<< vertexColor2 << "  "<< vertexColor3 << std::endl;
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
    //std::vector<Triangle> indices_;

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
        CalculateVertexColors(vertices_, vertexColors_);
    }
};