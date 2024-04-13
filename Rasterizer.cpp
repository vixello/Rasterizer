#include "Rasterizer.h"


Rasterizer::Rasterizer()
{
}

//void Rasterizer::DrawTriangle(Buffer& buffer, const std::vector<Point>& vertices, unsigned int color1, unsigned int color2, unsigned int color3) {
//    // Find bounding box of the triangle in screen coordinates
//    int minX = 0;
//    int maxX = buffer.getWidth() - 1;
//    int minY = 0;
//    int maxY = buffer.getHeight() - 1;
//
//    // Transform triangle vertices coordinates from canonical to coordinates in rendering window and update bounding box
//    // x' = (x + 1) * width/2
//    // y' = (y + 1) * height/2
//
//    std::vector<Point> transformedVertices;
//    transformedVertices.reserve(vertices.size());
//    for (const auto& vertex : vertices) {
//        float x = (vertex.x + 1.0f) * (buffer.getWidth() - 1) / 2.0f;
//        float y = (1.0f - vertex.y) * (buffer.getHeight() - 1) / 2.0f;
//
//        transformedVertices.emplace_back(x, y, vertex.z);
//
//        // Update bounding box
//        minX = std::min(minX, static_cast<int>(x));
//        maxX = std::max(maxX, static_cast<int>(x));
//        minY = std::min(minY, static_cast<int>(y));
//        maxY = std::max(maxY, static_cast<int>(y));
//    }
//    minX = std::max(minX, 0);
//    maxX = std::min(maxX, buffer.getWidth() - 1);
//    minY = std::max(minY, 0);
//    maxY = std::min(maxY, buffer.getHeight() - 1);
//
//    // Fill the triangle by iterating over bounding box 
//    for (float y = minY; y <= maxY; ++y) {
//        for (float x = minX; x <= maxX; ++x) {
//                
//            // Cases to check if the point is inside triangle
//            
//            //(X1 - X2) · (Y – Y1) – (Y1 – Y2) · (X – X2) > 0
//            float case1 = ((transformedVertices[0].x - transformedVertices[1].x) * (y - transformedVertices[0].y) -
//                (transformedVertices[0].y - transformedVertices[1].y) * (x - transformedVertices[0].x)); 
//            //(X2 - X3) (Y – Y2) – (Y2 – Y3) ·(X – X2) > 0
//            float case2 = ((transformedVertices[1].x - transformedVertices[2].x) * (y - transformedVertices[1].y) -
//                (transformedVertices[1].y - transformedVertices[2].y) * (x - transformedVertices[1].x));
//            //(X3 - X1) ·(Y – Y3) – (Y3 – Y1) ·(X − X3) > 0 
//            float case3 = ((transformedVertices[2].x - transformedVertices[0].x) * (y - transformedVertices[2].y) -
//                (transformedVertices[2].y - transformedVertices[0].y) * (x - transformedVertices[2].x));
//
//            float dx12 = transformedVertices[0].x - transformedVertices[1].x;
//            float dx23 = transformedVertices[1].x - transformedVertices[2].x;
//            float dx32 = transformedVertices[2].x - transformedVertices[1].x;
//            float dx31 = transformedVertices[2].x - transformedVertices[0].x;
//            float dx13 = transformedVertices[0].x - transformedVertices[2].x;
//
//            float dy12 = transformedVertices[0].y - transformedVertices[1].y;
//            float dy23 = transformedVertices[1].y - transformedVertices[2].y;
//            float dy31 = transformedVertices[2].y - transformedVertices[0].y;
//            float dy13 = transformedVertices[0].y - transformedVertices[2].y;
//
//            float lambda1 = (dy23 * (x - transformedVertices[2].x) + dx32 * (y - transformedVertices[2].y)) /
//                (dy23 * dx13 + dx32 * dy13);
//            float lambda2 = (dy31 * (x - transformedVertices[2].x) + dx13 * (y - transformedVertices[2].y)) /
//                (dy31 * dx23 + dx13 * dy23);
//            float lambda3 = 1 - lambda1 - lambda2;
//
//            bool tl1 = false, tl2 = false, tl3 = false;
//
//            // konwencja wypelnienia
//            if (dy12 < 0 || (dy12 == 0 && dx12 > 0)) {
//                tl1 = true;
//            }
//            if (dy23 < 0 || (dy23 == 0 && dx23 > 0)) {
//                tl2 = true;
//            }
//            if (dy31 < 0 || (dy31 == 0 && dx31 > 0)) {
//                tl3 = true;
//            }
//
//            bool topleft1;
//            bool topleft2;
//            bool topleft3;
//
//            // Top-left edge
//            if (tl1) topleft1 = case1 >= 0;
//            else topleft1 = case1 > 0;
//
//            if (tl2) topleft2 = case2 >= 0;
//            else topleft2 = case2 > 0;
//
//            if (tl3) topleft3 = case3 >= 0;
//            else topleft3 = case3 > 0;
//
//
//            auto color = interpolateColor(color1, color2, color3, lambda1, lambda2, lambda3);
//            
//            float depth = lambda1 * vertices[0].z + lambda2 * vertices[1].z + lambda3 * vertices[2].z;
//
//            // If the point is inside the triangle, set its color accordingly to its depth
//            if (topleft1 && topleft2 && topleft3)
//            {
//                // depth buffer
//                if (depth < buffer.depth[y * buffer.getWidth() + x])
//                    {
//                        buffer.color[y * buffer.getWidth() + x] =  color;
//                        buffer.depth[y * buffer.getWidth() + x] = depth;
//                    }
//            }
//        }
//    }
//}

