#pragma once

//#include <cmath>
//#include <cstdint>
//#include <ostream>
//#include <sstream>
//#include <string>

#include "flt.h"


    class Vector3;

    class Vector4 {
    public:
        flt x;
        flt y;
        flt z;
        flt w;

        Vector4() : x(0), y(0), z(0), w(0) {};
        //Vector4(const Vector3& vec3, flt w);
        Vector4(flt x) : x(x), y(x), z(x), w(x) {};
        Vector4(flt x, flt y, flt z, flt w) : x(x), y(y), z(z), w(w) {}

        Vector4& operator=(Vector4 const& another) {
            if (this == &another) {
                return *this;
            }

            x = another.x;
            y = another.y;
            z = another.z;
            w = another.w;
            return *this;
        }

        Vector4 operator-() const {
            Vector4 result;
            result.x = -x;
            result.y = -y;
            result.z = -z;
            result.w = -w;
            return result;
        }

        Vector4 operator+(Vector4 const& another) const {
            Vector4 result;
            result.x = x + another.x;
            result.y = y + another.y;
            result.z = z + another.z;
            result.w = w + another.w;
            return result;
        }

        flt operator[] (const uint8_t index) const {
            const flt values[] = { x, y, z, w };
            return values[index];
        }

        flt& operator[] (const uint8_t index) {
            flt* values[] = { &x, &y, &z, &w };
            return *values[index];
        }

        Vector4 operator-(Vector4 const& another) const {
            Vector4 result = *this + (-another);
            return result;
        }

        void operator+=(Vector4 const& another) { *this = *this + another; }
        void operator-=(Vector4 const& another) { *this = *this - another; }

        Vector4 operator*(flt const& scalar) const {
            Vector4 result;
            result.x = x * scalar;
            result.y = y * scalar;
            result.z = z * scalar;
            result.w = w * scalar;
            return result;
        }

        Vector4 operator/(flt const& scalar) const {
            Vector4 result;
            result.x = x / scalar;
            result.y = y / scalar;
            result.z = z / scalar;
            result.w = w / scalar;
            return result;
        }

        void operator*=(flt const& scalar) { *this = *this * scalar; }
        void operator/=(flt const& scalar) { *this = *this / scalar; }
        bool operator==(const Vector4& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
        bool operator!=(const Vector4& rhs) const { return !(rhs == *this); }

        friend std::ostream& operator<<(std::ostream& os, const Vector4& vector4) {
            os << "[" << vector4.x << "," << vector4.y << "," << vector4.z << "," << vector4.w << "]";
            return os;
        }

        std::string str() const {
            std::ostringstream result;
            result << *this;
            return result.str();
        }

        flt Length() const { return std::sqrt(x * x + y * y + z * z + w * w); }

        Vector4 Normal() const {
            double length = Length();
            if (length < std::numeric_limits<float>::min())
                return Vector4{ 0. };

            return *this / length;
        }

        flt Dot(const Vector4& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
        static flt Dot(const Vector4& a, Vector4& b) { return a.Dot(b); }

        flt Angle(Vector4& rhs) const { return acos(Dot(rhs) / (Length() * rhs.Length())); }

        friend class float4x4;
        friend class Vector3;
    };
