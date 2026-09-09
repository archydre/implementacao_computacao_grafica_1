# 1ª Avaliação de Computação Gráfica

Implementação integral da 1ª Avaliação Prática de Computação Gráfica, contemplando algoritmos de rasterização de retas, transformações geométricas 3D na pilha ModelView do OpenGL e projeções geométricas canônicas (ortográfica, oblíquas e perspectivas com pontos de fuga).

---

## 1. Tecnologias Utilizadas

* **Linguagem:** C++ (Padrão C++17)
* **API Gráfica:** OpenGL clássico (Immediate Mode / Display Lists)
* **Biblioteca de Janelas e Eventos:** FreeGLUT / GLUT
* **Utilitários Gráficos:** GLU (OpenGL Utility Library)
* **Sistema de Build:** CMake (versão mínima 3.16)
* **Compilador:** GCC / G++ ou Clang

---

## 2. Dependências do Sistema

Para compilar e executar o projeto no Linux, certifique-se de que os pacotes de desenvolvimento do OpenGL, GLU e FreeGLUT estejam instalados.

### Ubuntu / Debian / Pop!_OS / Linux Mint:
```bash
sudo apt update
sudo apt install build-essential cmake freeglut3-dev libglu1-mesa-dev mesa-common-dev
```

### Arch Linux / Manjaro:
```bash
sudo pacman -S base-devel cmake freeglut glu mesa
```

### Fedora / RHEL:
```bash
sudo dnf install gcc-c++ cmake freeglut-devel mesa-libGLU-devel mesa-libGL-devel
```

---

## 3. Compilação (Build)

O projeto utiliza **CMake** com suporte multiplataforma nativo. No diretório raiz do projeto:

```bash
# 1. Configurar o diretório de build
cmake -S . -B build

# 2. Compilar todos os executáveis
cmake --build build
```

Após a compilação bem-sucedida, os executáveis gerados estarão localizados em `build/`:
* `build/questao1_retas`
* `build/questao2_transformacoes`
* `build/questao3_projecoes`
* `build/test_matematica` (Suíte de validação automatizada)

---

## 4. Como Executar

O projeto gera executáveis nativos independentes para cada questão:

### 4.1 Suíte de Testes Automatizados (Sem necessidade de tela)
Executa a validação analítica e matricial de todas as questões:
```bash
./build/test_matematica
# ou via CTest:
ctest --test-dir build --output-on-failure
```

### 4.2 Questão 1 — Traçado de Retas
```bash
./build/questao1_retas
```
* Ao iniciar, solicite no terminal os pontos $(x_1, y_1)$ e $(x_2, y_2)$ ou pressione `ENTER` para carregar o segmento padrão `(10, 10) -> (100, 50)`.
* A janela gráfica desenha cada pixel via `GL_POINTS` e o terminal imprime a lista de coordenadas discretizadas de forma compacta.

### 4.3 Questão 2 — Transformações Geométricas
```bash
./build/questao2_transformacoes
```
* O terminal exibe a comprovação de não-coplanaridade, a translação que leva $D$ a $(0,0,0)$, a matriz de rotação que alinha a base com $XZ$, a garantia de $E_y < 0$ e os resíduos.
* A janela exibe o poliedro original e transformado em 3D, com eixos $X$ (vermelho), $Y$ (verde) e $Z$ (azul).

### 4.4 Questão 3 — Projeções Geométricas
```bash
./build/questao3_projecoes
```
* Permite digitar um poliedro arbitrário ou carregar modelos pré-configurados (Cubo, Casinha 3D ou o Poliedro da Questão 2).
* Alterna entre as 6 projeções canônicas exibindo a matriz homogênea $4 \times 4$ e a tabela de conversão no terminal.

---

## 5. Tabela de Controles e Atalhos

*(Nota: Pressionar `ESC` não encerra os programas; para sair utilize `Q` ou feche a janela gráfica).*

### Questão 1 — Traçado de Retas
| Tecla | Ação |
| :--- | :--- |
| `1` | Ativar algoritmo da **Equação Analítica** |
| `2` | Ativar algoritmo **DDA** (*Digital Differential Analyzer*) |
| `3` | Ativar algoritmo de **Bresenham** (aritmética puramente inteira) |
| `R` ou `r` | Digitar novas coordenadas $(x_1, y_1, x_2, y_2)$ pelo terminal |
| `Q` ou `q` | Encerrar o programa |
| `ESC` | Não encerra o programa (aviso no terminal) |

