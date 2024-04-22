#pragma once
#include "stb_image.h"
#include <vector>
#include "Light.h"

class Buffer {

public:
	std::vector<float> depth;
	std::vector<unsigned int> color;
	std::vector<unsigned char*> textures; // Vector to hold texture data
	std::vector<int> textureWidths; // Vector to hold texture widths
	std::vector<int> textureHeights; // Vector to hold texture heights
	std::vector<int> textureChannels; // Vector to hold texture channels (e.g., RGB, RGBA)
	std::vector<std::string> texturePaths;
	std::vector<Light*> lights;
	Buffer(int width, int height);
	int getWidth() { return w; };
	int getHeight() { return h; };

	void SetSize(int width, int height);
	void ClearColor(unsigned int colorToClear);
	void ClearDepth(unsigned int depthToSet);
	void SaveToTGA(const char* filename, unsigned short* header);
	void loadTexture(const std::string& filename);

	static Vector3 calculateDirectionalLighting(const Vector3& normal, const Vector3& viewDirection, const DirectionalLight& light);
	static Vector3 calculatePointLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const PointLight& light);
	static Vector3 calculateSpotlightLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const Spotlight& light);

	static Vector3 calculateGouraudDirectionalLighting(const Vector3& normal, const DirectionalLight& light);

	static Vector3 calculateGouraudPointLighting(const Vector3& position, const Vector3& normal, const PointLight& light);

	static Vector3 calculateGouraudSpotlightLighting(const Vector3& position, const Vector3& normal, const Spotlight& light);

private:
	int w, h;
	float minx, maxx, miny, maxy, len;
    static Vector3 reflect(const Vector3& incident, const Vector3& normal) {
        return incident - 2 * Vector3::Dot(incident, normal) * normal;
    }
	static float ToRad(float degrees)
	{
		return degrees * (M_PI / 180.0f);
	}
};

