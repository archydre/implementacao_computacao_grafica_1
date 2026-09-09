#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <string>
#include <sstream>
#include <algorithm>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "math.hpp"

// Tipos de Projeção solicitados
enum ProjectionType {
    PROJ_ORTHO = 1,
    PROJ_CABINET = 2,
    PROJ_CAVALIERE = 3,
    PROJ_PERSPECTIVE_1 = 4,
    PROJ_PERSPECTIVE_2 = 5,
    PROJ_PERSPECTIVE_3 = 6
};

// Estruturas de dados genéricas para poliedro
struct Edge {
    int a;
    int b;
};

struct Polyhedron {
    std::string name;
    std::vector<cg::Vec3> vertices;
    std::vector<Edge> edges;
};

// Estado Global
static Polyhedron g_polyhedron;
static ProjectionType g_currentProj = PROJ_ORTHO;

// Parâmetros de Projeção
static double g_alphaDeg = 45.0; // Ângulo de inclinação oblíqua
static double g_focalDist = 120.0; // Distância focal para fuga (fx, fy, fz)

static int g_windowWidth = 800;
static int g_windowHeight = 600;

// Estrutura para armazenar o vértice projetado
struct ProjectedVertex {
    cg::Vec4 homog;
    cg::Vec3 proj2D; // x_p, y_p, z_p
    bool valid{true};
};

static std::vector<ProjectedVertex> g_projectedVertices;
static cg::Mat4 g_activeMatrix;

// -------------------------------------------------------------
// Modelos Embutidos de Demonstração
// -------------------------------------------------------------
Polyhedron createDemoCube() {
    Polyhedron poly;
    poly.name = "Cubo de Demonstracao (30x30x30)";

    // Vértices de um cubo deslocado da origem para facilitar visualização de perspectiva
    // V0..V3: Face frontal (z = 0)
    // V4..V7: Face traseira (z = 30)
    poly.vertices = {
        { 5.0,  5.0,  5.0}, // 0
        {35.0,  5.0,  5.0}, // 1
        {35.0, 35.0,  5.0}, // 2
        { 5.0, 35.0,  5.0}, // 3
        { 5.0,  5.0, 35.0}, // 4
        {35.0,  5.0, 35.0}, // 5
        {35.0, 35.0, 35.0}, // 6
        { 5.0, 35.0, 35.0}  // 7
    };

    // 12 Arestas
    poly.edges = {
        // Face frontal
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        // Face traseira
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        // Arestas de conexão
        {0, 4}, {1, 5}, {2, 6}, {3, 7}
    };

    return poly;
}

Polyhedron createHouseModel() {
    Polyhedron poly;
    poly.name = "Casinha 3D (Cubo + Piramide no Teto)";
    poly.vertices = {
        { 5.0,  5.0,  5.0}, // 0
        {35.0,  5.0,  5.0}, // 1
        {35.0, 30.0,  5.0}, // 2
        { 5.0, 30.0,  5.0}, // 3
        { 5.0,  5.0, 35.0}, // 4
        {35.0,  5.0, 35.0}, // 5
        {35.0, 30.0, 35.0}, // 6
        { 5.0, 30.0, 35.0}, // 7
        {20.0, 45.0, 20.0}  // 8: Topo do telhado
    };
    poly.edges = {
        // Cubo base
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 5}, {5, 6}, {6, 7}, {7, 4},
        {0, 4}, {1, 5}, {2, 6}, {3, 7},
        // Telhado
        {2, 8}, {3, 8}, {6, 8}, {7, 8}
    };
    return poly;
}

Polyhedron createQuestao2Model() {
    Polyhedron poly;
    poly.name = "Poliedro da Questao 2 (Piramide irregular)";
    poly.vertices = {
        { 0.0,  0.0,   0.0}, // A
        {30.0,  5.0,  -5.0}, // B
        {25.0,  5.0, -20.0}, // C
        { 5.0,  0.0, -25.0}, // D
        {15.0, 20.0,  12.0}  // E
    };
    poly.edges = {
        {0, 1}, {1, 2}, {2, 3}, {3, 0},
        {4, 0}, {4, 1}, {4, 2}, {4, 3}
    };
    return poly;
}

