#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <string>
#include <cstdlib>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "math.hpp"

// Modo de visualização
enum DisplayMode {
    MODE_BOTH,
    MODE_ORIGINAL,
    MODE_TRANSFORMED
};

static DisplayMode g_mode = MODE_BOTH;

// Coordenadas originais obrigatórias do enunciado
static const cg::Vec3 origA(0.0, 0.0, 0.0);
static const cg::Vec3 origB(30.0, 5.0, -5.0);
static const cg::Vec3 origC(25.0, 5.0, -20.0);
static const cg::Vec3 origD(5.0, 0.0, -25.0);
static const cg::Vec3 origE(15.0, 20.0, 12.0);

static const std::vector<cg::Vec3> g_origVertices = {origA, origB, origC, origD, origE};
static const std::vector<std::string> g_vertexNames = {"A", "B", "C", "D", "E"};

// Arestas do sólido conforme enunciado
// AB, BC, CD, DA, EA, EB, EC, ED
static const std::vector<std::pair<int, int>> g_edges = {
    {0, 1}, // AB
    {1, 2}, // BC
    {2, 3}, // CD
    {3, 0}, // DA
    {4, 0}, // EA
    {4, 1}, // EB
    {4, 2}, // EC
    {4, 3}  // ED
};

// Parâmetros calculados da transformação
static cg::Vec3 g_transVec;      // Vetor de translação (-5, 0, 25)
static double g_rotAngleDeg;     // Ângulo de rotação em graus
static cg::Vec3 g_rotAxis;       // Eixo de rotação unitário
static cg::Mat4 g_matrixFinal;   // Matriz M = R * T (CPU)
static std::vector<cg::Vec3> g_finalVertices(5);

// Controle da câmera (órbita)
static float g_camYaw = 40.0f;
static float g_camPitch = 25.0f;
static float g_camDist = 110.0f;
static int g_lastMouseX = -1;
static int g_lastMouseY = -1;
static bool g_mouseRotating = false;

// -------------------------------------------------------------
// Cálculo Geométrico e Validação
// -------------------------------------------------------------
void computeTransformations() {
    std::cout << "\n======================================================================\n";
    std::cout << "QUESTAO 2 - TRANSFORMACOES GEOMETRICAS 3D (OpenGL ModelView)\n";
    std::cout << "======================================================================\n";

    // 1. Verificação de Coplanaridade dos vértices da base ABCD
    cg::Vec3 vAD = origA - origD;
    cg::Vec3 vBD = origB - origD;
    cg::Vec3 vCD = origC - origD;
    double tripleProduct = cg::scalarTripleProduct(vAD, vBD, vCD);

    if (std::abs(tripleProduct) > 1e-4) {
        std::cout << "[Coplanaridade] Produto misto = " << tripleProduct
                  << " (A,B,C,D nao sao coplanares; normal Newell aplicado na base)\n";
    }

    // 2. Passo 1: Levar D à origem
    // T = (-Dx, -Dy, -Dz) = (-5, 0, 25)
    g_transVec = cg::Vec3(-origD.x, (std::abs(origD.y) < 1e-9 ? 0.0 : -origD.y), -origD.z);
    cg::Mat4 matT = cg::Mat4::translation(g_transVec.x, g_transVec.y, g_transVec.z);

    // 3. Passo 2: Alinhar plano-base com XZ
    // Calcular normal de Newell para o quadrilátero ABCD
    std::vector<cg::Vec3> basePolygon = {origA, origB, origC, origD};
    cg::Vec3 newellNormal = cg::calculateNewellNormal(basePolygon);

    cg::Vec3 candidates[2] = {cg::Vec3(0.0, 1.0, 0.0), cg::Vec3(0.0, -1.0, 0.0)};
    for (int i = 0; i < 2; ++i) {
        cg::Vec3 target = candidates[i];
        cg::Vec3 axis = cg::cross(newellNormal, target);
        double axisLen = cg::length(axis);
        double dotVal = cg::clamp(cg::dot(newellNormal, target), -1.0, 1.0);
        double angleRad = (axisLen < 1e-6) ? ((dotVal < 0.0) ? cg::PI : 0.0) : std::acos(dotVal);
        cg::Vec3 unitAxis = (axisLen < 1e-6) ? cg::Vec3(1.0, 0.0, 0.0) : (axis / axisLen);

        cg::Mat4 matR = cg::Mat4::rotation(angleRad, unitAxis);
        cg::Mat4 testMat = matR * matT;
        cg::Vec4 testE = testMat * origE;

        // Condição: E.y < 0
        if (testE.y < 0.0) {
            g_rotAngleDeg = cg::degrees(angleRad);
            g_rotAxis = unitAxis;
            g_matrixFinal = testMat;
            break;
        }
    }

    std::cout << "[Translacao T]  (" << g_transVec.x << ", " << g_transVec.y << ", " << g_transVec.z << ") -> Leva D para (0,0,0)\n";
    std::cout << "[Rotacao R]     " << std::fixed << std::setprecision(2) << g_rotAngleDeg << " graus em ("
              << g_rotAxis.x << ", " << g_rotAxis.y << ", " << g_rotAxis.z << ") -> Alinha base e faz E.y < 0\n\n";

    g_matrixFinal.print("Matriz Composta M = R * T (CPU)");
    std::cout << "\nVertices: Original -> Transformado (M * V):\n";
    std::cout << std::fixed << std::setprecision(2);

    for (size_t i = 0; i < g_origVertices.size(); ++i) {
        cg::Vec4 transformed = g_matrixFinal * g_origVertices[i];
        g_finalVertices[i] = transformed.toVec3();
        std::cout << "  " << g_vertexNames[i] << ": ("
                  << std::setw(6) << g_origVertices[i].x << ","
                  << std::setw(6) << g_origVertices[i].y << ","
                  << std::setw(6) << g_origVertices[i].z << ") -> ("
                  << std::setw(7) << g_finalVertices[i].x << ","
                  << std::setw(7) << g_finalVertices[i].y << ","
                  << std::setw(7) << g_finalVertices[i].z << ")";
        if (i < 4) {
            std::cout << " | Residuo Y: " << std::setw(6) << g_finalVertices[i].y << (i == 3 ? " [D na Origem: OK]" : "");
        } else {
            std::cout << " | [E.y < 0: OK (" << g_finalVertices[i].y << ")]";
        }
        std::cout << "\n";
    }
    std::cout << "======================================================================\n\n";
}