### Questão 2 — Transformações Geométricas
| Tecla / Ação | Função |
| :--- | :--- |
| `O` ou `o` | Exibir apenas o sólido **Original** (cinza) |
| `T` ou `t` | Exibir apenas o sólido **Transformado** (verde) |
| `A` ou `a` | Exibir **Ambos** simultaneamente para comparação |
| `Arrastar Mouse (Botão Esquerdo)` | Rotacionar câmera orbital 3D |
| `+` / `-` | Ajustar aproximação (zoom) da câmera |
| `Q` ou `q` | Encerrar o programa |
| `ESC` | Não encerra o programa (aviso no terminal) |

### Questão 3 — Projeções Geométricas
| Tecla | Projeção Ativa / Ação |
| :--- | :--- |
| `1` | **Paralela Ortográfica** (plano $XY$, $z'=0$) |
| `2` | **Paralela Oblíqua Cabinet** ($l = 0.5$, $\alpha = 45^\circ$) |
| `3` | **Paralela Oblíqua Cavaleira** ($l = 1.0$, $\alpha = 45^\circ$) |
| `4` | **Perspectiva com 1 Ponto de Fuga** (foco no eixo $Z$) |
| `5` | **Perspectiva com 2 Pontos de Fuga** (focos nos eixos $X$ e $Z$) |
| `6` | **Perspectiva com 3 Pontos de Fuga** (focos nos eixos $X, Y, Z$) |
| `C` ou `c` | Alternar modelo de poliedro (Cubo / Casinha / Questão 2) |
| `+` / `-` | Aumentar ou diminuir a distância focal dos pontos de fuga |
| `Q` ou `q` | Encerrar o programa |
| `ESC` | Não encerra o programa (aviso no terminal) |

---

## 6. Resumo das Questões

1. **Questão 1 (`questao1_retas`):** Discretização pura de segmentos de reta sem usar `GL_LINES`. Demonstra o custo do cálculo contínuo (Equação Analítica), a eficiência do acumulador incremental (DDA) e a velocidade máxima da aritmética inteira com tomada de decisão por sinal de erro (Bresenham em todos os octantes).
2. **Questão 2 (`questao2_transformacoes`):** Aplicação das operações `glTranslatef` e `glRotatef` no pipeline do OpenGL para posicionar o sólido 3D de acordo com os requisitos físicos do enunciado, preservando os vértices originais.
3. **Questão 3 (`questao3_projecoes`):** Implementação manual da álgebra linear homogênea $4 \times 4$ para projeções ortográficas, oblíquas e cônicas em perspectiva, demonstrando a divisão por $w$ e o mapeamento correto na tela.

---

## 7. Observação Matemática sobre a Questão 2

O enunciado solicita que a base $ABCD$ fique paralela ao plano $XZ$. No entanto, calculando o produto misto a partir do vértice $D$:
$$\vec{u} = A - D = (-5, 0, 25)$$
$$\vec{v} = B - D = (25, 5, 20)$$
$$\vec{w} = C - D = (20, 5, 5)$$
$$\vec{u} \cdot (\vec{v} \times \vec{w}) = (-5)(-75) + 0(275) + 25(25) = \mathbf{1000} \ne 0$$

Como o volume não é nulo, **os 4 vértices não pertencem rigorosamente ao mesmo plano**. Para honrar o enunciado sem adulterar as coordenadas originais:
* O software preserva os vértices literais da prova;
* O software emite aviso fundamentado no terminal;
* Utiliza-se o **Vetor Normal de Newell** para encontrar o plano médio da base e alinhá-lo ao plano $XZ$;
* O vértice $E$ é invertido satisfazendo estritamente $Y_E = -17.6505 < 0$;
* Os resíduos em relação a $Y = 0$ são exibidos de forma clara e transparente.

---

## 8. Estrutura do Projeto

```text
comp_grafic/
├── CMakeLists.txt
├── README.md
├── .gitignore
├── docs/
│   ├── ENUNCIADO.md
│   ├── QUESTAO_1_RETAS.md
│   ├── QUESTAO_2_TRANSFORMACOES.md
│   ├── QUESTAO_3_PROJECOES.md
│   ├── VALIDACAO.md
│   ├── ROTEIRO_APRESENTACAO.md
│   └── GUIA_EXPLICACAO.md
└── src/
    ├── common/
    │   ├── math.hpp
    │   └── math.cpp
    ├── questao1/
    │   └── main.cpp
    ├── questao2/
    │   └── main.cpp
    ├── questao3/
    │   └── main.cpp
    └── tests/
        └── test_math.cpp
```
# implementacao_computacao_grafica_1
