#include "Rasterizer.h"


Rasterizer::Rasterizer()
{
}

Vector3 interpolateVector(const Vector3& v1, const Vector3& v2, const Vector3& v3,
    float lambda1, float lambda2, float lambda3) {
    return v1 * lambda1 + v2 * lambda2 + v3 * lambda3;
}
Vector3 reflect(const Vector3& incident, const Vector3& normal) {
    return incident - 2 * Vector3::Dot(incident, normal) * normal;
}
static Vector3 ColorFromUnsignedInt(unsigned int color) {
    float red = ((color >> 16) & 0xFF) / 255.0f;
    float green = ((color >> 8) & 0xFF) / 255.0f;
    float blue = (color & 0xFF) / 255.0f;
    return Vector3(red, green, blue);
}
static unsigned int ColorToUnsignedInt(const Vector3& color) {
    return 0xFF000000 | // Alpha channel (fully opaque)
        ((static_cast<unsigned int>(color.x * 255.0f) & 0xFF) << 16) | // Red channel
        ((static_cast<unsigned int>(color.y * 255.0f) & 0xFF) << 8) | // Green channel
        (static_cast<unsigned int>(color.z * 255.0f) & 0xFF);          // Blue channel
}
void Rasterizer::DrawTriangle(Buffer& buffer, const std::vector<Point>& vertices, unsigned int color1, unsigned int color2, unsigned int color3, bool isGouraud) {
    // Compute bounding box
    int minX = buffer.getWidth() - 1;
    int maxX = 0;
    int minY = buffer.getHeight() - 1;
    int maxY = 0;

    std::vector<Point> transformedVertices;
    transformedVertices.reserve(vertices.size());
    for (const auto& vertex : vertices) {
        float x = (vertex.x + 1.0f) * (buffer.getWidth() - 1) / 2.0f;
        float y = (1.0f - vertex.y) * (buffer.getHeight() - 1) / 2.0f;

        transformedVertices.emplace_back(x, y, vertex.z);

        minX = std::min(minX, static_cast<int>(x));
        maxX = std::max(maxX, static_cast<int>(x));
        minY = std::min(minY, static_cast<int>(y));
        maxY = std::max(maxY, static_cast<int>(y));
    }

    // Clamp bounding box to screen bounds
    minX = std::max(minX, 0);
    maxX = std::min(maxX, buffer.getWidth() - 1);
    minY = std::max(minY, 0);
    maxY = std::min(maxY, buffer.getHeight() - 1);

    float dx12 = transformedVertices[0].x - transformedVertices[1].x;
    float dx23 = transformedVertices[1].x - transformedVertices[2].x;
    float dx32 = transformedVertices[2].x - transformedVertices[1].x;
    float dx31 = transformedVertices[2].x - transformedVertices[0].x;
    float dx13 = transformedVertices[0].x - transformedVertices[2].x;

    float dy12 = transformedVertices[0].y - transformedVertices[1].y;
    float dy23 = transformedVertices[1].y - transformedVertices[2].y;
    float dy31 = transformedVertices[2].y - transformedVertices[0].y;
    float dy13 = transformedVertices[0].y - transformedVertices[2].y;
    float denom = dy23 * dx13 - dx23 * dy13;

    // Define the diffuse color variable
    Vector3 viewDirection(0.0f, 0.f, -1.0f); // Direction of the view (camera)


    std::vector<Vector3> normals(3);
    for (int i = 0; i < 3; ++i) {
        Vector3 v1 = Vector3(vertices[(i + 1) % 3].x, vertices[(i + 1) % 3].y, vertices[(i + 1) % 3].z) -
            Vector3(vertices[i].x, vertices[i].y, vertices[i].z);
        Vector3 v2 = Vector3(vertices[(i + 2) % 3].x, vertices[(i + 2) % 3].y, vertices[(i + 2) % 3].z) -
            Vector3(vertices[i].x, vertices[i].y, vertices[i].z);
        normals[i] = v1.Cross(v2).Normal();
    }

    // Fill the triangle by iterating over bounding box
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float lambda1 = ((dy23 * (x - transformedVertices[2].x) + dx32 * (y - transformedVertices[2].y)) / denom);
            float lambda2 = ((dy31 * (x - transformedVertices[2].x) + dx13 * (y - transformedVertices[2].y)) / denom);
            float lambda3 = 1 - lambda1 - lambda2;
            unsigned int interpolatedColor = interpolateColor(color1, color2, color3, lambda1, lambda2, lambda3);
            unsigned int finalColorInt;
            float depth = lambda1 * vertices[0].z + lambda2 * vertices[1].z + lambda3 * vertices[2].z;
            Vector3 finalColor = Vector3(0.f, 0.f, 0.f);

            Vector3 pixelPosition(x, y, depth); // Assuming you calculate the depth

            // Check if the point is inside the triangle
            if (lambda1 >= 0 && lambda1 <= 1 && lambda2 >= 0 && lambda2 <= 1 && lambda3 >= 0 && lambda3 <= 1) {
                Vector3 pixelPosition;

                if (!isGouraud) {

                    Vector3 interpolatedNormal = interpolateVector(normals[0], normals[1], normals[2], lambda1, lambda2, lambda3).Normal();
                    
                    Vector3 pixelColor(0.0f, 0.0f, 0.0f);

                    // Calculate lighting for each light source
                    for (const auto& light : buffer.lights) {
                        Vector3 lightColor = light->color;

                        // Choose the appropriate lighting calculation based on light type
                        Vector3 lighting;
                        switch (light->type) {
                        case LightType::Directional:
                            lighting = buffer.calculateDirectionalLighting(interpolatedNormal, viewDirection, *static_cast<DirectionalLight*>(light));
                            break;
                        case LightType::Point:
                            lighting = buffer.calculatePointLighting(pixelPosition, interpolatedNormal, viewDirection, *static_cast<PointLight*>(light));
                            break;
                        case LightType::Spotlight:
                            lighting = buffer.calculateSpotlightLighting(pixelPosition, interpolatedNormal, viewDirection, *static_cast<Spotlight*>(light));
                            break;
                        default:
                            break;
                        }

                        // Accumulate lighting contributions from all light sources
                        pixelColor += lighting.Mul(lightColor);
                    }

                    // Clamp pixel color to [0, 1] range
                    pixelColor = pixelColor.Clamp();

                    // Convert pixel color back to unsigned int
                    finalColorInt = ColorToUnsignedInt(pixelColor);

                }
                else {
                    //Gouraud shading - works
                    finalColorInt = interpolatedColor;
                }
                // Draw the pixel


                if (depth < buffer.depth[y * buffer.getWidth() + x])
                {
                    buffer.color[y * buffer.getWidth() + x] = finalColorInt;
                    buffer.depth[y * buffer.getWidth() + x] = depth;
                }
            }
        }
    }
}