Vector3 calculateNormal(const Point& vertex) {
    // Implement your normal calculation logic here
    return Vector3(0.0f, 0.0f, 1.0f); // Default normal for demonstration
}
Vector3 reflect(const Vector3& incident, const Vector3& normal) {
    return incident - 2 * Vector3::Dot(incident, normal) * normal;
}

void Rasterizer::DrawTriangle(Buffer& buffer, const std::vector<Point>& vertices, unsigned int color1, unsigned int color2, unsigned int color3) {
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

    // Calculate normals for each vertex
    std::vector<Vector3> vertexNormals;
    vertexNormals.reserve(vertices.size());
    for (const auto& vertex : vertices) {
        Vector3 normal = calculateNormal(vertex);
        vertexNormals.push_back(normal);
    }

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

     //Define the diffuse color variable
    Vector3 diffuseColor(0.8, 0.8, 0.8); // Example diffuse color (light gray)

    // Define the light position variable
    Vector3 lightPosition(10.0, 5.0, 20.0); // Example light position (x=10, y=5, z=20)

    // Define the reflect vector variable
    Vector3 pixelPosition; // Define the position of the pixel
    float shininess = 32.0f; // Shininess factor for specular highlights
    Vector3 specularColor(1.0f, 1.0f, 1.0f); // Specular color of the material
    Vector3 viewDirection(0.0f, 0.0f, 1.0f); // Direction of the view (camera)

    // Fill the triangle by iterating over bounding box 
    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            float lambda1 = ((dy23 * (x - transformedVertices[2].x) + dx32 * (y - transformedVertices[2].y)) / denom);
            float lambda2 = ((dy31 * (x - transformedVertices[2].x) + dx13 * (y - transformedVertices[2].y)) / denom);
            float lambda3 = 1 - lambda1 - lambda2;

            // Check if the point is inside the triangle
            if (lambda1 >= 0 && lambda1 <= 1 && lambda2 >= 0 && lambda2 <= 1 && lambda3 >= 0 && lambda3 <= 1) {
                
                // The point is inside the triangle, interpolate color and depth
                unsigned int color = interpolateColor(color1, color2, color3, lambda1, lambda2, lambda3);

                Vector3 interpolatedNormal;
                interpolatedNormal.x = lambda1 * vertexNormals[0].x + lambda2 * vertexNormals[1].x + lambda3 * vertexNormals[2].x;
                interpolatedNormal.y = lambda1 * vertexNormals[0].y + lambda2 * vertexNormals[1].y + lambda3 * vertexNormals[2].y;
                interpolatedNormal.z = lambda1 * vertexNormals[0].z + lambda2 * vertexNormals[1].z + lambda3 * vertexNormals[2].z;
                
                Vector3 lightDirection = (lightPosition - pixelPosition).Normal();
                float diffuseIntensity = Vector3::Dot(interpolatedNormal, lightDirection);
                float ambientIntensity = 0.1f; // Some ambient light intensity
                float specularIntensity = pow(Vector3::Dot(reflect(-lightDirection, interpolatedNormal), viewDirection), shininess);
                
                // Combine lighting with the color
                unsigned int finalColor = interpolateColor(color1, color2, color3, lambda1, lambda2, lambda3);
                //finalColor *= ambientIntensity + diffuseIntensity * diffuseColor + specularIntensity * specularColor;
                Vector3 ambientComponent = ambientIntensity * diffuseColor;
                
                // Perform component-wise multiplication between diffuse intensity and diffuse color
                Vector3 diffuseComponent = diffuseIntensity * diffuseColor;
                
                // Perform component-wise multiplication between specular intensity and specular color
                Vector3 specularComponent = specularIntensity * specularColor;
                
                // Combine all components
                finalColor = finalColor + (ambientComponent.x + diffuseComponent.x + specularComponent.x),
                    (ambientComponent.y + diffuseComponent.y + specularComponent.y),
                    (ambientComponent.z + diffuseComponent.z + specularComponent.z);
                


                // Draw the pixel
                float depth = lambda1 * vertices[0].z + lambda2 * vertices[1].z + lambda3 * vertices[2].z;
                if (depth < buffer.depth[y * buffer.getWidth() + x])
                {
                    buffer.color[y * buffer.getWidth() + x] = finalColor;
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
