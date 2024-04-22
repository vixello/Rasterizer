﻿
#include "Rasterizer.h"
#include "float4x4.h"
#include "camera.h"
#include "SphereMesh.h"
#include "ConeMesh.h"
#include "TorusMesh.h"


int main() {
    int width = 1400;
    int height = 1400;
    std::string texturePath = "earth.tga"; // Path to your PNG file
    std::string texturePath2 = "Brick.tga"; // Path to your PNG file

    //color buffer
    Buffer buffer(width, height);

    unsigned short header[9] = {
        0x0000, 0x0002, 0x0000, 0x0000, 0x0000, 0x0000,
        0x01000, 0x0100, // width height
        0x0820
    };
    buffer.SetSize(width, height);
    buffer.ClearColor(0xFF004277);
    buffer.ClearDepth(100.0f);
    buffer.loadTexture(texturePath);
    buffer.loadTexture(texturePath2);
    //DirectionalLight directionalLight({ 0.0f, -100.0f, -1.0f }, { -10.0f,8.0f, 10.0f }, { .1f, .1f, .1f }, { 0.1f, 0.1f, 0.1f }, { .1f, .1f, .1f }, 2.0f, { 1.0f, 1.0f, 1.0f });
// Directional light parameters:
    DirectionalLight directionalLight(
        { 0.f, 0.f, -1.f },        // Direction vector (normalized)
        { 0.f, -8.f, 4.f },         // Position (optional, for visualization)
        { .1f, .1f, .1f },          // Ambient color
        { .1f, .1f, .1f },          // Diffuse color
        { .1f, .1f, .1f },          // Specular color
        20.f,                        // Shininess (for Phong shading)
        1.9f,                        // Intensity
        { 1.f, 1.f, 1.f }           // Color (in RGB format)
    );

    // Point light parameters:
    PointLight pointLight(
        { 2.f, -2.f, 4.f },         // Position
        { .2f, .2f, .2f },          // Ambient color
        { .4f, .4f, .4f },              // Diffuse color
        { 2.f, 2.f, 2.f },          // Specular color
        7.5f,                      // Shininess
        2.f,                        // Intensity
        { .0f, .3f, 1.f }           // Color
        //{ 1.f, 0.2f, 0.f }           // Color
    );

    // Spotlight parameters:
    Spotlight spotlight(
        { -10.f, 2.f, 10.f },          // Position
        { 0.f, 5.f, 1.f },          // Direction vector (normalized)
        50.f,                       // Angle (in degrees)
        { .7f, .7f, .7f },          // Ambient color
        { .7f, .7f, .7f },          // Diffuse color
        { .7f, .7f, .7f },          // Specular color
        .7f,                       // Shininess
        { 0.f, 1.f, 0.f }           // Color
    );

    float4x4 worldTransform = Camera::Translation(0.f, .2f, .5f) * Camera::Scale(1.f, 1.f, 1.f) * (Camera::RotationX(0) * Camera::RotationY(0) * Camera::RotationZ(0));
    float4x4 viewTransform = Camera::LookAt({ 0.f, 0.f, 1.f }, { 0., 0., 0. }, { 0., 1., 0. });
    float4x4 projectionTransform = Camera::Perspective(Camera::ToRad(45.0f), 1.0f, 0.01f, 100.0f);


    // Sphere parameters
    int vert = 20; // Number of vertices vertically
    int horiz = 20; // Number of vertices horizontally
    float radius = .2f; // Radius of the sphere

    // Cone parameters
    int segments = 20;
    float coneHeight = .4f; // Height of the cone

    int radialSegments = 20; 
    int tubeSegments = 20; 
    float torusRadius = .3f; 
    float torusTubeRadius = .1f; 

    Rasterizer r;
    // - jest blizenj, + dalej na osi z
    // Sphere
    float4x4 sphereTransform = Camera::Translation(0.2f, .2f, 1.f) * Camera::Scale(1.f, 1.f, 1.f);
    SphereMesh sphereGrid(vert, horiz, radius, buffer, projectionTransform  * viewTransform * sphereTransform);

    sphereTransform = Camera::Translation(-0.2f, .2f, 1.f) * Camera::Scale(1.f, 1.f, 1.f) * Camera::RotationX(0);
    SphereMesh sphereGridGouraud(vert, horiz, radius, buffer, projectionTransform * viewTransform * sphereTransform);

    // Cone
    float4x4 coneTransform = Camera::Translation(.2f, -0.05f, 1.f) * Camera::Scale(1.f, 1.f, 1.f) * Camera::RotationX(5) ;
    ConeMesh coneGrid(segments, radius, coneHeight, buffer, projectionTransform * viewTransform * coneTransform);

    // Torus
    float4x4 torusTransform = Camera::Translation(0.f, -0.2f, 0.f) * Camera::Scale(.4f, 0.4f, .4f) * Camera::RotationX(50) * Camera::RotationY(-20);
    TorusMesh torusGrid(radialSegments, tubeSegments, torusRadius, torusTubeRadius, buffer, projectionTransform * viewTransform * torusTransform);

    torusTransform = Camera::Translation(-.2f, -0.2f, 0.f) * Camera::Scale(.4f, 0.4f, .4f) * Camera::RotationX(50) * Camera::RotationY(-20);
    TorusMesh torusGridGouraud(radialSegments, tubeSegments, torusRadius, torusTubeRadius, buffer, projectionTransform * viewTransform * torusTransform);

    // Add lights to the buffer
    buffer.lights.push_back(&directionalLight);
    buffer.lights.push_back(&pointLight);
    //std::cout << pointLight.position << std::endl << std::endl << std::endl << std::endl << std::endl;
    buffer.lights.push_back(&spotlight);


    // Generate and draw meshes
    sphereGrid.Generate();
    //sphereGrid.Draw(r, 0xFFA63446, 0xFF000004, 0xFFFF00FF);
    sphereGrid.Draw(r, 0, true, 0xFFA63446, 0xFFA63446, 0xFFA63446);

    sphereGridGouraud.Generate();
    sphereGridGouraud.Draw(r, 1, true, 0xFFA63446, 0xFFA63446, 0xFFA63446);

    //coneGrid.Generate();
    //coneGrid.Draw(r, 0xFFB4A0E5, 0xFFEC9386, 0xFFF564A9);
    //coneGrid.DrawGouraud(r, 0xFFF564A9, 0xFFF564A9, 0xFFF564A9);
    torusGrid.Generate();
    torusGridGouraud.Generate();
    //torusGridGouraud.Draw(r, 0xFFE6E6FA, 0xFF98FB98, 0xFF441151);
    torusGrid.Draw(r, 0, false, 0xFF441151, 0xFF441151, 0xFF441151);
    //torusGridGouraud.DrawGouraud(r, 0xFF441151, 0xFF441151, 0xFF441151);
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

    buffer.SaveToTGA("obrazek1.tga", header);

    return 0;

}