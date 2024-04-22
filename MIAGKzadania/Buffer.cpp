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

    // Specular component
    //Vector3 reflectDirection = reflect(-light.direction, normal);
    //float specularIntensity = pow(std::max(0.0f, Vector3::Dot(reflectDirection, -viewDirection)), light.shininess);
    //Vector3 specularContribution = light.specular * specularIntensity * light.intensity;


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


// Phong lighting model calculations for spotlight
//Vector3 Buffer::calculateSpotlightLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const Spotlight& light) {
//    Vector3 lightDirection = (light.position - position).Normal();
//
//    // Calculate angle between light direction and spotlight direction
//    float cosAngle = lightDirection.Dot(light.direction);
//    float spotFactor = 0.0f;
//    float cosCutoff = cos(ToRad(light.angle / 2.0f)); // Convert angle to radians
//    if (cosAngle >= cosCutoff) {
//        // Inside the spotlight cone
//        float smoothstepFactor = smoothstep(cosCutoff, 1.0f, cosAngle);
//        spotFactor = std::pow(std::max(0.0f, cosAngle), light.shininess) * smoothstepFactor;
//    }
//
//    // Attenuation based on distance
//
//    // Calculate diffuse component
//    float diffuseFactor = std::max(0.0f, normal.Dot(lightDirection));
//    Vector3 diffuse = light.diffuse * diffuseFactor * spotFactor;
//
//    // Calculate specular component
//    Vector3 reflectDirection = reflect(-lightDirection, normal);
//    float specularIntensity = pow(std::max(0.0f, Vector3::Dot(reflectDirection, -viewDirection)), light.shininess);
//    Vector3 specularContribution = light.specular * specularIntensity * spotFactor;
//
//    // Apply attenuation based on distance
//    float distance = (position - light.position).Length();
//    float attenuation = 1.0f / (1.0f + 0.1f * distance + 0.01f * distance * distance);
//
//    // Return the sum of diffuse and specular contributions
//    return (diffuse + specularContribution) * attenuation;
//}
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
    //// Calculate light direction
    //Vector3 lightDirection = (light.position - position).Normal();

    //// Calculate angle between light direction and spotlight direction
    //float cosAngle = lightDirection.Dot(light.direction);
    //float spotFactor = 0.0f;
    //float cosCutoff = cos(ToRad(light.angle / 2.0f)); // Convert angle to radians
    //if (cosAngle >= cosCutoff) {
    //    // Inside the spotlight cone
    //    float smoothstepFactor = smoothstep(cosCutoff, 1.0f, cosAngle);
    //    spotFactor = std::pow(std::max(0.0f, cosAngle), light.shininess) * smoothstepFactor;
    //}



    //// Calculate diffuse component
    //float diffuseFactor = std::max(0.0f, normal.Dot(lightDirection));
    //Vector3 diffuse = light.diffuse * diffuseFactor * spotFactor;

    //Vector3 reflectDirection = reflect(-lightDirection, normal);
    //float specularIntensity = pow(std::max(0.0f, Vector3::Dot(reflectDirection, lightDirection)), light.shininess);
    //Vector3 specularContribution = light.specular * specularIntensity * spotFactor;


    //float distance = (light.position - position).Length();
    //float attenuation = 1.0f / (1.0f + 0.1f * distance + 0.01f * distance * distance);
    ////return diffuse * attenuation ;

    //return diffuse;
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
//Vector3 Buffer::calculateDirectionalLighting(const Vector3& normal, const Vector3& viewDirection, const DirectionalLight& light) {
//    Vector3 lightDirection = -light.direction; // Directional light has constant direction
//    Vector3 reflectDirection = 2.0f * normal.Dot(lightDirection) * normal - lightDirection;
//
//    // Ambient
//    Vector3 ambient = light.ambient.Mul(light.color);
//
//    // Diffuse
//    float diffuseFactor = std::max(0.0f, normal.Dot(lightDirection));
//    Vector3 diffuse = light.diffuse * diffuseFactor;
//
//    // Specular
//    float specularFactor = std::pow(std::max(0.0f, viewDirection.Dot(reflectDirection)), light.shininess);
//    Vector3 specular = light.specular * specularFactor;
//
//    return ambient + diffuse + specular;
//}

//Vector3 Buffer::calculatePointLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const PointLight& light) {
    //    Vector3 lightDirection = (light.position - position).Normal();
    //    Vector3 reflectDirection = 2.0f * normal.Dot(lightDirection) * normal - lightDirection;
    //
    //    // Ambient
    //    Vector3 ambient = light.ambient.Mul(light.color);
    //
    //    // Diffuse
    //    float diffuseFactor = std::max(0.0f, normal.Dot(lightDirection));
    //    Vector3 diffuse = light.diffuse * diffuseFactor;
    //
    //    // Specular
    //    float specularFactor = std::pow(std::max(0.0f, viewDirection.Dot(reflectDirection)), light.shininess);
    //    Vector3 specular = light.specular * specularFactor;
    //
    //    return ambient + diffuse + specular;
    //}