// -------------------------------------------------------------
// Matrizes de Projeção Canônicas
// -------------------------------------------------------------
cg::Mat4 buildProjectionMatrix(ProjectionType type) {
    double alphaRad = cg::radians(g_alphaDeg);

    switch (type) {
        case PROJ_ORTHO:
            // 3.1 Paralela Ortográfica no plano XY (z' = 0)
            return cg::Mat4::orthoXY();

        case PROJ_CABINET:
            // 3.4 Cabinet: tan(beta) = 2 => l = 0.5, alpha = 45 graus
            return cg::Mat4::cabinet(alphaRad);

        case PROJ_CAVALIERE:
            // 3.3 Cavaleira: tan(beta) = 1 => l = 1.0, alpha = 45 graus
            return cg::Mat4::cavaliere(alphaRad);

        case PROJ_PERSPECTIVE_1:
            // 3.5 Perspectiva de 1 ponto de fuga (fz finito, fx = fy = inf)
            return cg::Mat4::perspective(0.0, 0.0, g_focalDist);

        case PROJ_PERSPECTIVE_2:
            // 3.6 Perspectiva de 2 pontos de fuga (fx e fz finitos, fy = inf)
            return cg::Mat4::perspective(g_focalDist, 0.0, g_focalDist);

        case PROJ_PERSPECTIVE_3:
            // 3.7 Perspectiva de 3 pontos de fuga (fx, fy, fz finitos)
            return cg::Mat4::perspective(g_focalDist, g_focalDist, g_focalDist);
    }
    return cg::Mat4::identity();
}

std::string getProjectionName(ProjectionType type) {
    switch (type) {
        case PROJ_ORTHO:         return "1 - Paralela Ortografica (Plano XY)";
        case PROJ_CABINET:       return "2 - Paralela Obliqua Cabinet (l = 0.5, alpha = 45 deg)";
        case PROJ_CAVALIERE:     return "3 - Paralela Obliqua Cavaleira (l = 1.0, alpha = 45 deg)";
        case PROJ_PERSPECTIVE_1: return "4 - Perspectiva 1 Ponto de Fuga (Eixo Z, fz = " + std::to_string(static_cast<int>(g_focalDist)) + ")";
        case PROJ_PERSPECTIVE_2: return "5 - Perspectiva 2 Pontos de Fuga (Eixos X e Z, f = " + std::to_string(static_cast<int>(g_focalDist)) + ")";
        case PROJ_PERSPECTIVE_3: return "6 - Perspectiva 3 Pontos de Fuga (Eixos X, Y e Z, f = " + std::to_string(static_cast<int>(g_focalDist)) + ")";
    }
    return "Desconhecida";
}

// -------------------------------------------------------------
// Cálculo de Projeção e Exibição no Terminal
// -------------------------------------------------------------
void updateProjectionAndPrint() {
    g_activeMatrix = buildProjectionMatrix(g_currentProj);
    g_projectedVertices.resize(g_polyhedron.vertices.size());

    std::string projName = getProjectionName(g_currentProj);

    std::cout << "\n--- Projecao: " << projName << " ---\n";
    std::cout << "Modelo: " << g_polyhedron.name << " ("
              << g_polyhedron.vertices.size() << " vertices, "
              << g_polyhedron.edges.size() << " arestas)\n\n";

    g_activeMatrix.print("Matriz 4x4");

    std::cout << "\nVertices: Original -> Homogeneo -> Projetado:\n";
    std::cout << std::fixed << std::setprecision(2);

    for (size_t i = 0; i < g_polyhedron.vertices.size(); ++i) {
        const auto& orig = g_polyhedron.vertices[i];
        cg::Vec4 homog = g_activeMatrix * orig;
        ProjectedVertex pv;
        pv.homog = homog;

        // Divisão homogênea por w com proteção contra singularidade
        if (std::abs(homog.w) < 1e-6) {
            pv.valid = false;
            pv.proj2D = {0.0, 0.0, 0.0};
            std::cout << "  V" << std::setw(2) << std::setfill('0') << i << std::setfill(' ')
                      << ": (" << std::setw(5) << orig.x << "," << std::setw(5) << orig.y << "," << std::setw(5) << orig.z << ")"
                      << " -> Homog[" << std::setw(5) << homog.x << "," << std::setw(5) << homog.y << ", w=" << homog.w << "]"
                      << " -> [SINGULARIDADE: w ~ 0]\n";
        } else {
            pv.valid = true;
            pv.proj2D.x = homog.x / homog.w;
            pv.proj2D.y = homog.y / homog.w;
            pv.proj2D.z = homog.z / homog.w;

            std::cout << "  V" << std::setw(2) << std::setfill('0') << i << std::setfill(' ')
                      << ": (" << std::setw(5) << orig.x << "," << std::setw(5) << orig.y << "," << std::setw(5) << orig.z << ")"
                      << " -> Homog[" << std::setw(5) << homog.x << "," << std::setw(5) << homog.y << ", w=" << std::setw(5) << homog.w << "]"
                      << " -> Proj(" << std::setw(6) << pv.proj2D.x << "," << std::setw(6) << pv.proj2D.y << ")\n";
        }
        g_projectedVertices[i] = pv;
    }

    std::cout << "----------------------------------------------------------------------\n";

    std::string title = "Questao 3 - Projeções [" + projName.substr(4) + "]";
    glutSetWindowTitle(title.c_str());
}

