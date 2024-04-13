#include "Buffer.h"
#include "SaveToTga.h"

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
    float diffuseIntensity = std::max(0.0f, Vector3::Dot(normal, -light.direction));
    Vector3 diffuseContribution = light.diffuse * diffuseIntensity;

    // Specular component
    Vector3 reflectDirection = reflect(-light.direction, normal);
    float specularIntensity = pow(std::max(0.0f, Vector3::Dot(reflectDirection, viewDirection)), light.shininess);
    Vector3 specularContribution = light.specular * specularIntensity;

    // Final color contribution
    return light.ambient + diffuseContribution + specularContribution;
}

// Phong lighting model calculations for point light
Vector3 Buffer::calculatePointLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const PointLight& light) {
    // Direction from the light to the surface point
    Vector3 lightDirection = (light.position - position).Normal(); // Corrected calculation

    // Diffuse component
    float diffuseIntensity = std::max(0.0f, Vector3::Dot(normal, lightDirection));
    Vector3 diffuseContribution = light.diffuse * diffuseIntensity;

    // Specular component
    Vector3 reflectDirection = reflect(-lightDirection, normal);
    float specularIntensity = pow(std::max(0.0f, Vector3::Dot(reflectDirection, viewDirection)), light.shininess);
    Vector3 specularContribution = light.specular * specularIntensity;

    // Attenuation based on distance
    float distance = (light.position - position).Length();
    //std::cout << "distance   "<<distance << std::endl;
    float attenuation = 1.0f / (1.0f + 0.1f * distance + 0.01f * distance * distance); // Adjust these parameters if needed

    // Final color contribution
    return attenuation*(light.ambient + diffuseContribution + specularContribution) ; // Apply attenuation to all components
}


// Phong lighting model calculations for spotlight
Vector3 Buffer::calculateSpotlightLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const Spotlight& light) {
    // Direction from the light to the surface point
    Vector3 lightDirection = (position - light.position).Normal();

    // Calculate the angle between the light direction and the spotlight direction
    float angleCosine = Vector3::Dot(-lightDirection, light.direction);
    float spotlightFactor = std::max(0.0f, angleCosine);

    // Check if the point is inside the spotlight cone
    if (spotlightFactor > cos(light.angle)) {
        // Diffuse component
        float diffuseIntensity = std::max(0.0f, Vector3::Dot(normal, -lightDirection));
        Vector3 diffuseContribution = light.diffuse * diffuseIntensity * spotlightFactor;

        // Specular component
        Vector3 reflectDirection = reflect(-lightDirection, normal);
        float specularIntensity = pow(std::max(0.0f, Vector3::Dot(reflectDirection, viewDirection)), light.shininess);
        Vector3 specularContribution = light.specular * specularIntensity * spotlightFactor;

        // Attenuation based on distance
        float distance = (position - light.position).Length();
        float attenuation = 1.0f / (1.0f + 0.1f * distance + 0.01f * distance * distance);

        // Final color contribution
        return light.ambient * attenuation + diffuseContribution * attenuation + specularContribution * attenuation;
    }
    else {
        // Outside the spotlight cone, no contribution
        return Vector3(0, 0, 0);
    }
}
