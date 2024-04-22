#include "Mesh.h"

class SphereMesh: public Mesh {
public:
    SphereMesh(int vert, int horiz, float radius, Buffer& buffer, float4x4 transform) 
        : Mesh(buffer, transform), vert_(vert), horiz_(horiz), radius_(radius){}

    void Generate() override{
        GenerateVertices();
        GenerateIndices();
        CalculateVertexNormals(vertices_, indices_, vertexNormals_);
        ApplyVertexShader();

    }

    void Draw(Rasterizer& r, unsigned int textureId, bool lightOn, unsigned int color1, unsigned int color2, unsigned int color3) const override {
        for (const auto& triangle : indices_) {
            Point p1 = vertices_[triangle.a];
            Point p2 = vertices_[triangle.b];
            Point p3 = vertices_[triangle.c];
            std::vector<Point> vertices = { p1, p2, p3 };
            std::vector<Vector3> normals = { vertexNormals_[triangle.a], vertexNormals_[triangle.b], vertexNormals_[triangle.c] };

            std::vector<Vector2> texCoords = { texCoords_[triangle.a], texCoords_[triangle.b], texCoords_[triangle.c] }; 

            r.DrawTriangle(buffer, vertices, normals, color1, color2, color3, texCoords, textureId, false, lightOn);
            std::cout << "Triangle in sphere" << triangle.a << " " << triangle.b << " " << triangle.c << " has been drawn" << std::endl;
        }
    }
    void DrawGouraud(Rasterizer& r, bool lightOn, unsigned int color1, unsigned int color2, unsigned int color3) const {
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
            //r.DrawTriangle(buffer, vertices, vertexNormals_, finalVertexColor1, finalVertexColor2, finalVertexColor3, true);
            std::vector<Vector2> texCoords = { texCoords_[triangle.a], texCoords_[triangle.b], texCoords_[triangle.c] };

            r.DrawTriangle(buffer, vertices, vertexNormals_, finalVertexColor1, finalVertexColor2, finalVertexColor3, texCoords, textureID, true, lightOn);
            std::cout << "Triangle in sphere " << triangle.a << " " << triangle.b << " " << triangle.c << " has been drawn" << std::endl;
            std::cout << "Vertex Colors sphere: " << vertexColor1 << "  " << vertexColor2 << "  " << vertexColor3 << std::endl;
        }
    }
private:
    int vert_; // Number of vertices vertically
    int horiz_; // Number of vertices horizontally
    float radius_; // Radius of the grid
    //Buffer& buffer;
    //float4x4 transform_;
    unsigned int textureID = 0;

    struct int3 {
        int a, b, c;
    };

    //std::vector<Point> vertices_{};

    void GenerateVertices() {
        vertices_.clear();
        texCoords_.clear(); // Clear texture coordinates
        for (int yy = 0; yy <= horiz_ + 1; ++yy) {
            float y = std::cos(yy * M_PI / (horiz_ + 1.f)); // Calculate y-coordinate
            float r = std::sqrt(1 - y * y); // Calculate radius based on y
            for (int rr = 0; rr < vert_; ++rr) {
                float x = -r * std::cos(2 * M_PI * rr / vert_); // Calculate x-coordinate
                float z = r * std::sin(2 * M_PI * rr / vert_); // Calculate z-coordinate
                Vector3 vertex(x * radius_, y * radius_, z * radius_); // Calculate vertex position
                vertices_.emplace_back(vertex.x, vertex.y, vertex.z); // Add vertex to the vertices vector

                // Calculate vertex normal (pointing outward from the center)
                Vector3 normal = vertex.Normal();
                vertexNormals_.emplace_back(normal.x, normal.y, normal.z); // Add vertex normal to the vertex normals vector
                float u = static_cast<float>(rr) / static_cast<float>(vert_ - 1);
                float v = static_cast<float>(yy) / static_cast<float>(horiz_ + 1); 
                texCoords_.emplace_back(u, v);

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
        CalculateVertexColors(vertices_, vertexColors_);

    }

};
