# 1ª Avaliação de Computação Gráfica — Enunciado Oficial

* **Modalidade:** Trabalho em grupo pré-definido.
* **Apresentação e Entrega:** 10/09/2026 (durante a aula).
* **Tecnologias Exigidas:** C/C++ com OpenGL clássico, FreeGLUT e GLU.
* **Foco:** Implementação conceitual e matemática dos algoritmos estudados em sala de aula, sem abstração por APIs de alto nível ou bibliotecas prontas de álgebra.

---

## Questão 1 — Traçado de Retas (3,0 pontos)

Implemente os 3 algoritmos de traçado de retas vistos em sala:
1. **Equação analítica** ($y = mx + b$);
2. **DDA** (*Digital Differential Analyzer*);
3. **Bresenham** (algoritmo de ponto médio com aritmética puramente inteira).

### Requisitos Funcionais
* A imagem da reta discretizada deve ser gerada em uma janela gráfica OpenGL utilizando `GL_POINTS` e `glVertex2i(...)` para cada pixel gerado. Não é permitido desenhar a reta diretamente via `GL_LINES`.
* As coordenadas $(x, y)$ de cada pixel discretizado devem ser exibidas sequencialmente no terminal.
* O sistema deve permitir a entrada de dois pontos arbitrários $(x_1, y_1)$ e $(x_2, y_2)$ e permitir alternar interativamente entre os três métodos para comparação visual e numérica.

---

## Questão 2 — Transformações Geométricas 3D (3,0 pontos)

Dado o poliedro com os seguintes vértices:
* **A** = $(0, 0, 0)$ (Canto da frente à esquerda)
* **B** = $(30, 5, -5)$ (Canto da frente à direita)
* **C** = $(25, 5, -20)$ (Canto do fundo à direita)
* **D** = $(5, 0, -25)$ (Canto do fundo à esquerda)
* **E** = $(15, 20, 12)$ (Topo / Ápice)

E com as arestas conectando a base ($AB, BC, CD, DA$) e as laterais ao ápice ($EA, EB, EC, ED$).

### Objetivo
Aplicar uma sequência de transformações geométricas utilizando **funções OpenGL** (`glTranslatef`, `glRotatef`) na pilha ModelView de modo que o objeto atenda às seguintes condições finais:
1. O vértice **D** termine posicionado exatamente na origem do sistema de coordenadas $(0, 0, 0)$;
2. O polígono formado pela base **ABCD** fique paralelo ao plano $XZ$ ($Y = 0$);
3. O vértice **E** fique posicionado abaixo do polígono $ABCD$, com sua coordenada $Y$ negativa ($Y_E < 0$).

### Nota de Rigor Matemático
* As coordenadas literais fornecidas no enunciado resultam em um produto misto $(A-D) \cdot ((B-D) \times (C-D)) = 1000 \ne 0$, comprovando que a tétrade $ABCD$ não é estritamente coplanar.
* O software preserva as coordenadas originais do enunciado e utiliza a orientação representativa pelo Vetor Normal de Newell para alinhar o plano médio da base com $XZ$, demonstrando o resíduo no terminal e deixando a arquitetura aberta para qualquer retificação posterior.

---

## Questão 3 (Item C) — Projeções Geométricas 3D (4,0 pontos)

Utilizando as matrizes homogêneas de projeção $4 \times 4$ vistas em sala, desenvolva um programa que receba **um poliedro qualquer** (número arbitrário de vértices e arestas) e o exiba nas seguintes projeções:
1. **Paralela Ortográfica** (projeção no plano $XY$ com $z'=0$);
2. **Paralela Oblíqua Cabinet** ($\tan\beta = 2 \implies l = 0.5$, $\alpha = 45^\circ$);
3. **Paralela Oblíqua Cavaleira** ($\tan\beta = 1 \implies l = 1.0$, $\alpha = 45^\circ$);
4. **Perspectiva com 1 Ponto de Fuga** (foco no eixo $Z$);
5. **Perspectiva com 2 Pontos de Fuga** (focos nos eixos $X$ e $Z$);
6. **Perspectiva com 3 Pontos de Fuga** (focos nos eixos $X$, $Y$ e $Z$).

### Requisitos Funcionais
* Não é permitido usar `gluPerspective` ou projeções embutidas do OpenGL para computar a projeção dos vértices. A multiplicação matricial homogênea e a divisão por $w$ devem ser efetuadas pelo programa.
* O programa deve imprimir no terminal a matriz $4 \times 4$ ativa e a tabela de conversão de cada vértice: Coordenada Original $\to$ Coordenada Homogênea $\to$ Coordenada Projetada pós divisão por $w$.
* O desenho 2D final é renderizado no canvas com ajuste de enquadramento (viewport/auto-fit) desacoplado do cálculo projetivo.
