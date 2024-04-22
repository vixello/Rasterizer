#define STB_IMAGE_IMPLEMENTATION
#include "Buffer.h"
#include "SaveToTga.h"
#include <cmath>

Buffer::Buffer(int width, int height) 
	: w(width), h(height){
	minx = 0;
	maxx = width - 1;
	miny = 0;
	maxy = height - 1;
	len = width * height;
}

void Buffer::SetSize(int width, int height)
{
	w = width;
	h = height;
	len = width * height;
	color.resize(len);
	depth.resize(len);
}

void Buffer::ClearColor(unsigned int colorToClear)
{
	std::fill(color.begin(), color.end(), colorToClear);
}

void Buffer::ClearDepth(unsigned int depthToSet)
{
	std::fill(depth.begin(), depth.end(), depthToSet);
}

void Buffer::SaveToTGA(const char* filename, unsigned short* header) {
	SaveToTga::SaveImage(filename, header, w, h, color);
}

// Phong lighting model calculations for directional light
Vector3 Buffer::calculateDirectionalLighting(const Vector3& normal, const Vector3& viewDirection, const DirectionalLight& light) {
    // Diffuse component
    float diffuseFactor = std::max(0.0f, normal.Dot(-light.direction));
    Vector3 diffuse = light.diffuse * diffuseFactor * light.intensity;

    // Final color contribution
    return  diffuse ;
}


Vector3 Buffer::calculatePointLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const PointLight& light) {
    Vector3 lightDirection = (light.position - position).Normal();
    Vector3 reflectDirection = 2.0f * normal.Dot(lightDirection) * normal - lightDirection;

    Vector3 ambient = light.ambient.Mul(light.color);

    float diffuseFactor = std::max(0.0f, normal.Dot(lightDirection));
    Vector3 diffuse = light.diffuse * diffuseFactor * light.intensity;

    float specularFactor = std::pow(std::max(0.0f, lightDirection.Dot(reflectDirection)), light.shininess);
    Vector3 specular = light.specular * specularFactor * light.intensity;
    return /*ambient + */diffuse + specular;
}


float smoothstep(float edge0, float edge1, float x) {
    // Scale, bias and saturate x to 0..1 range
    x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
    // Evaluate polynomial
    return x * x * (3 - 2 * x);
}

Vector3 Buffer::calculateSpotlightLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const Spotlight& light) {
    // Calculate light direction
    Vector3 lightDirection = (light.position - position).Normal();

    // Calculate angle between light direction and spotlight direction
    float cosAngle = lightDirection.Dot(light.direction);
    float spotFactor = 0.0f;
    float cosCutoff = cos(ToRad(light.angle / 2.0f)); // Convert angle to radians
    if (cosAngle >= cosCutoff) {
        // Inside the spotlight cone
        spotFactor = std::pow(std::max(0.0f, cosAngle), light.shininess);

        // Modify shininess based on the angle
        float angleFactor = (cosAngle - cosCutoff) / (1.0f - cosCutoff); // Scale factor based on angle
        float adjustedShininess = light.shininess * angleFactor; // Adjusted shininess
        spotFactor = std::pow(spotFactor, adjustedShininess); // Apply adjusted shininess

    }
    // Calculate diffuse component
    float diffuseFactor = std::max(0.0f, normal.Dot(lightDirection));
    Vector3 diffuse = light.diffuse * diffuseFactor * spotFactor;

    // No ambient component for spotlight in Gouraud shading

    Vector3 reflectDirection = reflect(-lightDirection, normal);
    float specularIntensity = pow(std::max(0.0f, Vector3::Dot(reflectDirection, -viewDirection)), light.shininess);
    Vector3 specularContribution = light.specular * specularIntensity * spotFactor;
    float smoothstepFactor = smoothstep(cosCutoff, 1.0f, cosAngle);

    return diffuse;
}


Vector3 Buffer::calculateGouraudDirectionalLighting(const Vector3& normal, const DirectionalLight& light) {
    // Calculate diffuse component
    float diffuseFactor = std::max(0.0f, normal.Dot(-light.direction));
    Vector3 diffuse = light.diffuse * diffuseFactor * light.intensity;

    // No ambient component for directional light in Gouraud shading

    // No specular component for directional light in Gouraud shading

    return diffuse;
}
Vector3 Buffer::calculateGouraudPointLighting(const Vector3& position, const Vector3& normal, const PointLight& light) {
    // Calculate light direction
    Vector3 lightDirection = (light.position - position).Normal();
    Vector3 reflectDirection = 2.0f * normal.Dot(lightDirection) * normal - lightDirection;

    // Calculate diffuse component
    float diffuseFactor = std::max(0.0f, normal.Dot(lightDirection));
    Vector3 diffuse = light.diffuse * diffuseFactor * light.intensity;

    // No ambient component for point light in Gouraud shading
    Vector3 ambient = light.ambient.Mul(light.color);

    // No specular component for point light in Gouraud shading
    float specularFactor = std::pow(std::max(0.0f, lightDirection.Dot(reflectDirection)), light.shininess);
    Vector3 specular = light.specular * specularFactor * light.intensity;
    return ambient + diffuse + specular;
}

Vector3 Buffer::calculateGouraudSpotlightLighting(const Vector3& position, const Vector3& normal, const Spotlight& light) {
    // Calculate light direction
    Vector3 lightDirection = (light.position - position).Normal();

    // Calculate angle between light direction and spotlight direction
    float cosAngle = lightDirection.Dot(light.direction);
    float spotFactor = 0.0f;
    float cosCutoff = cos(ToRad(light.angle / 2.0f)); // Convert angle to radians
    if (cosAngle >= cosCutoff) {
        // Inside the spotlight cone
        spotFactor = std::pow(std::max(0.0f, cosAngle), light.shininess);

        // Modify shininess based on the angle
        float angleFactor = (cosAngle - cosCutoff) / (1.0f - cosCutoff); // Scale factor based on angle
        float adjustedShininess = light.shininess * angleFactor; // Adjusted shininess
        spotFactor = std::pow(spotFactor, adjustedShininess); // Apply adjusted shininess
    }
    // Calculate diffuse component
    float diffuseFactor = std::max(0.0f, normal.Dot(lightDirection));
    Vector3 diffuse = light.diffuse * diffuseFactor * spotFactor;

    // No ambient component for spotlight in Gouraud shading

    Vector3 reflectDirection = reflect(-lightDirection, normal);
    float specularIntensity = pow(std::max(0.0f, Vector3::Dot(reflectDirection, -lightDirection)), light.shininess);
    Vector3 specularContribution = light.specular * specularIntensity * spotFactor;

    return diffuse;

}

void Buffer::loadTexture(const std::string& filename) {
    int width, height, channels;
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha); // Load texture data with alpha channel

    if (!data) {
        std::cerr << "Failed to load texture: " << filename << std::endl;
        return;
    }

    textures.push_back(data);
    textureWidths.push_back(width);
    textureHeights.push_back(height);
    textureChannels.push_back(channels);
    texturePaths.push_back(filename);

    std::cout << "Texture loaded successfully: " << filename << std::endl;
}
