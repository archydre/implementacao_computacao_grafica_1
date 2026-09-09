#include <iostream>
#include <vector>
#include <cassert>
#include <cmath>
#include <iomanip>
#include "math.hpp"

// Re-declarar protótipos das funções de retas para teste direto
struct Point2D { int x, y; };

std::vector<Point2D> lineAnalytical(int x1, int y1, int x2, int y2) {
    std::vector<Point2D> pts;
    int dx = x2 - x1;
    int dy = y2 - y1;
    if (dx == 0) {
        int sy = (y1 <= y2) ? 1 : -1;
        for (int y = y1; y != y2 + sy; y += sy) pts.push_back({x1, y});
        return pts;
    }
    double m = static_cast<double>(dy) / static_cast<double>(dx);
    double b = static_cast<double>(y1) - m * static_cast<double>(x1);
    if (std::abs(m) <= 1.0) {
        int sx = (x1 <= x2) ? 1 : -1;
        for (int x = x1; x != x2 + sx; x += sx) {
            pts.push_back({x, static_cast<int>(std::round(m * x + b))});
        }
    } else {
        int sy = (y1 <= y2) ? 1 : -1;
        for (int y = y1; y != y2 + sy; y += sy) {
            pts.push_back({static_cast<int>(std::round((y - b) / m)), y});
        }
    }
    return pts;
}

std::vector<Point2D> lineDDA(int x1, int y1, int x2, int y2) {
    std::vector<Point2D> pts;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = std::max(std::abs(dx), std::abs(dy));
    if (steps == 0) { pts.push_back({x1, y1}); return pts; }
    double xInc = static_cast<double>(dx) / static_cast<double>(steps);
    double yInc = static_cast<double>(dy) / static_cast<double>(steps);
    double x = x1;
    double y = y1;
    for (int i = 0; i <= steps; ++i) {
        pts.push_back({static_cast<int>(std::round(x)), static_cast<int>(std::round(y))});
        x += xInc;
        y += yInc;
    }
    return pts;
}

std::vector<Point2D> lineBresenham(int x1, int y1, int x2, int y2) {
    std::vector<Point2D> pts;
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    int x = x1;
    int y = y1;
    while (true) {
        pts.push_back({x, y});
        if (x == x2 && y == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx)  { err += dx; y += sy; }
    }
    return pts;
}

void testQuestao1() {
    std::cout << "\n========================================================\n";
    std::cout << "TESTE AUTOMATIZADO - QUESTAO 1: TRACADO DE RETAS\n";
    std::cout << "========================================================\n";

    struct TestCase {
        int x1, y1, x2, y2;
        std::string desc;
    };

    std::vector<TestCase> cases = {
        {10, 10, 100, 50,  "(10,10) -> (100,50)  [0 < m < 1]"},
        {10, 10, 50, 100,  "(10,10) -> (50,100)  [m > 1]"},
        {100, 50, 10, 10,  "(100,50) -> (10,10)  [Sentido invertido]"},
        {50, 10, 50, 100,  "(50,10) -> (50,100)  [Vertical dx=0]"},
        {10, 50, 100, 50,  "(10,50) -> (100,50)  [Horizontal dy=0]"},
        {10, 100, 100, 10, "(10,100) -> (100,10) [Inclinacao negativa]"}
    };

    std::cout << std::left << std::setw(32) << "Caso de Teste"
              << std::setw(12) << "Analitico"
              << std::setw(10) << "DDA"
              << std::setw(12) << "Bresenham"
              << "Status Extremos\n";
    std::cout << std::string(75, '-') << "\n";

    for (const auto& tc : cases) {
        auto pAnalitico = lineAnalytical(tc.x1, tc.y1, tc.x2, tc.y2);
        auto pDDA = lineDDA(tc.x1, tc.y1, tc.x2, tc.y2);
        auto pBres = lineBresenham(tc.x1, tc.y1, tc.x2, tc.y2);

        // Validar primeiro e último ponto
        assert(pAnalitico.front().x == tc.x1 && pAnalitico.front().y == tc.y1);
        assert(pAnalitico.back().x == tc.x2 && pAnalitico.back().y == tc.y2);

        assert(pDDA.front().x == tc.x1 && pDDA.front().y == tc.y1);
        assert(pDDA.back().x == tc.x2 && pDDA.back().y == tc.y2);

        assert(pBres.front().x == tc.x1 && pBres.front().y == tc.y1);
        assert(pBres.back().x == tc.x2 && pBres.back().y == tc.y2);

        std::cout << std::left << std::setw(32) << tc.desc
                  << std::setw(12) << pAnalitico.size()
                  << std::setw(10) << pDDA.size()
                  << std::setw(12) << pBres.size()
                  << "OK (P1 e P2 exatos)\n";
    }
    std::cout << ">>> QUESTAO 1: Todos os testes passaram com sucesso! <<<\n";
}