unsigned int Rasterizer::interpolateColor(unsigned int color1, unsigned int color2, unsigned int color3, float lambda1, float lambda2, float lambda3) {
    // Extract individual color components
    unsigned char alpha1 = (color1 >> 24) & 0xFF;
    unsigned char red1 = (color1 >> 16) & 0xFF;
    unsigned char green1 = (color1 >> 8) & 0xFF;
    unsigned char blue1 = color1 & 0xFF;

    unsigned char alpha2 = (color2 >> 24) & 0xFF;
    unsigned char red2 = (color2 >> 16) & 0xFF;
    unsigned char green2 = (color2 >> 8) & 0xFF;
    unsigned char blue2 = color2 & 0xFF;

    unsigned char alpha3 = (color3 >> 24) & 0xFF;
    unsigned char red3 = (color3 >> 16) & 0xFF;
    unsigned char green3 = (color3 >> 8) & 0xFF;
    unsigned char blue3 = color3 & 0xFF;

    // Interpolate each color component separately
    unsigned char interpolatedAlpha = static_cast<unsigned char>(lambda1 * alpha1 + lambda2 * alpha2 + lambda3 * alpha3);
    unsigned char interpolatedRed = static_cast<unsigned char>(lambda1 * red1 + lambda2 * red2 + lambda3 * red3);
    unsigned char interpolatedGreen = static_cast<unsigned char>(lambda1 * green1 + lambda2 * green2 + lambda3 * green3);
    unsigned char interpolatedBlue = static_cast<unsigned char>(lambda1 * blue1 + lambda2 * blue2 + lambda3 * blue3);

    // Combine interpolated color components into a single color value
    unsigned int interpolatedColor = (interpolatedAlpha << 24) | (interpolatedRed << 16) | (interpolatedGreen << 8) | interpolatedBlue;

    return interpolatedColor;
}
//// Calculate the interpolated position using barycentric coordinates
//pixelPosition.x = lambda1 * vertices[0].x + lambda2 * vertices[1].x + lambda3 * vertices[2].x;
//pixelPosition.y = lambda1 * vertices[0].y + lambda2 * vertices[1].y + lambda3 * vertices[2].y;
//pixelPosition.z = lambda1 * vertices[0].z + lambda2 * vertices[1].z + lambda3 * vertices[2].z;
//
//Vector3 interpolatedNormal;
//interpolatedNormal.x = lambda1 * transformedVertices[0].x + lambda2 * transformedVertices[1].x + lambda3 * transformedVertices[2].x;
//interpolatedNormal.y = lambda1 * transformedVertices[0].y + lambda2 * transformedVertices[1].y + lambda3 * transformedVertices[2].y;
//interpolatedNormal.z = lambda1 * transformedVertices[0].z + lambda2 * transformedVertices[1].z + lambda3 * transformedVertices[2].z;
//interpolatedNormal.Normal();
//// Interpolate colors
//unsigned int interpolatedColor = interpolateColor(color1, color2, color3, lambda1, lambda2, lambda3);
//unsigned int finalColorInt;
//Vector3 finalColor = Vector3(0.f, 0.f, 0.f);
//for (const auto& light : buffer.lights) {
//    switch (light->type) {
//    case LightType::Directional:
//        finalColor += buffer.calculateDirectionalLighting(interpolatedNormal, viewDirection, *static_cast<DirectionalLight*>(light));
//        break;
//    case LightType::Point:
//        finalColor += buffer.calculatePointLighting(pixelPosition, interpolatedNormal, viewDirection, *static_cast<PointLight*>(light));
//        //std::cout << buffer.calculatePointLighting(pixelPosition, interpolatedNormal, viewDirection, *static_cast<PointLight*>(light)) << std::endl;
//        break;
//    case LightType::Spotlight:
//        finalColor += buffer.calculateSpotlightLighting(pixelPosition, interpolatedNormal, viewDirection, *static_cast<Spotlight*>(light));
//        break;
//    }
//    //finalColor.x = std::clamp(finalColor.x, 0.0f, 1.0f);
//    //finalColor.y = std::clamp(finalColor.y, 0.0f, 1.0f);
//    //finalColor.z = std::clamp(finalColor.z, 0.0f, 1.0f);
//}
//
//interpolatedColor = interpolateColor(color1, color2, color3, lambda1, lambda2, lambda3);
//// Combine finalColor components with interpolatedColor
//Vector3 colorVector = ColorFromUnsignedInt(interpolatedColor);
//finalColor.x += colorVector.x;
//finalColor.y += colorVector.y;
//finalColor.z += colorVector.z;
//
//// Clamp finalColor components to [0, 1]
//finalColor.x = std::clamp(finalColor.x, 0.0f, 1.0f);
//finalColor.y = std::clamp(finalColor.y, 0.0f, 1.0f);
//finalColor.z = std::clamp(finalColor.z, 0.0f, 1.0f);
//
//// Convert finalColor to unsigned int color value
////unsigned int finalColorInt = static_cast<unsigned int>(255.0f * finalColor.x) << 16 |
////    static_cast<unsigned int>(255.0f * finalColor.y) << 8 |
////    static_cast<unsigned int>(255.0f * finalColor.z);
//
//
//finalColorInt = 0xFF000000 | // Alpha channel (fully opaque)
//((static_cast<unsigned int>(finalColor.x * 255.0f) & 0xFF) << 16) | // Red channel
//((static_cast<unsigned int>(finalColor.y * 255.0f) & 0xFF) << 8) | // Green channel
//(static_cast<unsigned int>(finalColor.z * 255.0f) & 0xFF); // Blue channel



