#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <string>
#include <sstream>
#include <algorithm>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

// Estrutura de ponto 2D discreto (pixel)
struct Point2D {
    int x;
    int y;
};

// Algoritmos disponíveis
enum Algorithm {
    ALGO_ANALYTICAL = 1,
    ALGO_DDA = 2,
    ALGO_BRESENHAM = 3
};

// Variáveis globais do estado
static int g_x1 = 10;
static int g_y1 = 10;
static int g_x2 = 100;
static int g_y2 = 50;

static Algorithm g_currentAlgo = ALGO_ANALYTICAL;
static std::vector<Point2D> g_pixels;
static int g_windowWidth = 800;
static int g_windowHeight = 600;

// -------------------------------------------------------------
// 1. Algoritmo da Equação Analítica
// -------------------------------------------------------------
// y = m*x + b  (para |m| <= 1, passo unitário em x)
// x = (y - b)/m (para |m| > 1, passo unitário em y)
// Trata reta vertical separadamente para evitar divisão por zero.
std::vector<Point2D> lineAnalytical(int x1, int y1, int x2, int y2) {
    std::vector<Point2D> pts;

    int dx = x2 - x1;
    int dy = y2 - y1;

    // Caso 1: Reta vertical (dx == 0)
    if (dx == 0) {
        int sy = (y1 <= y2) ? 1 : -1;
        for (int y = y1; y != y2 + sy; y += sy) {
            pts.push_back({x1, y});
        }
        return pts;
    }

    double m = static_cast<double>(dy) / static_cast<double>(dx);
    double b = static_cast<double>(y1) - m * static_cast<double>(x1);

    // Caso 2: |m| <= 1 (variação preponderante em X)
    if (std::abs(m) <= 1.0) {
        int sx = (x1 <= x2) ? 1 : -1;
        for (int x = x1; x != x2 + sx; x += sx) {
            double yVal = m * static_cast<double>(x) + b;
            int yPlot = static_cast<int>(std::round(yVal));
            pts.push_back({x, yPlot});
        }
    }
    // Caso 3: |m| > 1 (variação preponderante em Y)
    else {
        int sy = (y1 <= y2) ? 1 : -1;
        for (int y = y1; y != y2 + sy; y += sy) {
            double xVal = (static_cast<double>(y) - b) / m;
            int xPlot = static_cast<int>(std::round(xVal));
            pts.push_back({xPlot, y});
        }
    }

    return pts;
}

// -------------------------------------------------------------
// 2. Algoritmo DDA (Digital Differential Analyzer)
// -------------------------------------------------------------
std::vector<Point2D> lineDDA(int x1, int y1, int x2, int y2) {
    std::vector<Point2D> pts;

    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = std::max(std::abs(dx), std::abs(dy));

    if (steps == 0) {
        pts.push_back({x1, y1});
        return pts;
    }

    double xInc = static_cast<double>(dx) / static_cast<double>(steps);
    double yInc = static_cast<double>(dy) / static_cast<double>(steps);

    double x = static_cast<double>(x1);
    double y = static_cast<double>(y1);

    for (int i = 0; i <= steps; ++i) {
        pts.push_back({static_cast<int>(std::round(x)), static_cast<int>(std::round(y))});
        x += xInc;
        y += yInc;
    }

    return pts;
}

// -------------------------------------------------------------
// 3. Algoritmo de Bresenham (Somente aritmética inteira)
// -------------------------------------------------------------
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

        if (x == x2 && y == y2) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }

    return pts;
}