void testQuestao2() {
    std::cout << "\n========================================================\n";
    std::cout << "TESTE AUTOMATIZADO - QUESTAO 2: TRANSFORMACOES\n";
    std::cout << "========================================================\n";

    cg::Vec3 A(0, 0, 0);
    cg::Vec3 B(30, 5, -5);
    cg::Vec3 C(25, 5, -20);
    cg::Vec3 D(5, 0, -25);
    cg::Vec3 E(15, 20, 12);

    // 1. Teste do produto misto
    cg::Vec3 vAD = A - D;
    cg::Vec3 vBD = B - D;
    cg::Vec3 vCD = C - D;
    double trip = cg::scalarTripleProduct(vAD, vBD, vCD);
    std::cout << "1. Produto misto (A-D) . ((B-D) x (C-D)) = " << trip << "\n";
    assert(std::abs(trip - 1000.0) < 1e-4);
    std::cout << "   -> Inconsistencia matematicamente comprovada (trip == 1000 != 0).\n";

    // 2. Newell
    std::vector<cg::Vec3> base = {A, B, C, D};
    cg::Vec3 newell = cg::calculateNewellNormal(base);
    std::cout << "2. Normal de Newell = (" << newell.x << ", " << newell.y << ", " << newell.z << ")\n";
    assert(std::abs(cg::length(newell) - 1.0) < 1e-5);

    // 3. Translacao D -> Origem
    cg::Vec3 T(-D.x, -D.y, -D.z);
    cg::Mat4 matT = cg::Mat4::translation(T.x, T.y, T.z);
    cg::Vec4 D_trans = matT * D;
    assert(std::abs(D_trans.x) < 1e-6 && std::abs(D_trans.y) < 1e-6 && std::abs(D_trans.z) < 1e-6);
    std::cout << "3. D transladado para origem: (" << D_trans.x << ", " << D_trans.y << ", " << D_trans.z << ") OK\n";

    // 4. Rotacao para alinhar Newell com -Y (para garantir E.y < 0)
    cg::Vec3 target(0.0, -1.0, 0.0);
    cg::Vec3 axis = cg::normalize(cg::cross(newell, target));
    double dotVal = cg::clamp(cg::dot(newell, target), -1.0, 1.0);
    double angleRad = std::acos(dotVal);
    cg::Mat4 matR = cg::Mat4::rotation(angleRad, axis);

    cg::Mat4 M = matR * matT;
    cg::Vec4 D_final = M * D;
    cg::Vec4 E_final = M * E;
    cg::Vec4 A_final = M * A;
    cg::Vec4 B_final = M * B;
    cg::Vec4 C_final = M * C;

    std::cout << "4. Coordenadas finais calculadas por M = R * T:\n";
    std::cout << "   D_final: (" << D_final.x << ", " << D_final.y << ", " << D_final.z << ")\n";
    std::cout << "   E_final: (" << E_final.x << ", " << E_final.y << ", " << E_final.z << ")\n";
    std::cout << "   Residuos de Y da base ABCD: A=" << A_final.y << ", B=" << B_final.y << ", C=" << C_final.y << ", D=" << D_final.y << "\n";

    assert(std::abs(D_final.x) < 1e-4 && std::abs(D_final.y) < 1e-4 && std::abs(D_final.z) < 1e-4);
    assert(E_final.y < 0.0);
    std::cout << ">>> QUESTAO 2: Todos os requisitos matematicos validados com sucesso! <<<\n";
}

