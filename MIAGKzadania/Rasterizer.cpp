#include "Rasterizer.h"


Rasterizer::Rasterizer()
{
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
void Rasterizer::DrawTriangle(Buffer& buffer, const std::vector<Point>& vertices, const std::vector<Vector3>& normals, unsigned int color1, unsigned int color2, unsigned int color3, const std::vector<Vector2>& texCoords, unsigned int textureID, bool isGouraud, bool lightOn) {
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
    Vector3 viewDirection(0.0f, 0.f, 1.0f); // Direction of the view (camera)


    // Fill the triangle by iterating over bounding box
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float lambda1 = ((dy23 * (x - transformedVertices[2].x) + dx32 * (y - transformedVertices[2].y)) / denom);
            float lambda2 = ((dy31 * (x - transformedVertices[2].x) + dx13 * (y - transformedVertices[2].y)) / denom);
            float lambda3 = 1 - lambda1 - lambda2;

            unsigned int interpolatedColor = interpolateColor(color1, color2, color3, lambda1, lambda2, lambda3);
            unsigned int finalColorInt;
            float depth = lambda1 * vertices[0].z + lambda2 * vertices[1].z + lambda3 * vertices[2].z;
            Vector2 interpolatedTexCoord = texCoords[0] * lambda1  + texCoords[1] * lambda2 + texCoords[2] * lambda3;
            Vector3 texColor = SampleTexture(buffer, interpolatedTexCoord, textureID);

            Vector3 finalColor = Vector3(0.f, 0.f, 0.f);
            // Check if the point is inside the triangle
            if (lambda1 >= 0 && lambda1 <= 1 && lambda2 >= 0 && lambda2 <= 1 && lambda3 >= 0 && lambda3 <= 1) {
                //pixelPosition = Vector3(vertices[0].x, vertices[0].y, vertices[0].z);
                if (!isGouraud) {
                    Vector3 interpolatedNormal;

                    interpolatedNormal.x = lambda1 * normals[0].x + lambda2 * normals[1].x + lambda3 * normals[2].x;
                    interpolatedNormal.y = lambda1 * normals[0].y + lambda2 * normals[1].y + lambda3 * normals[2].y;
                    interpolatedNormal.z = lambda1 * normals[0].z + lambda2 * normals[1].z + lambda3 * normals[2].z;

                    Vector3 pixelColor(0.f, 0.f, 0.f);
                    Vector3 pixelPosition(x, y, depth);
                    //Vector3 colorVector = ColorFromUnsignedInt(interpolatedColor);
                    //pixelColor.x += colorVector.x;
                    //pixelColor.y += colorVector.y;
                    //pixelColor.z += colorVector.z;
                    if (lightOn) {
                        pixelColor = Vector3(0.2f, 0.2f, 0.2f);

                        // Calculate lighting for each light source
                        for (const auto& light : buffer.lights) {
                            Vector3 lightColor = light->color;

                            // Choose the appropriate lighting calculation based on light type
                            Vector3 lighting;
                            switch (light->type) {
                            case LightType::Directional:
                                lighting = buffer.calculateDirectionalLighting(interpolatedNormal, interpolatedNormal.Normal(), *static_cast<DirectionalLight*>(light)).Mul(lightColor);
                                break;
                            case LightType::Point:
                                lighting = buffer.calculatePointLighting(interpolatedNormal, interpolatedNormal.Normal(), viewDirection, *static_cast<PointLight*>(light)).Mul(lightColor);
                                break;
                            case LightType::Spotlight:
                                lighting = buffer.calculateSpotlightLighting(interpolatedNormal, interpolatedNormal.Normal(), viewDirection, *static_cast<Spotlight*>(light)).Mul(lightColor);
                                break;
                            default:
                                break;
                            }

                            // Accumulate lighting contributions from all light sources
                            //pixelColor += lighting;
                            pixelColor.x += lighting.x;
                            pixelColor.y += lighting.y;
                            pixelColor.z += lighting.z;
                        }
                    }
                    else {
                        pixelColor = Vector3(.2f, .2f, .2f);
                    }
                    // Clamp pixel color to [0, 1] range
                    pixelColor.x *= texColor.x;
                    pixelColor.y *= texColor.y;
                    pixelColor.z *= texColor.z;
                    //pixelColor = pixelColor.Clamp();
                    pixelColor.x = std::clamp(pixelColor.x, 0.0f, 1.0f);
                    pixelColor.y = std::clamp(pixelColor.y, 0.0f, 1.0f);
                    pixelColor.z = std::clamp(pixelColor.z, 0.0f, 1.0f);
 
                    // Convert pixel color back to unsigned int
                    //finalColorInt = ColorToUnsignedInt(texColor);
                    finalColorInt = ColorToUnsignedInt(pixelColor);

                }
                else {
                    //Gouraud shading - works
                    finalColorInt = interpolatedColor;
                }

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
Vector3 Rasterizer::SampleTexture(Buffer& buffer, const Vector2& texCoord, unsigned int textureID) {
    // Check if texture ID is valid
    if (textureID >= buffer.textures.size()) {
        std::cerr << "Invalid texture ID: " << textureID << std::endl;
        return Vector3(0.0f, 0.0f, 0.0f); // Return black color as fallback
    }

    int texWidth = buffer.textureWidths[textureID];
    int texHeight = buffer.textureHeights[textureID];
    int texChannels = buffer.textureChannels[textureID];
    unsigned char* texData = buffer.textures[textureID];

    // Check if texture channels is valid (e.g., 3 for RGB or 4 for RGBA)
    if (texChannels != 3 && texChannels != 4) {
        std::cerr << "Invalid number of texture channels: " << texChannels << std::endl;
        return Vector3(0.0f, 0.0f, 0.0f); // Return black color as fallback
    }

    // Calculate texture coordinates
    float u = texCoord.x * texWidth;
    float v = texCoord.y * texHeight;

    // Clamp texture coordinates to valid range
    u = std::clamp(u, 0.0f, static_cast<float>(texWidth - 1));
    v = std::clamp(v, 0.0f, static_cast<float>(texHeight - 1));

    // Calculate index into texture data
    int texIndex = (static_cast<int>(v) * texWidth + static_cast<int>(u)) * texChannels;

    // Extract color channels from texture data and normalize to [0, 1] range
    //float r = texData[texIndex] / 255.0f;
    //float g = texData[texIndex + 1] / 255.0f;
    //float b = texData[texIndex + 2] / 255.0f;

    // Bilinear filtering
    int x = static_cast<int>(u);
    int y = static_cast<int>(v);
    float u_ratio = u - x;
    float v_ratio = v - y;
    float u_opposite = 1 - u_ratio;
    float v_opposite = 1 - v_ratio;

    // Get the four surrounding pixels
    const unsigned char* p0 = texData + ((y * texWidth + x) * texChannels);
    const unsigned char* p1 = texData + ((y * texWidth + (x + 1)) * texChannels);
    const unsigned char* p2 = texData + (((y + 1) * texWidth + x) * texChannels);
    const unsigned char* p3 = texData + (((y + 1) * texWidth + (x + 1)) * texChannels);

    // Calculate the interpolated color
    float r = (p0[0] * u_opposite + p1[0] * u_ratio) * v_opposite + (p2[0] * u_opposite + p3[0] * u_ratio) * v_ratio;
    float g = (p0[1] * u_opposite + p1[1] * u_ratio) * v_opposite + (p2[1] * u_opposite + p3[1] * u_ratio) * v_ratio;
    float b = (p0[2] * u_opposite + p1[2] * u_ratio) * v_opposite + (p2[2] * u_opposite + p3[2] * u_ratio) * v_ratio;

    // Normalize the color values
    return Vector3(r / 255.0f, g / 255.0f, b / 255.0f);
}