// -------------------------------------------------------------
// Impressão e Atualização dos Pixels
// -------------------------------------------------------------
void updatePixelsAndPrint() {
    std::string name;
    switch (g_currentAlgo) {
        case ALGO_ANALYTICAL:
            name = "EQUACAO ANALITICA";
            g_pixels = lineAnalytical(g_x1, g_y1, g_x2, g_y2);
            break;
        case ALGO_DDA:
            name = "DDA";
            g_pixels = lineDDA(g_x1, g_y1, g_x2, g_y2);
            break;
        case ALGO_BRESENHAM:
            name = "BRESENHAM";
            g_pixels = lineBresenham(g_x1, g_y1, g_x2, g_y2);
            break;
    }

    std::cout << "\n--- " << name << " ---\n";
    std::cout << "Reta: (" << g_x1 << ", " << g_y1 << ") -> (" << g_x2 << ", " << g_y2 << ")\n";
    std::cout << "Pixels gerados:\n";

    for (size_t i = 0; i < g_pixels.size(); ++i) {
        std::cout << " [" << std::setw(3) << std::setfill('0') << i << "]("
                  << g_pixels[i].x << "," << g_pixels[i].y << ")" << std::setfill(' ');
        if ((i + 1) % 4 == 0 || i + 1 == g_pixels.size()) {
            std::cout << "\n";
        }
    }

    std::cout << "Total: " << g_pixels.size() << " pixels\n";
    std::cout << "----------------------------------------\n";

    std::string title = "Questao 1 - Tracado de Retas [" + name + "]";
    glutSetWindowTitle(title.c_str());
}

// Renderizar texto na tela OpenGL
void renderBitmapString(float x, float y, void* font, const std::string& str) {
    glRasterPos2f(x, y);
    for (char c : str) {
        glutBitmapCharacter(font, c);
    }
}

