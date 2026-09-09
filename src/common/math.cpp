#include "math.hpp"

namespace cg {

Mat4::Mat4() {
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            m[r][c] = 0.0;
        }
    }
}

Mat4 Mat4::identity() {
    Mat4 res;
    for (int i = 0; i < 4; ++i) {
        res.m[i][i] = 1.0;
    }
    return res;
}

Mat4 Mat4::translation(double tx, double ty, double tz) {
    Mat4 res = identity();
    res.m[0][3] = (std::abs(tx) < 1e-9) ? 0.0 : tx;
    res.m[1][3] = (std::abs(ty) < 1e-9) ? 0.0 : ty;
    res.m[2][3] = (std::abs(tz) < 1e-9) ? 0.0 : tz;
    return res;
}

Mat4 Mat4::scale(double sx, double sy, double sz) {
    Mat4 res = identity();
    res.m[0][0] = sx;
    res.m[1][1] = sy;
    res.m[2][2] = sz;
    return res;
}

Mat4 Mat4::rotation(double angleRad, const Vec3& axis) {
    Vec3 u = normalize(axis);
    if (length(u) < 1e-9 || std::abs(angleRad) < 1e-9) {
        return identity();
    }

    double c = std::cos(angleRad);
    double s = std::sin(angleRad);
    double C = 1.0 - c;

    Mat4 res = identity();
    res.m[0][0] = u.x * u.x * C + c;
    res.m[0][1] = u.x * u.y * C - u.z * s;
    res.m[0][2] = u.x * u.z * C + u.y * s;

    res.m[1][0] = u.y * u.x * C + u.z * s;
    res.m[1][1] = u.y * u.y * C + c;
    res.m[1][2] = u.y * u.z * C - u.x * s;

    res.m[2][0] = u.z * u.x * C - u.y * s;
    res.m[2][1] = u.z * u.y * C + u.x * s;
    res.m[2][2] = u.z * u.z * C + c;

    // Limpar valores numericamente desprezíveis (como -0.0000)
    for (int r = 0; r < 4; ++r) {
        for (int col = 0; col < 4; ++col) {
            if (std::abs(res.m[r][col]) < 1e-12) {
                res.m[r][col] = 0.0;
            }
        }
    }

    return res;
}

Mat4 Mat4::orthoXY() {
    Mat4 res = identity();
    res.m[2][2] = 0.0; // z' = 0
    return res;
}

Mat4 Mat4::oblique(double l, double alphaRad) {
    Mat4 res = identity();
    res.m[0][2] = l * std::cos(alphaRad);
    res.m[1][2] = l * std::sin(alphaRad);
    res.m[2][2] = 0.0; // Projeta no plano z = 0
    return res;
}

Mat4 Mat4::cavaliere(double alphaRad) {
    // tan(beta) = 1 => l = 1.0
    return oblique(1.0, alphaRad);
}

Mat4 Mat4::cabinet(double alphaRad) {
    // tan(beta) = 2 => l = 0.5
    return oblique(0.5, alphaRad);
}

Mat4 Mat4::perspective(double fx, double fy, double fz) {
    Mat4 res = identity();
    res.m[2][2] = 0.0; // z' = 0

    // Componentes de fuga na 4ª linha (para vetor-coluna: w' = p*x + q*y + r*z + 1)
    if (std::abs(fx) > 1e-6) {
        res.m[3][0] = -1.0 / fx;
    }
    if (std::abs(fy) > 1e-6) {
        res.m[3][1] = -1.0 / fy;
    }
    if (std::abs(fz) > 1e-6) {
        res.m[3][2] = -1.0 / fz;
    }

    return res;
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 res;
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {
            double sum = 0.0;
            for (int k = 0; k < 4; ++k) {
                sum += m[r][k] * other.m[k][c];
            }
            if (std::abs(sum) < 1e-12) sum = 0.0;
            res.m[r][c] = sum;
        }
    }
    return res;
}

Vec4 Mat4::operator*(const Vec4& v) const {
    Vec4 res;
    res.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    res.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    res.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    res.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
    if (std::abs(res.x) < 1e-12) res.x = 0.0;
    if (std::abs(res.y) < 1e-12) res.y = 0.0;
    if (std::abs(res.z) < 1e-12) res.z = 0.0;
    if (std::abs(res.w) < 1e-12) res.w = 0.0;
    return res;
}

Vec4 Mat4::operator*(const Vec3& v) const {
    return (*this) * Vec4(v.x, v.y, v.z, 1.0);
}

void Mat4::print(const std::string& name) const {
    if (!name.empty()) {
        std::cout << name << " =\n";
    }
    std::cout << std::fixed << std::setprecision(4) << std::setfill(' ');
    for (int r = 0; r < 4; ++r) {
        std::cout << "  [ ";
        for (int c = 0; c < 4; ++c) {
            std::cout << std::setw(9) << m[r][c] << " ";
        }
        std::cout << "]\n";
    }
}

Vec3 calculateNewellNormal(const std::vector<Vec3>& polygon) {
    Vec3 normal(0.0, 0.0, 0.0);
    size_t n = polygon.size();
    if (n < 3) return normal;

    for (size_t i = 0; i < n; ++i) {
        const Vec3& cur = polygon[i];
        const Vec3& next = polygon[(i + 1) % n];

        normal.x += (cur.y - next.y) * (cur.z + next.z);
        normal.y += (cur.z - next.z) * (cur.x + next.x);
        normal.z += (cur.x - next.x) * (cur.y + next.y);
    }
    return normalize(normal);
}

void printVec3(const std::string& label, const Vec3& v) {
    std::cout << std::fixed << std::setprecision(4) << std::setfill(' ');
    std::cout << label << " = (" << v.x << ", " << v.y << ", " << v.z << ")\n";
}

void printVec4(const std::string& label, const Vec4& v) {
    std::cout << std::fixed << std::setprecision(4) << std::setfill(' ');
    std::cout << label << " = (" << v.x << ", " << v.y << ", " << v.z << ", w=" << v.w << ")\n";
}

} // namespace cg
