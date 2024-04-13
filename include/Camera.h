#pragma once
#include "float4x4.h"

class Camera {
public:
    static float4x4 Translation(flt x, flt y, flt z) {
        float4x4 result(1);
        result[3] = Vector4{ x, y, z, 1 };

        return  result;
    }


    static float4x4 Scale(flt x, flt y, flt z)
    {
        float4x4 result(1);

        result[{0, 0}] = x;
        result[{1, 1}] = y;
        result[{2, 2}] = z;
        return  result;
    }
    static float ToRad(float degrees)
    {
        return degrees * (M_PI / 180.0f);
    }

    static float4x4 RotationX(float degrees)
    {
        float radians = ToRad(degrees);
        float4x4 result(1);

        result[{1, 1}] = std::cos(radians);
        result[{2, 1}] = -std::sin(radians);
        result[{1, 2}] = std::sin(radians);
        result[{2, 2}] = std::cos(radians);
        return  result;
    }

    static float4x4 RotationY(float degrees)
    {
        float radians = ToRad(degrees);
        float4x4 result(1);

        result[{0, 0}] = std::cos(radians);
        result[{0, 2}] = std::sin(radians);
        result[{2, 0}] = -std::sin(radians);
        result[{2, 2}] = std::cos(radians);
        return  result;
    }

    static float4x4 RotationZ(float degrees)
    {
        float radians = ToRad(degrees);
        float4x4 result(1);

        result[{0, 0}] = std::cos(radians);
        result[{0, 1}] = -std::sin(radians);
        result[{1, 0}] = std::sin(radians);
        result[{1, 1}] = std::cos(radians);
        return  result;
    }

    static float4x4 Perspective(flt fov, flt aspect, flt near, flt far) {
        float4x4 result{ 0 };

        const flt f = std::cos(fov / 2.) / std::sin(fov / 2.);

        result[0] = Vector4{ f / aspect, 0, 0, 0 };
        result[1] = Vector4{ 0, f, 0, 0 };
        result[2] = Vector4{ 0, 0, -(far + near) / (near - far), 1. };
        result[3] = Vector4{ 0, 0, static_cast<flt>(2.) * far * near / (near - far), 0. };

        return result;
    }

    static float4x4 LookAt(const Vector3& eye, const Vector3& center, const Vector3& up) {
        float4x4 result{ 1 };

        const Vector3 forward = (center - eye).Normal();
        const Vector3 right = Vector3::Cross(forward, up);
        const Vector3 trueUp = Vector3::Cross(right, forward);

        result[0] = Vector4{ right.x, trueUp.x, forward.x, 0 };
        result[1] = Vector4{ right.y, trueUp.y, forward.y, 0 };
        result[2] = Vector4{ right.z, trueUp.z, forward.z, 0 };
        result[3] = Vector4{ 0., 0., 0., 1. };

        return result * Translation(-eye.x, -eye.y, -eye.z);
    }
    static void vertexShader(std::vector<Point>& vertices, const float4x4& transform) {
        for (auto& vertex : vertices) {
            // Convert Point vertex to float4 for matrix multiplication
            Vector4 vertexVec(vertex.x, vertex.y, vertex.z, 1.0f);
            vertexVec = transform * vertexVec;
            // Update the vertex coordinates with transformed values
            vertex.x = vertexVec.x;
            vertex.y = vertexVec.y;
            vertex.z = vertexVec.z;
        }
    }
    //static void vertexShader(std::vector<Point>& vertices, const float4x4& transform) {
    //    for (auto& vertex : vertices) {
    //        Vector4 vertexVec(vertex.x, vertex.y, vertex.z, 1.0f);
    //        vertexVec = transform * vertexVec;

    //        // Perform perspective division
    //        if (vertexVec.w != 0) { // Avoid division by zero
    //            vertex.x = vertexVec.x / vertexVec.w;
    //            vertex.y = vertexVec.y / vertexVec.w;
    //            vertex.z = vertexVec.z / vertexVec.w;
    //        }
    //    }
    //}
private:
    std::array<Vector4, 4> grid{};
};