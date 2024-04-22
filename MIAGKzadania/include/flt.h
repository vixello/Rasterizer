#pragma once

#include <cmath>
#include <ostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <vector>

    typedef float flt;

    constexpr flt SmallFloat = 1e-6;
    struct Point {
        float x, y, z;
        unsigned char color_r, color_g, color_b; // Color components

        // Default constructor
        Point() : x(0.0f), y(0.0f), z(0.0f), color_r(255), color_g(255), color_b(255) {} // Default color is white (255, 255, 255)

        // Constructor with arguments
        Point(float x, float y, float z) : x(x), y(y), z(z), color_r(255), color_g(255), color_b(255) {} // Default color is white (255, 255, 255)
    };
    struct Vector2 {
        float x;
        float y;

        Vector2(float _x, float _y) : x(_x), y(_y) {}

        Vector2 operator*(float scalar) const {
            return Vector2(x * scalar, y * scalar);
        }
        Vector2 operator+(const Vector2& other) const {
            return Vector2(x + other.x, y + other.y);
        }
    };