// Renderização dos eixos cartesianos
void drawAxes(float length) {
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    // Eixo X - Vermelho
    glColor3f(1.0f, 0.2f, 0.2f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(length, 0.0f, 0.0f);

    // Eixo Y - Verde
    glColor3f(0.2f, 1.0f, 0.2f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, length, 0.0f);

    // Eixo Z - Azul
    glColor3f(0.3f, 0.5f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(0.0f, 0.0f, length);
    glEnd();
}

// Renderiza o wireframe do poliedro a partir dos vértices originais
void drawOriginalWireframe() {
    glLineWidth(1.8f);
    glBegin(GL_LINES);
    for (const auto& edge : g_edges) {
        const cg::Vec3& p1 = g_origVertices[edge.first];
        const cg::Vec3& p2 = g_origVertices[edge.second];
        glVertex3f(static_cast<float>(p1.x), static_cast<float>(p1.y), static_cast<float>(p1.z));
        glVertex3f(static_cast<float>(p2.x), static_cast<float>(p2.y), static_cast<float>(p2.z));
    }
    glEnd();

    // Vértices como pontos
    glPointSize(6.0f);
    glBegin(GL_POINTS);
    for (const auto& v : g_origVertices) {
        glVertex3f(static_cast<float>(v.x), static_cast<float>(v.y), static_cast<float>(v.z));
    }
    glEnd();
}

// Renderiza texto 2D na janela
void renderBitmapText(float x, float y, void* font, const std::string& str) {
    glRasterPos2f(x, y);
    for (char c : str) {
        glutBitmapCharacter(font, c);
    }
}

void display() {
    glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // Configurar projeção 3D perspectiva para a câmera
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 800.0 / 600.0, 1.0, 1000.0);

    // Configurar visão da câmera
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float radYaw = cg::radians(g_camYaw);
    float radPitch = cg::radians(g_camPitch);

    float cx = g_camDist * std::cos(radPitch) * std::sin(radYaw);
    float cy = g_camDist * std::sin(radPitch);
    float cz = g_camDist * std::cos(radPitch) * std::cos(radYaw);

    gluLookAt(cx, cy, cz, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

    // 1. Desenhar Eixos Cartesianos do Mundo (X=Vermelho, Y=Verde, Z=Azul)
    drawAxes(45.0f);

    // 2. Desenhar Poliedro Original (em cinza translúcido/discreto)
    if (g_mode == MODE_ORIGINAL || g_mode == MODE_BOTH) {
        glColor3f(0.55f, 0.55f, 0.60f); // Cinza
        drawOriginalWireframe();
    }

    // 3. Desenhar Poliedro Transformado usando a PILHA DE MATRIZES OPENGL
    // O enunciado exige explicitamente:
    // "A transformação aplicada ao desenho deve ser feita pela matriz OpenGL/model-view,
    // por exemplo com: glTranslatef(...); glRotatef(...);"
    // "Não simplesmente substituir os vértices originais por coordenadas finais calculadas à mão."
    if (g_mode == MODE_TRANSFORMED || g_mode == MODE_BOTH) {
        glPushMatrix();

        // Aplicação da sequência geométrica solicitada:
        // Ordem de chamadas OpenGL (a última chamada é aplicada primeiro ao vértice):
        // 1. glTranslatef leva D para a origem (v' = v + T)
        // 2. glRotatef alinha o plano da base com XZ (v'' = R * v')
        glRotatef(static_cast<float>(g_rotAngleDeg),
                  static_cast<float>(g_rotAxis.x),
                  static_cast<float>(g_rotAxis.y),
                  static_cast<float>(g_rotAxis.z));

        glTranslatef(static_cast<float>(g_transVec.x),
                     static_cast<float>(g_transVec.y),
                     static_cast<float>(g_transVec.z));

        // Desenha usando exatamente os vértices ORIGINAIS!
        // O pipeline OpenGL aplica R e T na placa/pipeline clássico.
        glColor3f(0.2f, 1.0f, 0.4f); // Verde brilhante para o transformado
        drawOriginalWireframe();

        glPopMatrix();
    }

    // 4. Desenhar HUD / Legenda 2D
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0.95f, 0.95f, 0.95f);
    renderBitmapText(20, 575, GLUT_BITMAP_HELVETICA_12,
                     "Questao 2 - Transformacoes Geometricas (OpenGL ModelView)");

    std::string modeStr = (g_mode == MODE_BOTH)        ? "Ambos (Cinza: Original | Verde: Transformado)" :
                          (g_mode == MODE_ORIGINAL)    ? "Apenas Original" : "Apenas Transformado";
    renderBitmapText(20, 555, GLUT_BITMAP_HELVETICA_12, "Modo de Exibicao: " + modeStr);

    renderBitmapText(20, 535, GLUT_BITMAP_HELVETICA_12,
                     "D final: (" + std::to_string(g_finalVertices[3].x).substr(0, 4) + ", "
                                  + std::to_string(g_finalVertices[3].y).substr(0, 4) + ", "
                                  + std::to_string(g_finalVertices[3].z).substr(0, 4) + ")  |  "
                     "E.y final: " + std::to_string(g_finalVertices[4].y).substr(0, 6) + " (< 0 OK)");

    renderBitmapText(20, 25, GLUT_BITMAP_HELVETICA_12,
                     "Teclas: [O] Original  [T] Transformado  [A] Ambos  [Arrastar Mouse] Rotacionar Camera  [Q] Sair");

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, (h > 0) ? h : 1);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
        case 'o':
        case 'O':
            g_mode = MODE_ORIGINAL;
            glutPostRedisplay();
            break;
        case 't':
        case 'T':
            g_mode = MODE_TRANSFORMED;
            glutPostRedisplay();
            break;
        case 'a':
        case 'A':
            g_mode = MODE_BOTH;
            glutPostRedisplay();
            break;
        case '+':
            g_camDist = std::max(20.0f, g_camDist - 5.0f);
            glutPostRedisplay();
            break;
        case '-':
            g_camDist = std::min(400.0f, g_camDist + 5.0f);
            glutPostRedisplay();
            break;
        case 27: // ESC não fecha o comando
            std::cout << "[ESC] Pressionado (para sair use 'Q' ou feche a janela)\n";
            break;
        case 'q':
        case 'Q':
            std::cout << "Encerrando questao 2...\n";
            std::exit(0);
            break;
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            g_mouseRotating = true;
            g_lastMouseX = x;
            g_lastMouseY = y;
        } else {
            g_mouseRotating = false;
        }
    }
}

void motion(int x, int y) {
    if (g_mouseRotating) {
        float dx = static_cast<float>(x - g_lastMouseX);
        float dy = static_cast<float>(y - g_lastMouseY);

        g_camYaw += dx * 0.5f;
        g_camPitch += dy * 0.5f;

        if (g_camPitch > 85.0f) g_camPitch = 85.0f;
        if (g_camPitch < -85.0f) g_camPitch = -85.0f;

        g_lastMouseX = x;
        g_lastMouseY = y;

        glutPostRedisplay();
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(150, 150);
    glutCreateWindow("Questao 2 - Transformacoes Geometricas");

    computeTransformations();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glutMainLoop();
    return 0;
}