void testQuestao3() {
    std::cout << "\n========================================================\n";
    std::cout << "TESTE AUTOMATIZADO - QUESTAO 3: PROJECOES\n";
    std::cout << "========================================================\n";

    cg::Vec3 pt(20.0, 30.0, 40.0);

    // 1. Ortográfica
    cg::Mat4 mOrtho = cg::Mat4::orthoXY();
    cg::Vec4 pOrtho = mOrtho * pt;
    std::cout << "1. Ortografica: Original(20,30,40) -> (" << pOrtho.x << ", " << pOrtho.y << ", " << pOrtho.z << ")\n";
    assert(std::abs(pOrtho.x - 20.0) < 1e-5);
    assert(std::abs(pOrtho.y - 30.0) < 1e-5);
    assert(std::abs(pOrtho.z - 0.0) < 1e-5);

    // 2. Cavaleira (l=1.0, alpha=45)
    cg::Mat4 mCav = cg::Mat4::cavaliere(cg::radians(45.0));
    cg::Vec4 pCav = mCav * pt;
    double expXCav = 20.0 + 40.0 * 1.0 * std::cos(cg::radians(45.0));
    double expYCav = 30.0 + 40.0 * 1.0 * std::sin(cg::radians(45.0));
    std::cout << "2. Cavaleira: Projetado(" << pCav.x << ", " << pCav.y << ")\n";
    assert(std::abs(pCav.x - expXCav) < 1e-4);
    assert(std::abs(pCav.y - expYCav) < 1e-4);

    // 3. Cabinet (l=0.5, alpha=45)
    cg::Mat4 mCab = cg::Mat4::cabinet(cg::radians(45.0));
    cg::Vec4 pCab = mCab * pt;
    double expXCab = 20.0 + 40.0 * 0.5 * std::cos(cg::radians(45.0));
    double expYCab = 30.0 + 40.0 * 0.5 * std::sin(cg::radians(45.0));
    std::cout << "3. Cabinet: Projetado(" << pCab.x << ", " << pCab.y << ")\n";
    assert(std::abs(pCab.x - expXCab) < 1e-4);
    assert(std::abs(pCab.y - expYCab) < 1e-4);

    // 4. Perspectiva 1 ponto (fz = 100)
    cg::Mat4 mPersp1 = cg::Mat4::perspective(0.0, 0.0, 100.0);
    cg::Vec4 pPersp1 = mPersp1 * pt;
    double expW1 = 1.0 - 40.0 / 100.0; // 0.6
    assert(std::abs(pPersp1.w - expW1) < 1e-5);
    double xp1 = pPersp1.x / pPersp1.w;
    double yp1 = pPersp1.y / pPersp1.w;
    std::cout << "4. Perspectiva 1 Ponto: w=" << pPersp1.w << " -> Projetado(" << xp1 << ", " << yp1 << ")\n";
    assert(std::abs(xp1 - (20.0 / 0.6)) < 1e-4);
    assert(std::abs(yp1 - (30.0 / 0.6)) < 1e-4);

    std::cout << ">>> QUESTAO 3: Todas as matrizes e divisoes homogeneas validadas com sucesso! <<<\n";
}

int main() {
    std::cout << "INICIANDO BATERIA DE TESTES MATEMATICOS DE VALIDACAO...\n";
    testQuestao1();
    testQuestao2();
    testQuestao3();
    std::cout << "\n========================================================\n";
    std::cout << "TODOS OS TESTES DE VALIDACAO PASSARAM COM 100% DE SUCESSO!\n";
    std::cout << "========================================================\n";
    return 0;
}
