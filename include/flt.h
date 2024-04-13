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
        Point(float x, float y, float z) : x(x), y(y), z(z) {}
    };