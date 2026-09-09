# Relatório de Validação e Testes Numéricos

Este documento registra a validação completa de todos os requisitos do projeto, apresentando as tabelas comparativas e a checklist formal exigida no enunciado.

---

## 1. Checklist Geral de Requisitos

- [x] `questao1_retas` compila sem erros ou warnings
- [x] `questao2_transformacoes` compila sem erros ou warnings
- [x] `questao3_projecoes` compila sem erros ou warnings
- [x] `test_matematica` compila e passa em 100% dos testes unitários
- [x] Equação Analítica desenha via `GL_POINTS` e imprime coordenadas no terminal
- [x] DDA desenha via `GL_POINTS` e imprime coordenadas no terminal
- [x] Bresenham utiliza aritmética puramente inteira, desenha via `GL_POINTS` e imprime coordenadas
- [x] Vértice $D$ termina rigorosamente na origem $(0, 0, 0)$
- [x] Verificação de coplanaridade executada e impressa (Produto misto = $1000$)
- [x] Vértice $E$ termina orientado para $Y$ negativo ($Y_E = -17.6505 < 0$)
- [x] Erro residual da base $ABCD$ é demonstrado no terminal
- [x] Projeção Ortográfica funciona com matriz canônica
- [x] Projeção Cabinet funciona com $l = 0.5$ e $\alpha = 45^\circ$
- [x] Projeção Cavaleira funciona com $l = 1.0$ e $\alpha = 45^\circ$
- [x] Projeção Perspectiva de 1 ponto de fuga funciona com divisão homogênea por $w$
- [x] Projeção Perspectiva de 2 pontos de fuga funciona com divisão homogênea por $w$
- [x] Projeção Perspectiva de 3 pontos de fuga funciona com divisão homogênea por $w$
- [x] Matrizes de transformação e projeção são formatadas e impressas no terminal
- [x] `README.md` detalha comandos de build, execução e uso dos controles

---

## 2. Testes de Rasterização da Questão 1

Foram submetidas à validação as seis retas canônicas cobrindo todos os quadrantes, octantes, retas verticais, horizontais e inclinações suaves e íngremes:

| Caso de Teste | Segmento | Analítico | DDA | Bresenham | Primeiro Pixel | Último Pixel | Status |
| :--- | :--- | :---: | :---: | :---: | :---: | :---: | :---: |
| **1. Octante 1 ($0 < m < 1$)** | $(10, 10) \to (100, 50)$ | 91 pixels | 91 pixels | 91 pixels | $(10, 10)$ | $(100, 50)$ | **OK** |
| **2. Octante 2 ($m > 1$)** | $(10, 10) \to (50, 100)$ | 91 pixels | 91 pixels | 91 pixels | $(10, 10)$ | $(50, 100)$ | **OK** |
| **3. Sentido Invertido** | $(100, 50) \to (10, 10)$ | 91 pixels | 91 pixels | 91 pixels | $(100, 50)$ | $(10, 10)$ | **OK** |
| **4. Reta Vertical ($\Delta x = 0$)** | $(50, 10) \to (50, 100)$ | 91 pixels | 91 pixels | 91 pixels | $(50, 10)$ | $(50, 100)$ | **OK** |
| **5. Reta Horizontal ($\Delta y = 0$)** | $(10, 50) \to (100, 50)$ | 91 pixels | 91 pixels | 91 pixels | $(10, 50)$ | $(100, 50)$ | **OK** |
| **6. Inclinação Negativa ($m < 0$)** | $(10, 100) \to (100, 10)$ | 91 pixels | 91 pixels | 91 pixels | $(10, 100)$ | $(100, 10)$ | **OK** |

### Observações:
1. Em todos os casos, a quantidade de pixels gerada é exatamente $\max(|\Delta x|, |\Delta y|) + 1 = 91$ pixels.
2. O primeiro pixel é exatamente $(x_1, y_1)$ e o último pixel é $(x_2, y_2)$.
3. O algoritmo de Bresenham concorda perfeitamente com o modelo analítico e DDA em toda a trajetória discreta.