// Callback de exibição OpenGL
void display() {
    glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // Determinar limites ortográficos dinâmicos
    int minX = std::min(g_x1, g_x2);
    int maxX = std::max(g_x1, g_x2);
    int minY = std::min(g_y1, g_y2);
    int maxY = std::max(g_y1, g_y2);

    int spanX = std::max(10, maxX - minX);
    int spanY = std::max(10, maxY - minY);

    int marginX = std::max(15, static_cast<int>(spanX * 0.2));
    int marginY = std::max(15, static_cast<int>(spanY * 0.2));

    int orthoLeft = std::min(0, minX) - marginX;
    int orthoRight = std::max(0, maxX) + marginX;
    int orthoBottom = std::min(0, minY) - marginY;
    int orthoTop = std::max(0, maxY) + marginY;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(orthoLeft, orthoRight, orthoBottom, orthoTop);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // 1. Desenhar Eixos Cartesianos (cinza escuro)
    glColor3f(0.35f, 0.35f, 0.40f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    // Eixo X
    glVertex2i(orthoLeft, 0);
    glVertex2i(orthoRight, 0);
    // Eixo Y
    glVertex2i(0, orthoBottom);
    glVertex2i(0, orthoTop);
    glEnd();

    // 2. Desenhar os Pixels da Reta usando GL_POINTS
    // Conforme enunciado: "Os pixels calculados pelo algoritmo devem ser desenhados
    // com OpenGL usando glBegin(GL_POINTS); glVertex2i(...); glEnd();"
    glPointSize(4.0f);
    glColor3f(0.2f, 0.9f, 1.0f); // Ciano brilhante para a reta
    glBegin(GL_POINTS);
    for (const auto& pt : g_pixels) {
        glVertex2i(pt.x, pt.y);
    }
    glEnd();

    // 3. Destaque dos pontos inicial (verde) e final (vermelho)
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    glColor3f(0.2f, 1.0f, 0.2f); // Inicial verde
    glVertex2i(g_x1, g_y1);
    glColor3f(1.0f, 0.3f, 0.3f); // Final vermelho
    glVertex2i(g_x2, g_y2);
    glEnd();

    // 4. Legenda na tela (canto superior esquerdo)
    glColor3f(0.9f, 0.9f, 0.9f);
    float textX = orthoLeft + marginX * 0.5f;
    float textY = orthoTop - marginY * 0.6f;
    float lineStep = (orthoTop - orthoBottom) * 0.04f;

    std::string algoName = (g_currentAlgo == ALGO_ANALYTICAL) ? "1 - Equacao Analitica" :
                           (g_currentAlgo == ALGO_DDA)        ? "2 - DDA" : "3 - Bresenham";

    renderBitmapString(textX, textY, GLUT_BITMAP_HELVETICA_12, "Algoritmo Ativo: " + algoName);
    renderBitmapString(textX, textY - lineStep, GLUT_BITMAP_HELVETICA_12,
                       "P1: (" + std::to_string(g_x1) + ", " + std::to_string(g_y1) + ")  |  "
                       "P2: (" + std::to_string(g_x2) + ", " + std::to_string(g_y2) + ")  |  "
                       "Total de Pixels: " + std::to_string(g_pixels.size()));
    renderBitmapString(textX, textY - 2 * lineStep, GLUT_BITMAP_HELVETICA_12,
                       "Teclas: [1] Analitico  [2] DDA  [3] Bresenham  [R] Nova Reta  [Q] Sair");

    glutSwapBuffers();
}

// Redimensionamento da janela
void reshape(int w, int h) {
    g_windowWidth = w;
    g_windowHeight = (h > 0) ? h : 1;
    glViewport(0, 0, g_windowWidth, g_windowHeight);
    glutPostRedisplay();
}

// Callback do teclado
void keyboard(unsigned char key, int, int) {
    switch (key) {
        case '1':
            g_currentAlgo = ALGO_ANALYTICAL;
            updatePixelsAndPrint();
            glutPostRedisplay();
            break;
        case '2':
            g_currentAlgo = ALGO_DDA;
            updatePixelsAndPrint();
            glutPostRedisplay();
            break;
        case '3':
            g_currentAlgo = ALGO_BRESENHAM;
            updatePixelsAndPrint();
            glutPostRedisplay();
            break;
        case 'r':
        case 'R': {
            std::cout << "\nDigite as novas coordenadas (x1 y1 x2 y2): ";
            int nx1, ny1, nx2, ny2;
            if (std::cin >> nx1 >> ny1 >> nx2 >> ny2) {
                g_x1 = nx1;
                g_y1 = ny1;
                g_x2 = nx2;
                g_y2 = ny2;
                updatePixelsAndPrint();
                glutPostRedisplay();
            } else {
                std::cin.clear();
                std::string dummy;
                std::getline(std::cin, dummy);
                std::cout << "Entrada invalida! Mantendo coordenadas anteriores.\n";
            }
            break;
        }
        case 27: // ESC não fecha o comando
            std::cout << "[ESC] Pressionado (para sair, pressione 'Q' ou feche a janela)\n";
            break;
        case 'q':
        case 'Q':
            std::cout << "Encerrando questao 1...\n";
            std::exit(0);
            break;
    }
}

int main(int argc, char** argv) {
    std::cout << "========================================================\n";
    std::cout << "    QUESTAO 1 - TRACADO DE RETAS (OpenGL / FreeGLUT)    \n";
    std::cout << "========================================================\n";
    std::cout << "Algoritmos disponiveis:\n";
    std::cout << "  1. Equacao Analitica\n";
    std::cout << "  2. DDA (Digital Differential Analyzer)\n";
    std::cout << "  3. Bresenham (Apenas aritmetica inteira)\n\n";

    std::cout << "Digite as coordenadas da reta (x1 y1 x2 y2)\n";
    std::cout << "ou pressione ENTER para usar o padrao (10 10 100 50): ";

    std::string line;
    if (std::getline(std::cin, line) && !line.empty()) {
        std::stringstream ss(line);
        int nx1, ny1, nx2, ny2;
        if (ss >> nx1 >> ny1 >> nx2 >> ny2) {
            g_x1 = nx1;
            g_y1 = ny1;
            g_x2 = nx2;
            g_y2 = ny2;
        } else {
            std::cout << "Entrada incompleta. Usando padrao: (10, 10) -> (100, 50)\n";
        }
    } else {
        std::cout << "Usando coordenadas padrao: (10, 10) -> (100, 50)\n";
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(g_windowWidth, g_windowHeight);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Questao 1 - Tracado de Retas");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);

    // Executa e imprime o primeiro algoritmo
    updatePixelsAndPrint();

    glutMainLoop();
    return 0;
}
