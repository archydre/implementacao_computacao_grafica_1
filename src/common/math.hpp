#ifndef CG_MATH_HPP
#define CG_MATH_HPP

#include <cmath>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

namespace cg {

constexpr double PI = 3.14159265358979323846;

// Conversões de ângulos
inline double radians(double deg) {
    return deg * (PI / 180.0);
}

inline double degrees(double rad) {
    return rad * (180.0 / PI);
}

// Clamp genérico
template <typename T>
inline T clamp(T val, T minVal, T maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

// Vetor 3D
struct Vec3 {
    double x{0.0};
    double y{0.0};
    double z{0.0};

    Vec3() = default;
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& other) const {
        return {x + other.x, y + other.y, z + other.z};
    }

    Vec3 operator-(const Vec3& other) const {
        return {x - other.x, y - other.y, z - other.z};
    }

    Vec3 operator*(double s) const {
        return {x * s, y * s, z * s};
    }

    Vec3 operator/(double s) const {
        return {x / s, y / s, z / s};
    }

    Vec3 operator-() const {
        return {-x, -y, -z};
    }
};

// Vetor 4D (Coordenadas Homogêneas)
struct Vec4 {
    double x{0.0};
    double y{0.0};
    double z{0.0};
    double w{1.0};

    Vec4() = default;
    Vec4(double x, double y, double z, double w = 1.0) : x(x), y(y), z(z), w(w) {}

    Vec4 operator+(const Vec4& other) const {
        return {x + other.x, y + other.y, z + other.z, w + other.w};
    }

    Vec4 operator-(const Vec4& other) const {
        return {x - other.x, y - other.y, z - other.z, w - other.w};
    }

    Vec4 operator*(double s) const {
        return {x * s, y * s, z * s, w * s};
    }

    Vec3 toVec3() const {
        return {x, y, z};
    }
};

// Operações vetoriais
inline double dot(const Vec3& a, const Vec3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vec3 cross(const Vec3& a, const Vec3& b) {
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

inline double length(const Vec3& v) {
    return std::sqrt(dot(v, v));
}

inline Vec3 normalize(const Vec3& v) {
    double len = length(v);
    if (len < 1e-9) {
        return {0.0, 0.0, 0.0};
    }
    return v / len;
}

// Produto misto: (u x v) . w ou u . (v x w)
inline double scalarTripleProduct(const Vec3& u, const Vec3& v, const Vec3& w) {
    return dot(u, cross(v, w));
}

// Matriz 4x4 Homogênea
// Convenção Adotada:
// 1. Vetor-coluna: v' = M * v
// 2. Componente homogênea w na 4ª linha/coluna
// 3. Armazenamento interno: m[linha][coluna] (Row-major)
// 4. Multiplicação: (A * B)[i][j] = sum_k (A[i][k] * B[k][j])
struct Mat4 {
    double m[4][4]{{0.0}};

    Mat4(); // Inicializa como matriz de zeros

    static Mat4 identity();
    static Mat4 translation(double tx, double ty, double tz);
    static Mat4 scale(double sx, double sy, double sz);
    static Mat4 rotation(double angleRad, const Vec3& axis);

    // Projeções canônicas conforme slides
    static Mat4 orthoXY();
    static Mat4 oblique(double l, double alphaRad);
    static Mat4 cavaliere(double alphaRad = radians(45.0));
    static Mat4 cabinet(double alphaRad = radians(45.0));
    static Mat4 perspective(double fx, double fy, double fz);

    // Multiplicação matriz x matriz
    Mat4 operator*(const Mat4& other) const;

    // Multiplicação matriz x vetor-coluna homogêneo (4D)
    Vec4 operator*(const Vec4& v) const;

    // Multiplicação matriz x vetor 3D (assume w=1.0)
    Vec4 operator*(const Vec3& v) const;

    // Impressão formatada
    void print(const std::string& name = "") const;
};

// Cálculo de vetor normal médio pelo Método de Newell
Vec3 calculateNewellNormal(const std::vector<Vec3>& polygon);

// Impressões utilitárias
void printVec3(const std::string& label, const Vec3& v);
void printVec4(const std::string& label, const Vec4& v);

} // namespace cg

#endif // CG_MATH_HPP
