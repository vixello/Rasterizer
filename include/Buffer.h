#pragma once
#include <vector>
#include "Light.h"

class Buffer {

public:
	std::vector<float> depth;
	std::vector<unsigned int> color;
	std::vector<Light*> lights;
	Buffer(int width, int height);
	int getWidth() { return w; };
	int getHeight() { return h; };

	void SetSize(int width, int height);
	void ClearColor(unsigned int colorToClear);
	void ClearDepth(unsigned int depthToSet);
	void SaveToTGA(const char* filename, unsigned short* header);

	static Vector3 calculateDirectionalLighting(const Vector3& normal, const Vector3& viewDirection, const DirectionalLight& light);
	static Vector3 calculatePointLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const PointLight& light);
	static Vector3 calculateSpotlightLighting(const Vector3& position, const Vector3& normal, const Vector3& viewDirection, const Spotlight& light);

private:
	int w, h;
	float minx, maxx, miny, maxy, len;
    static Vector3 reflect(const Vector3& incident, const Vector3& normal) {
        return incident - 2 * Vector3::Dot(incident, normal) * normal;
    }

};

