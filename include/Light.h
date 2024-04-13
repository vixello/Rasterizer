#pragma once
#include "Vector3.h"

//class Light {
//public:
//    Vector3 position;
//    Vector3 ambient;
//    Vector3 diffuse;
//    Vector3 specular;
//    float shininess;
//    Light()
//        : position(Vector3()), ambient(Vector3()), diffuse(Vector3()), specular(Vector3()), shininess(0.0f) {}
//
//    Light(const Vector3& pos, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
//        : position(pos), ambient(amb), diffuse(diff), specular(spec), shininess(shine) {}
//};
//
//class DirectionalLight : public Light {
//public:
//    DirectionalLight(const Vector3& dir, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
//        : Light(Vector3(), amb, diff, spec, shine), direction(dir) {}
//
//    Vector3 direction;
//};
//
//class PointLight : public Light {
//public:
//    PointLight(const Vector3& pos, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
//        : Light(pos, amb, diff, spec, shine) {}
//};
//
//class ReflectorLight : public Light {
//public:
//    ReflectorLight(const Vector3& pos, const Vector3& dir, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
//        : Light(pos, amb, diff, spec, shine), direction(dir) {}
//
//    Vector3 direction;
//};
enum class LightType {
    Directional,
    Point,
    Spotlight
};

//class Light {
//public:
//    LightType type;
//    // Common light properties
//    Vector3 ambient;
//    Vector3 diffuse;
//    Vector3 specular;
//    float shininess; // Add shininess property
//
//    Light(LightType t, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
//        : type(t), ambient(amb), diffuse(diff), specular(spec), shininess(shine) {}
//};
//
//class DirectionalLight : public Light {
//public:
//    Vector3 direction; // Add direction property
//
//    DirectionalLight(const Vector3& dir, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
//        : Light(LightType::Directional, amb, diff, spec, shine), direction(dir) {}
//};
//
//class PointLight : public Light {
//public:
//    Vector3 position;
//
//    PointLight(const Vector3& pos, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
//        : Light(LightType::Point, amb, diff, spec, shine), position(pos) {}
//};
//
//class Spotlight : public Light {
//public:
//    Vector3 position;
//    Vector3 direction;
//    float angle;
//
//    Spotlight(const Vector3& pos, const Vector3& dir, float a, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
//        : Light(LightType::Spotlight, amb, diff, spec, shine), position(pos), direction(dir), angle(a) {}
//};

class Light {
public:
    LightType type;
    // Common light properties
    Vector3 position; // Added position property
    Vector3 ambient;
    Vector3 diffuse;
    Vector3 specular;
    float shininess; // Add shininess property

    Light(LightType t, const Vector3& pos, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
        : type(t), position(pos), ambient(amb), diffuse(diff), specular(spec), shininess(shine) {}
};

class DirectionalLight : public Light {
public:
    Vector3 direction; // Add direction property

    DirectionalLight(const Vector3& pos, const Vector3& dir, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
        : Light(LightType::Directional, pos, amb, diff, spec, shine), direction(dir) {}
};

class PointLight : public Light {
public:
    PointLight(const Vector3& pos, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
        : Light(LightType::Point, pos, amb, diff, spec, shine) {}
};

class Spotlight : public Light {
public:
    Vector3 direction;
    float angle;

    Spotlight(const Vector3& pos, const Vector3& dir, float a, const Vector3& amb, const Vector3& diff, const Vector3& spec, float shine)
        : Light(LightType::Spotlight, pos, amb, diff, spec, shine), direction(dir), angle(a) {}
};