---

## 3. Testes Numéricos da Questão 2

### 3.1 Teste de Coplanaridade
* Vetor $\vec{u} = A - D = (-5, 0, 25)$
* Vetor $\vec{v} = B - D = (25, 5, 20)$
* Vetor $\vec{w} = C - D = (20, 5, 5)$
* Produto Vetorial $\vec{v} \times \vec{w} = (-75, 275, 25)$
* **Produto Misto:** $\vec{u} \cdot (\vec{v} \times \vec{w}) = (-5)(-75) + 0(275) + 25(25) = \mathbf{1000.0} \ne 0$.

### 3.2 Alinhamento do Normal de Newell e Translação
* Vetor Normal de Newell normalizado: $\hat{n} = (-0.1961, 0.9806, 0.0000)$.
* Alvo descendente escolhido para garantir $E_y < 0$: $\hat{t} = (0.0000, -1.0000, 0.0000)$.
* Ângulo de rotação: $\theta = 168.6901^\circ$, Eixo: $(0, 0, 1)$.
* Vetor de translação: $\vec{T} = (-5.0, 0.0, 25.0)$.

### 3.3 Coordenadas Finais e Resíduos

| Vértice | Coordenada Original | Coordenada Final $M \cdot V$ | Resíduo $Y$ | Verificação |
| :--- | :--- | :--- | :--- | :--- |
| **A** | $(0.00, 0.00, 0.00)$ | $(4.9029, -0.9806, 25.0000)$ | $-0.9806$ | Plano médio $Y \approx 0$ |
| **B** | $(30.00, 5.00, -5.00)$ | $(-25.4951, 0.0000, 20.0000)$ | $0.0000$ | Exato em $Y = 0$ |
| **C** | $(25.00, 5.00, -20.00)$ | $(-20.5922, -0.9806, 5.0000)$ | $-0.9806$ | Plano médio $Y \approx 0$ |
| **D** | $(5.00, 0.00, -25.00)$ | $(\mathbf{0.0000, 0.0000, 0.0000})$ | $\mathbf{0.0000}$ | **Origem Exata** |
| **E** | $(15.00, 20.00, 12.00)$ | $(-13.7281, \mathbf{-17.6505}, 37.0000)$ | $-$ | $\mathbf{Y_E < 0}$ **(Sucesso)** |

---

## 4. Testes Numéricos da Questão 3

Teste aplicado ao ponto canônico $P = (20.0, 30.0, 40.0)$ com distância focal de referência $f = 100.0$:

| Projeção | Matriz Homogênea Aplicada | Vetor Homogêneo $[x_h, y_h, z_h, w]$ | Coordenada Projetada $(x_p, y_p)$ |
| :--- | :--- | :--- | :--- |
| **1. Ortográfica** | $z'=0$ | $[20.00, 30.00, 0.00, 1.00]$ | $(20.00, 30.00)$ |
| **2. Cabinet** | $l=0.5, \alpha=45^\circ$ | $[34.1421, 44.1421, 0.00, 1.00]$ | $(34.1421, 44.1421)$ |
| **3. Cavaleira** | $l=1.0, \alpha=45^\circ$ | $[48.2843, 58.2843, 0.00, 1.00]$ | $(48.2843, 58.2843)$ |
| **4. Persp. 1 pt** | $f_z = 100$ | $[20.00, 30.00, 0.00, w=0.60]$ | $(33.3333, 50.0000)$ |
| **5. Persp. 2 pt** | $f_x=100, f_z=100$ | $[20.00, 30.00, 0.00, w=0.40]$ | $(50.0000, 75.0000)$ |
| **6. Persp. 3 pt** | $f_x=100, f_y=100, f_z=100$| $[20.00, 30.00, 0.00, w=0.10]$ | $(200.0000, 300.0000)$ |

* Proteção contra divisão por zero testada para pontos com $|w| < 10^{-6}$, identificados como singulares sem provocar crash no programa.
