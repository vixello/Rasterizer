#include "Rasterizer.h"
#include "SphereMesh.h"
#include "ConeMesh.h"
#include "TorusMesh.h"

#include "Camera.h"

int main() {
    int width = 1400;
    int height = 1400;

    //color buffer
	Buffer buffer(width, height);

    unsigned short header[9] = {
        0x0000, 0x0002, 0x0000, 0x0000, 0x0000, 0x0000,
        0x01000, 0x0100, // width height
        0x0820
    };
    buffer.SetSize(width, height);
    buffer.ClearColor(0xFF005090);
    buffer.ClearDepth(100.0f);

    float4x4 worldTransform = Camera::Translation(0.f, .2f, .5f) * Camera::Scale(1.f, 1.f, 1.f) * (Camera::RotationX(0) * Camera::RotationY(0) * Camera::RotationZ(0));
    float4x4 viewTransform = Camera::LookAt({ 0.f, 0.f, 1.f }, { 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f });
    float4x4 projectionTransform = Camera::Perspective(Camera::ToRad(45.0f), 1.0f, 0.01f, 100.0f);


    // Sphere parameters
    int vert = 10; // Number of vertices vertically
    int horiz = 10; // Number of vertices horizontally
    float radius = .2f; // Radius of the sphere

    // Cone parameters
    int segments = 10;
    float coneHeight = .4f; // Height of the cone

    int radialSegments = 5; // Liczba segmentów wokół osi centralnej torusa. Zwiększenie tej wartości spowoduje uzyskanie gładziej powierzchni torusa.
    int tubeSegments = 5; // Liczba segmentów wokół grubości torusa.Zwiększenie tej wartości spowoduje uzyskanie bardziej szczegółowego kształtu rury.
    float torusRadius = .3f; // Radius of the grid
    float torusTubeRadius = .1f; // Radius of the grid

    Rasterizer r;

    // - jest blizenj, + dalej na osi z
    // Sphere
    float4x4 sphereTransform = Camera::Translation(0.f, .2f, .5f) * Camera::Scale(1.f, 1.f, 1.f) * Camera::RotationX(90);
    SphereMesh sphereGrid(vert, horiz, radius, buffer, projectionTransform * viewTransform * sphereTransform);

    // Cone
    float4x4 coneTransform = Camera::Translation(.2f, -0.05f, 1.f) * Camera::Scale(1.f, 1.f, 1.f) * Camera::RotationX(30);
    ConeMesh coneGrid(segments, radius, coneHeight, buffer, projectionTransform * viewTransform * coneTransform);

    // Torus
    float4x4 torusTransform = Camera::Translation(-.2f, -0.1f, 1.f) * Camera::Scale(.4f, 0.4f, .4f) * Camera::RotationX(40);
    TorusMesh torusGrid(radialSegments, tubeSegments, torusRadius, torusTubeRadius, buffer, projectionTransform * viewTransform * torusTransform);

    // Generate and draw meshes
    sphereGrid.Generate();
    //sphereGrid.Draw(r, 0xFFA63446, 0xFF000004, 0xFFFF00FF);
    sphereGrid.Draw(r, 0xFFA63446, 0xFFA63446, 0xFFA63446);
    coneGrid.Generate();
    //coneGrid.Draw(r, 0xFFB4A0E5, 0xFFEC9386, 0xFFF564A9);
    coneGrid.Draw(r, 0xFFB4A0E5, 0xFFB4A0E5, 0xFFB4A0E5);
    torusGrid.Generate();
    //torusGrid.Draw(r, 0xFFE6E6FA, 0xFF98FB98, 0xFF441151);
    torusGrid.Draw(r, 0xFFE6E6FA, 0xFFE6E6FA, 0xFFE6E6FA);
    /// Colors
    //    Color 1: Blue(0xFF0000FF)
    //    Color 2 : Cyan(0xFF00FFFF)
    //    Color 3 : Green(0xFF00FF00)
    //    Color 4 : Pink(0xFFFFC0CB)
    //    Color 5 : Lavender(0xFFE6E6FA)
    //    Color 6 : Mint(0xFF98FB98)
    //    Color 7 : Magenta(0xFFFF00FF)
    //    Color 8 : Lime(0xFF00FF00)
    //    Color 9 : Turquoise(0xFF40E0D0)
    //    Color 10: off-white shade (0xFFFAF3DD)
    //    Color 11: rich black (0xFF0B2027) 
    //    Color 12: wisteria black (0xFFB4A0E5) 
    //    Color 13: Hot pink (0xFFF564A9) 
    //    Color 14: Russian violet (0xFF441151) 
    //    Color 15: coral pink (0xFFEC9386) 
    //    Color 16: polynesian blue (0xFF005090) 

    //Camera::vertexShader(vertices, transform);
    //Camera::vertexShader(vertices2, transform);
    //Camera::vertexShader(vertices3, transform);
    //Camera::vertexShader(vertices4, transform);
    //
    //Rasterizer::DrawTriangle(buffer, vertices, 0xFFFAF3DD, 0xFF00FFFF, 0xFF98FB98);
    //Rasterizer::DrawTriangle(buffer, vertices2, 0xFFA63446, 0xFF000004, 0xFFFF00FF);
    //Rasterizer::DrawTriangle(buffer, vertices3, 0xFFB4A0E5, 0xFFEC9386, 0xFFF564A9);
    //Rasterizer::DrawTriangle(buffer, vertices4, 0xFF00FF00, 0xFFE6E6FA, 0xFFF564A9);
    buffer.SaveToTGA("obrazek1.tga", header);

    return 0;

}