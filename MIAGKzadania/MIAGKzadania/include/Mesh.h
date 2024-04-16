#pragma once
#include <vector>
#include <cmath>
#include <iostream>
#include "Rasterizer.h"
#include "float4x4.h"
#include <algorithm>

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
    std::vector<Vector3> vertexColors_; // Store vertex colors for Gouraud shading

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
    //void CalculateVertexNormals(std::vector<Point>& vertices_) {
    //    vertexNormals_.clear();
    //    vertexNormals_.resize(vertices_.size(), Vector3(0.0f, 0.0f, 0.0f));

    //    for (const auto& triangle : indices_) {
    //        const Vector3& v0 = Vector3(vertices_[triangle.a].x, vertices_[triangle.a].y, vertices_[triangle.a].z);
    //        const Vector3& v1 = Vector3(vertices_[triangle.b].x, vertices_[triangle.b].y, vertices_[triangle.b].z);
    //        const Vector3& v2 = Vector3(vertices_[triangle.c].x, vertices_[triangle.c].y, vertices_[triangle.c].z);

    //        Vector3 normal = Vector3::Cross(v1 - v0, v2 - v0).Normal();

    //        vertexNormals_[triangle.a] += normal;
    //        vertexNormals_[triangle.b] += normal;
    //        vertexNormals_[triangle.c] += normal;
    //    }

    //    // Normalize vertex normals
    //    for (auto& normal : vertexNormals_) {
    //        normal = normal.Normal(); // Assign the normalized vector back to itself
    //    }
    //}
    void CalculateVertexNormals(std::vector<Point>& vertices_, std::vector<Triangle>& indices_, std::vector<Vector3>& vertexNormals_) {
        // Clear previous vertex normals
        vertexNormals_.clear();
        vertexNormals_.resize(vertices_.size(), Vector3(0.0f, 0.0f, 0.0f));

        // Iterate through each triangle
        for (const auto& triangle : indices_) {
            // Calculate triangle normal
            const Vector3& v1 = Vector3(vertices_[triangle.a].x, vertices_[triangle.a].y, vertices_[triangle.a].z);
            const Vector3& v2 = Vector3(vertices_[triangle.b].x, vertices_[triangle.b].y, vertices_[triangle.b].z);
            const Vector3& v3 = Vector3(vertices_[triangle.c].x, vertices_[triangle.c].y, vertices_[triangle.c].z);
            Vector3 edge1 = v2 - v1;
            Vector3 edge2 = v3 - v1;
            Vector3 normal = edge1.Cross(edge2);
            //normal = normal.Normal();

            // Accumulate normals for each vertex of the triangle
            vertexNormals_[triangle.a] += normal;
            vertexNormals_[triangle.b] += normal;
            vertexNormals_[triangle.c] += normal;
        }

        // Normalize vertex normals
        for (auto& normal : vertexNormals_) {
            normal = normal.Normal();
        }
    }

    void CalculateVertexColors(std::vector<Point>& vertices_, std::vector<Vector3>& vertexColors_) {
        vertexColors_.clear();
        vertexColors_.reserve(vertices_.size());

        // Iterate through each vertex
        for (size_t i = 0; i < vertices_.size(); ++i) {
            // Retrieve vertex position and normal
            const Vector3& vertexVec = Vector3(vertices_[i].x, vertices_[i].y, vertices_[i].z);
            const Vector3& normal = vertexNormals_[i];

            //std::cout << "vertexVec  " << vertexVec << std::endl;
            //std::cout << "vertexNormals_  " << vertexNormals_[i] << std::endl;

            // Initialize color for this vertex
            Vector3 vertexColor(0.0f, 0.0f, 0.0f);

            // Iterate through lights and calculate contributions
            for (const auto& light : buffer.lights) {
                // Calculate light contribution based on light type
                switch (light->type) {
                case LightType::Directional:
                    vertexColor += buffer.calculateGouraudDirectionalLighting(normal,  *static_cast<DirectionalLight*>(light)).Mul(light->color);
                    break;
                case LightType::Point:
                    vertexColor += buffer.calculateGouraudPointLighting(vertexVec, normal,  *static_cast<PointLight*>(light)).Mul(light->color);
                    //std::cout << buffer.calculatePointLighting(pixelPosition, interpolatedNormal, viewDirection, *static_cast<PointLight*>(light)) << std::endl;
                    break;
                case LightType::Spotlight:
                    vertexColor += buffer.calculateGouraudSpotlightLighting(vertexVec, normal,*static_cast<Spotlight*>(light)).Mul(light->color);
                    break;
                }
            }

            // Add calculated color to vertexColors_
            vertexColor.x = std::clamp(vertexColor.x, 0.0f, 1.0f);
            vertexColor.y = std::clamp(vertexColor.y, 0.0f, 1.0f);
            vertexColor.z = std::clamp(vertexColor.z, 0.0f, 1.0f);
            vertexColors_.push_back(vertexColor);
            //std::cout << "Vertex Color "<< i << "calcualted: " << vertexColor<< std::endl;

        }
    }

    static unsigned int ColorToUnsignedInt(const Vector3& color) {
        return 0xFF000000 | // Alpha channel (fully opaque)
            ((static_cast<unsigned int>(color.x * 255.0f) & 0xFF) << 16) | // Red channel
            ((static_cast<unsigned int>(color.y * 255.0f) & 0xFF) << 8) | // Green channel
            (static_cast<unsigned int>(color.z * 255.0f) & 0xFF);          // Blue channel
    }
   static Vector3 ColorFromUnsignedInt(unsigned int color) {
        float red = ((color >> 16) & 0xFF) / 255.0f;
        float green = ((color >> 8) & 0xFF) / 255.0f;
        float blue = (color & 0xFF) / 255.0f;
        return Vector3(red, green, blue);
    }

};