#pragma once

#include "Vector4.h"
#include "flt.h"


    class Vector3 {
    public:
        flt x;
        flt y;
        flt z;

        Vector3() : x(0), y(0), z(0) {}

        explicit Vector3(flt x) : x(x), y(x), z(x) {}

        Vector3(flt X, flt Y, flt Z) : x(X), y(Y), z(Z) {}

        explicit Vector3(const Vector4& Vector4);

        flt Length() const { return std::sqrt(x * x + y * y + z * z); }
        flt SquaredLength() const { return x * x + y * y + z * z; }

        Vector3 Normal() const {
            const double length = Length();
            if (length < SmallFloat)
                return Vector3{ 0. };

            return *this / length;
        }

        Vector3 Sat() const {
            Vector3 result;

            result.x = std::clamp(x, 0.f, 1.f);
            result.y = std::clamp(y, 0.f, 1.f);
            result.z = std::clamp(z, 0.f, 1.f);

            return result;
        }

        bool IsNear(const Vector3& vector3, flt error = SmallFloat) const {
            return std::abs(Length() - vector3.Length()) < error;
        }

        flt Dot(const Vector3& rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }
        static flt Dot(const Vector3& a, const Vector3& b) { return a.Dot(b); }

        flt Angle(const Vector3& rhs) const { return acos(Dot(rhs) / (Length() * rhs.Length())); }

        Vector3 Abs() const { return { std::abs(x), std::abs(y), std::abs(z) }; }

        Vector3 Cross(const Vector3& rhs) const {
            return {
                y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x
            };
        }

        static Vector3 Cross(const Vector3& a, const Vector3& b) { return a.Cross(b); }

        static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
            return a + (b - a) * t;
        }

        Vector3& operator=(Vector3 const& another) {
            if (this == &another) {
                return *this;
            }

            x = another.x;
            y = another.y;
            z = another.z;
            return *this;
        }

        Vector3 operator-() const {
            Vector3 result;
            result.x = -x;
            result.y = -y;
            result.z = -z;
            return result;
        }

        Vector3 operator+(Vector3 const& another) const {
            Vector3 result;
            result.x = x + another.x;
            result.y = y + another.y;
            result.z = z + another.z;
            return result;
        }

        Vector3 operator-(Vector3 const& another) const {
            Vector3 result;
            result.x = x - another.x;
            result.y = y - another.y;
            result.z = z - another.z;
            return result;
        }

        Vector3 Mul(const Vector3& another) const {
            Vector3 result;
            result.x = x * another.x;
            result.y = y * another.y;
            result.z = z * another.z;
            return result;
        }

        void operator+=(Vector3 const& another) { *this = *this + another; }
        void operator-=(Vector3 const& another) { *this = *this - another; }

        Vector3 operator*(flt const& scalar) const {
            Vector3 result;
            result.x = x * scalar;
            result.y = y * scalar;
            result.z = z * scalar;
            return result;
        }

        friend Vector3 operator*(flt scalar, const Vector3& vector) {
            return vector * scalar;
        }

        Vector3 operator/(flt const& scalar) const {
            Vector3 result;
            result.x = x / scalar;
            result.y = y / scalar;
            result.z = z / scalar;
            return result;
        }

        flt operator[] (const uint8_t index) const {
            const flt values[] = { x, y, z };
            return values[index];
        }

        flt& operator[] (const uint8_t index) {
            flt* values[] = { &x, &y, &z };
            return *values[index];
        }

        void operator*=(flt const& scalar) { *this = *this * scalar; }
        void operator/=(flt const& scalar) { *this = *this / scalar; }
        bool operator==(const Vector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }

        bool operator!=(const Vector3& rhs) const { return !(rhs == *this); }
        bool operator<(const Vector3& rhs) const { return Length() < rhs.Length(); }

        bool operator>(const Vector3& rhs) const { return rhs < *this; }
        bool operator<=(const Vector3& rhs) const { return !(rhs < *this); }
        bool operator>=(const Vector3& rhs) const { return !(*this < rhs); }

        friend std::ostream& operator<<(std::ostream& Os, const Vector3& vector3) {
            Os << "[" << vector3.x << "," << vector3.y << "," << vector3.z << "]";
            return Os;
        }

        std::string str() const {
            std::ostringstream result;
            result << *this;
            return result.str();
        }

        //static const Vector3 Up;
        //static const Vector3 Forward;
        //static const Vector3 Right;
        //static const Vector3 Zero;

        //static const Vector3 Red;
        //static const Vector3 Green;
        //static const Vector3 Blue;

        friend class Vector4;
    };
//Vector3::Vector3(const Vector4& Vector4)
//        : x(Vector4.x)
//        , y(Vector4.y)
//        , z(Vector4.z) {}
    
//const Vector3 Vector3::Up {0., 1., 0.}; 
//const Vector3 Vector3::Forward {0., 0., -1.}; 
//const Vector3 Vector3::Right {1., 0., 0.}; 
//const Vector3 Vector3::Zero {0., 0., 0.};
//
//const Vector3 Vector3::Red {1., 0., 0.}; 
//const Vector3 Vector3::Green {0., 1., 0.}; 
//const Vector3 Vector3::Blue {0., 0., 1.}; 