//Vector3 Buffer::calculateSpotlightLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const Spotlight& light) {
    //    Vector3 lightDirection = (light.position - position).Normal();
    //    Vector3 reflectDirection = 2.0f * normal.Dot(lightDirection) * normal - lightDirection;
    //
    //    float cosAngle = lightDirection.Dot(light.direction);
    //    float cosCutoff = cos(ToRad(light.angle / 2.0f)); // Convert angle to radians
    //    float spotFactor = 0.0f;
    //    if (cosAngle >= cosCutoff) {
    //        spotFactor = std::pow(std::max(0.0f, cosAngle), light.shininess);
    //    }
    //
    //    // Ambient
    //    Vector3 ambient = light.ambient.Mul(light.color);
    //
    //    // Diffuse
    //    float diffuseFactor = std::max(0.0f, normal.Dot(lightDirection));
    //    Vector3 diffuse = light.diffuse * diffuseFactor * spotFactor;
    //
    //    // Specular
    //    float specularFactor = std::pow(std::max(0.0f, viewDirection.Dot(reflectDirection)), light.shininess);
    //    Vector3 specular = light.specular * specularFactor * spotFactor;
    //
    //    return ambient + diffuse + specular;
    //}







    //Vector3 Buffer::calculateGouraudDirectionalLighting(const Vector3& normal, const DirectionalLight& light) {
//    // Ambient component
//    Vector3 ambientContribution = light.ambient;
//
//    // Diffuse component
//    float diffuseIntensity = std::max(0.0f, Vector3::Dot(normal, -light.direction));
//    Vector3 diffuseContribution = light.diffuse * diffuseIntensity;
//
//    // Debug output
//    std::cout << "Dot product: " << diffuseIntensity << std::endl;
//
//    // No specular component for Gouraud shading at the vertex level
//
//    // Final color contribution at the vertex
//    return ambientContribution + diffuseContribution;
//}
//
//// Gouraud shading calculations for point light
//Vector3 Buffer::calculateGouraudPointLighting(const Vector3 & position, const Vector3 & normal, const PointLight & light) {
//    // Direction from the light to the surface point
//    Vector3 lightDirection = (light.position - position).Normal();
//
//    // Ambient component
//    Vector3 ambientContribution = light.ambient;
//
//    // Diffuse component
//    float diffuseIntensity = std::max(0.0f, Vector3::Dot(normal, lightDirection));
//    Vector3 diffuseContribution = light.diffuse * diffuseIntensity;
//
//    // No specular component for Gouraud shading at the vertex level
//
//    // Attenuation based on distance
//    float distance = (light.position - position).Length();
//    float attenuation = 1.0f / (1.0f + 0.1f * distance + 0.01f * distance * distance);
//
//    // Final color contribution at the vertex
//    return attenuation * (ambientContribution + diffuseContribution);
//}
//// Gouraud shading calculations for spotlight
//Vector3 Buffer::calculateGouraudSpotlightLighting(const Vector3& position, const Vector3& normal, const Spotlight& light) {
//    // Direction from the light to the surface point
//    Vector3 lightDirection = (position - light.position).Normal();
//
//    // Calculate the angle between the light direction and the spotlight direction
//    float angleCosine = Vector3::Dot(-lightDirection, light.direction);
//    // Check if the point is inside the spotlight cone
//    if (angleCosine > cos(light.angle)) {
//        // Ambient component
//        Vector3 ambientContribution = light.ambient;
//
//        // Diffuse component
//        float diffuseIntensity = std::max(0.0f, Vector3::Dot(normal, -lightDirection));
//        Vector3 diffuseContribution = light.diffuse * diffuseIntensity * angleCosine;
//
//        // No specular component for Gouraud shading at the vertex level
//
//        // Attenuation based on distance
//        float distance = (position - light.position).Length();
//        //float attenuation = 1.0f / (1.0f + light.attenuation.x * distance + light.attenuation.y * distance * distance);
//        float attenuation = 1.0f / (1.0f + 0.1f * distance + 0.01f * distance * distance);
//
//        // Final color contribution at the vertex
//        return attenuation * (ambientContribution + diffuseContribution);
//    }
//    else {
//        // If the point is outside the spotlight cone, it receives no light from this spotlight
//        return Vector3(0.0f, 0.0f, 0.0f);
//    }
//}