#pragma once

#include <array>
#include <cstdint>
//#include <ostream>
//#include <cmath>


#include "Vector3.h"
constexpr float M_PI = 3.14159265358979323846f;


    class float4x4 {
    private:
        std::array<Vector4, 4> grid{};

    public:
        float4x4() : float4x4(0) {}

        explicit float4x4(flt x) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (i == j)
                        grid[i][j] = x;
                    else
                        grid[i][j] = 0;
                }
            }
        }

        float4x4(const float4x4& clone) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    grid[i][j] = clone.grid[i][j];
                }
            }
        }

        flt operator[](const std::pair<int, int>& coordinates) const {
            return grid[coordinates.first][coordinates.second];
        }

        flt& operator[](const std::pair<int, int>& coordinates) {
            return grid[coordinates.first][coordinates.second];
        }

        Vector4& operator[](const uint32_t index) {
            return grid[index];
        }

        const Vector4& operator[](const uint32_t index) const {
            return grid[index];
        }

        float4x4 operator-() const {
            float4x4 result(0);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.grid[i][j] = -this->grid[i][j];
                }
            }
            return result;
        }
        float4x4 operator-(const float4x4& m) const {
            float4x4 result(0);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.grid[i][j] = grid[i][j] - m.grid[i][j];
                }
            }

            return result;
        }

        float4x4 operator+(const float4x4& m) const {
            float4x4 result(0);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.grid[i][j] = grid[i][j] + m.grid[i][j];
                }
            }

            return result;
        }

        float4x4& operator=(const float4x4& m) {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    grid[i][j] = m.grid[i][j];
                }
            }

            return *this;
        }

        bool operator==(const float4x4& m) const {
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    if (grid[i][j] != m.grid[i][j])
                        return false;
                }
            }

            return true;
        }

        //void operator+=(const float4x4& another) {
        //    *this = *this + another;
        //}
        void operator+=(const float4x4& matrix) {
            float4x4 result(0);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.grid[i][j] = grid[i][j] + matrix.grid[i][j];
                }
            }
            grid = result.grid;
        }
        void operator-=(const float4x4& m) {
            *this = *this - m;
        }

        float4x4 operator*(const flt scalar) const {
            float4x4 result(0);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    result.grid[i][j] = scalar * grid[i][j];
                }
            }
            return result;
        }

        void operator*=(flt scalar) {
            *this = *this * scalar;
        }

        float4x4 operator*(const float4x4& m) const {
            float4x4 result(0);
            for (int i = 0; i < 4; ++i) {
                for (int j = 0; j < 4; ++j) {
                    for (int k = 0; k < 4; ++k) {
                        result.grid[i][j] += grid[k][j] * m.grid[i][k];
                    }
                }
            }

            return result;
        }

        void operator*=(const float4x4& m) {
            *this = *this * m;
        }

        float4x4 Transpose() const {
            float4x4 result(0);

            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    result.grid[i][j] = grid[j][i];
                }
            }
            return result;
        }

        flt Det() const {
            flt result = 1;
            float4x4 helper(*this);
            for (int i = 0; i < 4; i++) {
                for (int k = i + 1; k < 4; k++) {
                    const flt coefficient = helper.grid[k][i] / helper.grid[i][i];
                    for (int j = i; j < 4; j++)
                        helper.grid[k][j] = helper.grid[k][j] - coefficient * helper.grid[i][j];
                }
            }
            for (int i = 0; i < 4; i++)
                result *= helper.grid[i][i];
            return result;
        }

        friend std::ostream& operator<<(std::ostream& os, const float4x4& mat4) {
            for (int i = 0; i < 4; ++i) {
                os << "[ ";
                for (int j = 0; j < 4; ++j) {
                    os << mat4.grid[j][i] << " ";
                }
                os << "]" << "\n";
            }
            return os;
        }

        Vector4 operator*(const Vector4& vector4) const {
            Vector4 result;
            result.x = vector4.x * grid[0][0] + vector4.y * grid[1][0] + vector4.z * grid[2][0] + vector4.w * grid[3][0];
            result.y = vector4.x * grid[0][1] + vector4.y * grid[1][1] + vector4.z * grid[2][1] + vector4.w * grid[3][1];
            result.z = vector4.x * grid[0][2] + vector4.y * grid[1][2] + vector4.z * grid[2][2] + vector4.w * grid[3][2];
            result.w = vector4.x * grid[0][3] + vector4.y * grid[1][3] + vector4.z * grid[2][3] + vector4.w * grid[3][3];

            return result;
        }
    };

//float ToRad(float degrees) {
//    return degrees * (M_PI / 180.0f);
//}