// -------------------------------------------------------------
// Viewport Auto-fit e Renderização 2D
// -------------------------------------------------------------
void renderBitmapText2D(float x, float y, void* font, const std::string& str) {
    glRasterPos2f(x, y);
    for (char c : str) {
        glutBitmapCharacter(font, c);
    }
}

void display() {
    glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 1. Ajuste à Tela / Viewport (Independente da matemática de projeção)
    // Conforme enunciado: "Esse ajuste deve acontecer depois da projeção e não deve
    // modificar a matriz matemática sendo demonstrada."
    double minX = 1e9, maxX = -1e9;
    double minY = 1e9, maxY = -1e9;
    bool anyValid = false;

    for (const auto& pv : g_projectedVertices) {
        if (!pv.valid) continue;
        minX = std::min(minX, pv.proj2D.x);
        maxX = std::max(maxX, pv.proj2D.x);
        minY = std::min(minY, pv.proj2D.y);
        maxY = std::max(maxY, pv.proj2D.y);
        anyValid = true;
    }

    if (!anyValid) {
        minX = -50.0; maxX = 50.0;
        minY = -50.0; maxY = 50.0;
    }

    double spanX = std::max(1.0, maxX - minX);
    double spanY = std::max(1.0, maxY - minY);
    double centerX = (minX + maxX) * 0.5;
    double centerY = (minY + maxY) * 0.5;

    // Manter aspecto 1:1 e aplicar margem generosa (30%)
    double halfSpan = std::max(spanX, spanY) * 0.65;
    if (halfSpan < 5.0) halfSpan = 5.0;

    double left   = centerX - halfSpan;
    double right  = centerX + halfSpan;
    double bottom = centerY - halfSpan;
    double top    = centerY + halfSpan;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(left, right, bottom, top);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 2. Desenhar Eixos 2D de Referência (em cinza escuro)
    glColor3f(0.28f, 0.28f, 0.32f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    glVertex2d(left, 0.0);
    glVertex2d(right, 0.0);
    glVertex2d(0.0, bottom);
    glVertex2d(0.0, top);
    glEnd();

    // 3. Desenhar Arestas Projetadas do Poliedro (Ciano brilhante)
    glLineWidth(2.0f);
    glColor3f(0.2f, 0.9f, 1.0f);
    glBegin(GL_LINES);
    for (const auto& edge : g_polyhedron.edges) {
        if (edge.a >= 0 && edge.a < static_cast<int>(g_projectedVertices.size()) &&
            edge.b >= 0 && edge.b < static_cast<int>(g_projectedVertices.size())) {
            const auto& p1 = g_projectedVertices[edge.a];
            const auto& p2 = g_projectedVertices[edge.b];
            if (p1.valid && p2.valid) {
                glVertex2d(p1.proj2D.x, p1.proj2D.y);
                glVertex2d(p2.proj2D.x, p2.proj2D.y);
            }
        }
    }
    glEnd();

    // 4. Desenhar Vértices Projetados (Pontos amarelos)
    glPointSize(7.0f);
    glColor3f(1.0f, 0.85f, 0.2f);
    glBegin(GL_POINTS);
    for (const auto& pv : g_projectedVertices) {
        if (pv.valid) {
            glVertex2d(pv.proj2D.x, pv.proj2D.y);
        }
    }
    glEnd();

    // 5. Identificação dos Vértices na tela
    glColor3f(1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < g_projectedVertices.size(); ++i) {
        const auto& pv = g_projectedVertices[i];
        if (pv.valid) {
            float labelOffset = static_cast<float>(halfSpan * 0.035);
            renderBitmapText2D(static_cast<float>(pv.proj2D.x) + labelOffset,
                               static_cast<float>(pv.proj2D.y) + labelOffset,
                               GLUT_BITMAP_HELVETICA_10,
                               "V" + std::to_string(i));
        }
    }

    // 6. HUD / Legenda de Controle (fixo na janela)
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, g_windowWidth, 0, g_windowHeight);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0.95f, 0.95f, 0.95f);
    renderBitmapText2D(20, g_windowHeight - 25, GLUT_BITMAP_HELVETICA_12,
                       "Projecao: " + getProjectionName(g_currentProj));
    renderBitmapText2D(20, g_windowHeight - 45, GLUT_BITMAP_HELVETICA_12,
                       "Modelo: " + g_polyhedron.name + "  |  Foco f = " + std::to_string(static_cast<int>(g_focalDist)));
    renderBitmapText2D(20, 20, GLUT_BITMAP_HELVETICA_12,
                       "Teclas: [1] Ortografica  [2] Cabinet  [3] Cavaleira  [4] Persp 1pt  [5] Persp 2pt  [6] Persp 3pt  [C] Modelo  [+/-] Foco  [Q] Sair");

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    g_windowWidth = w;
    g_windowHeight = (h > 0) ? h : 1;
    glViewport(0, 0, g_windowWidth, g_windowHeight);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case '1':
            g_currentProj = PROJ_ORTHO;
            updateProjectionAndPrint();
            glutPostRedisplay();
            break;
        case '2':
            g_currentProj = PROJ_CABINET;
            updateProjectionAndPrint();
            glutPostRedisplay();
            break;
        case '3':
            g_currentProj = PROJ_CAVALIERE;
            updateProjectionAndPrint();
            glutPostRedisplay();
            break;
        case '4':
            g_currentProj = PROJ_PERSPECTIVE_1;
            updateProjectionAndPrint();
            glutPostRedisplay();
            break;
        case '5':
            g_currentProj = PROJ_PERSPECTIVE_2;
            updateProjectionAndPrint();
            glutPostRedisplay();
            break;
        case '6':
            g_currentProj = PROJ_PERSPECTIVE_3;
            updateProjectionAndPrint();
            glutPostRedisplay();
            break;
        case 'c':
        case 'C': {
            static int modelIdx = 0;
            modelIdx = (modelIdx + 1) % 3;
            if (modelIdx == 0) g_polyhedron = createDemoCube();
            else if (modelIdx == 1) g_polyhedron = createHouseModel();
            else g_polyhedron = createQuestao2Model();
            std::cout << "\n>>> Alternado para modelo: " << g_polyhedron.name << " <<<\n";
            updateProjectionAndPrint();
            glutPostRedisplay();
            break;
        }
        case '+':
            g_focalDist += 20.0;
            std::cout << "Nova distancia focal f = " << g_focalDist << "\n";
            updateProjectionAndPrint();
            glutPostRedisplay();
            break;
        case '-':
            if (g_focalDist > 40.0) {
                g_focalDist -= 20.0;
            }
            std::cout << "Nova distancia focal f = " << g_focalDist << "\n";
            updateProjectionAndPrint();
            glutPostRedisplay();
            break;
        case 27: // ESC não fecha o comando
            std::cout << "[ESC] Pressionado (para sair use 'Q' ou feche a janela)\n";
            break;
        case 'q':
        case 'Q':
            std::cout << "Encerrando questao 3...\n";
            std::exit(0);
            break;
    }
}

