#pragma once
#include "Vector3.h"


enum class LightType {
    Directional,
    Point,
    Spotlight
};
class Light {
public:
    LightType type;
    // Common light properties
    Vector3 position; // Added position property
    Vector3 ambient;
    Vector3 diffuse;
    //for phong
    Vector3 specular;
    float shininess; 

    // Add color property
    Vector3 color;

    Light(LightType t, const Vector3& pos, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine, const Vector3& col)
        : type(t), position(pos), ambient(amb), diffuse(diff), specular(spec), shininess(shine), color(col) {}
};

class DirectionalLight : public Light {
public:
    Vector3 direction;
    float intensity;

    DirectionalLight(const Vector3& pos, const Vector3& dir, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine, float intensity, const Vector3& col)
        : Light(LightType::Directional, pos, amb, diff, spec, shine, col), direction(dir), intensity(intensity) {}
};

class PointLight : public Light {
public:
    float intensity;

    PointLight(const Vector3& pos, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine, float intensity, const Vector3& col)
        : Light(LightType::Point, pos, amb, diff, spec, shine, col), intensity(intensity) {}
};

class Spotlight : public Light {
public:
    Vector3 direction;
    float angle;

    Spotlight(const Vector3& pos, const Vector3& dir, float a, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine, const Vector3& col)
        : Light(LightType::Spotlight, pos, amb, diff, spec, shine, col), direction(dir), angle(a) {}
};