/*


    // Fill the triangle by iterating over bounding box
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float lambda1 = ((dy23 * (x - transformedVertices[2].x) + dx32 * (y - transformedVertices[2].y)) / denom);
            float lambda2 = ((dy31 * (x - transformedVertices[2].x) + dx13 * (y - transformedVertices[2].y)) / denom);
            float lambda3 = 1 - lambda1 - lambda2;

            // Check if the point is inside the triangle
            if (lambda1 >= 0 && lambda1 <= 1 && lambda2 >= 0 && lambda2 <= 1 && lambda3 >= 0 && lambda3 <= 1) {
                Vector3 pixelPosition;
                // Calculate the interpolated position using barycentric coordinates
                pixelPosition.x = lambda1 * vertices[0].x + lambda2 * vertices[1].x + lambda3 * vertices[2].x;
                pixelPosition.y = lambda1 * vertices[0].y + lambda2 * vertices[1].y + lambda3 * vertices[2].y;
                pixelPosition.z = lambda1 * vertices[0].z + lambda2 * vertices[1].z + lambda3 * vertices[2].z;

                Vector3 interpolatedNormal;
                interpolatedNormal.x = lambda1 * transformedVertices[0].x + lambda2 * transformedVertices[1].x + lambda3 * transformedVertices[2].x;
                interpolatedNormal.y = lambda1 * transformedVertices[0].y + lambda2 * transformedVertices[1].y + lambda3 * transformedVertices[2].y;
                interpolatedNormal.z = lambda1 * transformedVertices[0].z + lambda2 * transformedVertices[1].z + lambda3 * transformedVertices[2].z;
                interpolatedNormal.Normal();
                // Interpolate colors
                unsigned int interpolatedColor = interpolateColor(color1, color2, color3, lambda1, lambda2, lambda3);
                unsigned int finalColorInt;
                Vector3 finalColor = Vector3(0.f, 0.f, 0.f);

                if (!isGouraud) {

                    for (const auto& light : buffer.lights) {
                        switch (light->type) {
                        case LightType::Directional:
                            finalColor += buffer.calculateDirectionalLighting(interpolatedNormal, viewDirection, *static_cast<DirectionalLight*>(light));
                            break;
                        case LightType::Point:
                            finalColor += buffer.calculatePointLighting(pixelPosition, interpolatedNormal, viewDirection, *static_cast<PointLight*>(light));
                            //std::cout << buffer.calculatePointLighting(pixelPosition, interpolatedNormal, viewDirection, *static_cast<PointLight*>(light)) << std::endl;
                            break;
                        case LightType::Spotlight:
                            finalColor += buffer.calculateSpotlightLighting(pixelPosition, interpolatedNormal, viewDirection, *static_cast<Spotlight*>(light));
                            break;
                        }
                        //finalColor.x = std::clamp(finalColor.x, 0.0f, 1.0f);
                        //finalColor.y = std::clamp(finalColor.y, 0.0f, 1.0f);
                        //finalColor.z = std::clamp(finalColor.z, 0.0f, 1.0f);
                    }

                    interpolatedColor = interpolateColor(color1, color2, color3, lambda1, lambda2, lambda3);
                    // Combine finalColor components with interpolatedColor
                    Vector3 colorVector = ColorFromUnsignedInt(interpolatedColor);
                    finalColor.x += colorVector.x;
                    finalColor.y += colorVector.y;
                    finalColor.z += colorVector.z;

                    // Clamp finalColor components to [0, 1]
                    finalColor.x = std::clamp(finalColor.x, 0.0f, 1.0f);
                    finalColor.y = std::clamp(finalColor.y, 0.0f, 1.0f);
                    finalColor.z = std::clamp(finalColor.z, 0.0f, 1.0f);

                    // Convert finalColor to unsigned int color value
                    //unsigned int finalColorInt = static_cast<unsigned int>(255.0f * finalColor.x) << 16 |
                    //    static_cast<unsigned int>(255.0f * finalColor.y) << 8 |
                    //    static_cast<unsigned int>(255.0f * finalColor.z);


                    finalColorInt = 0xFF000000 | // Alpha channel (fully opaque)
                        ((static_cast<unsigned int>(finalColor.x * 255.0f) & 0xFF) << 16) | // Red channel
                        ((static_cast<unsigned int>(finalColor.y * 255.0f) & 0xFF) << 8) | // Green channel
                        (static_cast<unsigned int>(finalColor.z * 255.0f) & 0xFF); // Blue channel

                }
                else {
                    finalColorInt = interpolatedColor;
                }
                // Draw the pixel


                float depth = lambda1 * vertices[0].z + lambda2 * vertices[1].z + lambda3 * vertices[2].z;
                if (depth < buffer.depth[y * buffer.getWidth() + x])
                {
                    buffer.color[y * buffer.getWidth() + x] = finalColorInt;
                    buffer.depth[y * buffer.getWidth() + x] = depth;
                }
            }
        }
    }
    */