int main(int argc, char** argv) {
    std::cout << "======================================================================\n";
    std::cout << "            QUESTAO 3 - PROJECOES GEOMETRICAS (OpenGL)                \n";
    std::cout << "======================================================================\n";
    std::cout << "Escolha o poliedro inicial:\n";
    std::cout << "  1 - Digitar poliedro customizado\n";
    std::cout << "  2 - Usar Cubo de demonstracao (Padrao)\n";
    std::cout << "  3 - Usar Casinha 3D\n";
    std::cout << "  4 - Usar Poliedro da Questao 2\n";
    std::cout << "Opcao (pressione ENTER para opcao 2): ";

    std::string line;
    int opt = 2;
    if (std::getline(std::cin, line) && !line.empty()) {
        std::stringstream ss(line);
        ss >> opt;
    }

    if (opt == 1) {
        Polyhedron customPoly;
        customPoly.name = "Poliedro Customizado";
        std::cout << "\nNumero de vertices: ";
        int nV;
        if (std::cin >> nV && nV > 0) {
            std::cout << "Digite x y z para cada um dos " << nV << " vertices:\n";
            for (int i = 0; i < nV; ++i) {
                double x, y, z;
                std::cout << "Vertice " << i << " (x y z): ";
                std::cin >> x >> y >> z;
                customPoly.vertices.push_back({x, y, z});
            }
            std::cout << "Numero de arestas: ";
            int nE;
            if (std::cin >> nE && nE >= 0) {
                std::cout << "Digite i1 i2 (indices de 0 a " << (nV - 1) << ") para cada aresta:\n";
                for (int i = 0; i < nE; ++i) {
                    int a, b;
                    std::cout << "Aresta " << i << " (i1 i2): ";
                    std::cin >> a >> b;
                    customPoly.edges.push_back({a, b});
                }
            }
            g_polyhedron = customPoly;
        } else {
            std::cout << "Entrada invalida. Carregando Cubo de demonstracao.\n";
            g_polyhedron = createDemoCube();
        }
    } else if (opt == 3) {
        g_polyhedron = createHouseModel();
    } else if (opt == 4) {
        g_polyhedron = createQuestao2Model();
    } else {
        g_polyhedron = createDemoCube();
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_windowWidth, g_windowHeight);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Questao 3 - Projecoes Geometricas");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    updateProjectionAndPrint();

    glutMainLoop();
    return 0;
}
