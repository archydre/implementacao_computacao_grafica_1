# Documentação Técnica — Questão 3: Projeções Geométricas 3D

Este documento descreve as formulações matemáticas das seis projeções geométricas implementadas no executável `questao3_projecoes`.

---

## 1. Convenção Adotada

Todas as operações matriciais seguem a convenção clássica de Álgebra Linear e dos livros canônicos de Computação Gráfica (Hearn & Baker / Foley / Rogers):
1. **Vetor-coluna homogêneo:** $P = \begin{bmatrix} x \\ y \\ z \\ 1 \end{bmatrix}$
2. **Multiplicação da transformação:** $P' = M \cdot P$
3. **Divisão homogênea:** $x_p = \frac{x'}{w'}, \quad y_p = \frac{y'}{w'}, \quad z_p = \frac{z'}{w'}$
4. **Armazenamento:** Matriz $4 \times 4$ com índices `m[linha][coluna]`.

---

## 2. Projeção Paralela Ortográfica no Plano XY

### 2.1 Conceito
Os raios de projeção são paralelos entre si e perpendiculares ao plano de projeção ($XY$). A coordenada de profundidade $Z$ é descartada, preservando tamanhos e paralelismo.

### 2.2 Matriz Homogênea
$$M_{\text{ortho}} = 
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & 0 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}$$

### 2.3 Exemplo Numérico
Para $P = (20, 30, 40)$:
$$P' = M_{\text{ortho}} \cdot \begin{bmatrix} 20 \\ 30 \\ 40 \\ 1 \end{bmatrix} = \begin{bmatrix} 20 \\ 30 \\ 0 \\ 1 \end{bmatrix} \implies (x_p, y_p) = (20, 30)$$

---

## 3. Projeções Paralelas Oblíquas

Nas projeções oblíquas, os raios de projeção são paralelos entre si, mas incidem de forma oblíqua (não-perpendicular) em relação ao plano de projeção.

A formulação teórica vista em aula define:
$$x_p = x + z \cdot l \cos(\alpha)$$
$$y_p = y + z \cdot l \sin(\alpha)$$
$$z_p = 0$$
onde $\alpha$ é o ângulo de inclinação do eixo de profundidade em relação à horizontal (convencionalmente $45^\circ$) e $l = \frac{1}{\tan(\beta)}$, sendo $\beta$ o ângulo dos raios projetores com o plano de projeção.

Matriz homogênea geral oblíqua:
$$M_{\text{oblique}} = 
\begin{bmatrix}
1 & 0 & l \cos\alpha & 0 \\
0 & 1 & l \sin\alpha & 0 \\
0 & 0 & 0 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}$$

---

### 3.1 Projeção Cavaleira
* $\tan(\beta) = 1 \implies \beta = 45^\circ \implies l = 1.0$.
* O fator de profundidade é **integralmente preservado ($100\%$)**.
* Com $\alpha = 45^\circ$: $l \cos(45^\circ) = \frac{\sqrt{2}}{2} \approx 0.7071$, $l \sin(45^\circ) \approx 0.7071$.

Matriz Cavaleira:
$$M_{\text{cav}} = 
\begin{bmatrix}
1 & 0 & 0.7071 & 0 \\
0 & 1 & 0.7071 & 0 \\
0 & 0 & 0 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}$$

Exemplo para $P = (20, 30, 40)$:
$$x_p = 20 + 40 \times 0.7071 = 48.2843$$
$$y_p = 30 + 40 \times 0.7071 = 58.2843$$

---

### 3.2 Projeção Cabinet
* $\tan(\beta) = 2 \implies \beta \approx 63.43^\circ \implies l = 0.5$.
* O fator de profundidade é **reduzido pela metade ($50\%$)**, gerando uma aparência visualmente mais natural ao olho humano do que a cavaleira.
* Com $\alpha = 45^\circ$: $0.5 \cos(45^\circ) \approx 0.3536$, $0.5 \sin(45^\circ) \approx 0.3536$.

Matriz Cabinet:
$$M_{\text{cab}} = 
\begin{bmatrix}
1 & 0 & 0.3536 & 0 \\
0 & 1 & 0.3536 & 0 \\
0 & 0 & 0 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}$$

Exemplo para $P = (20, 30, 40)$:
$$x_p = 20 + 40 \times 0.3536 = 34.1421$$
$$y_p = 30 + 40 \times 0.3536 = 44.1421$$

---

## 4. Projeções em Perspectiva com Pontos de Fuga

Na projeção perspectiva, os raios convergem para um centro de projeção finito. As retas paralelas na cena convergem para **pontos de fuga** no plano de projeção.

A matriz geral homogênea com termos de fuga nos eixos principais é:
$$M_{\text{persp}} = 
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & 0 & 0 \\
-\frac{1}{f_x} & -\frac{1}{f_y} & -\frac{1}{f_z} & 1
\end{bmatrix}$$

Ao multiplicar pelo ponto $(x, y, z, 1)^T$, obtém-se o vetor homogêneo:
$$P_h = \begin{bmatrix} x \\ y \\ 0 \\ 1 - \frac{x}{f_x} - \frac{y}{f_y} - \frac{z}{f_z} \end{bmatrix}$$

A coordenada projetada final no plano $XY$ é obtida dividindo-se por $w$:
$$x_p = \frac{x}{w}, \quad y_p = \frac{y}{w}$$

---

### 4.1 Perspectiva com 1 Ponto de Fuga
* Apenas um eixo converge para o infinito (tipicamente o eixo de profundidade $Z$).
* $f_x = \infty \implies -\frac{1}{f_x} = 0$
* $f_y = \infty \implies -\frac{1}{f_y} = 0$
* $f_z$ é finito (ex: $f_z = 120$).
* Fator homogêneo: $w = 1 - \frac{z}{f_z}$.

Matriz:
$$M_{P1} = 
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & 0 & 0 \\
0 & 0 & -\frac{1}{f_z} & 1
\end{bmatrix}$$

---

### 4.2 Perspectiva com 2 Pontos de Fuga
* Dois eixos convergem para pontos de fuga finitos (tipicamente $X$ e $Z$, simulando uma visão de esquina/aresta vertical alinhada).
* $f_x$ e $f_z$ são finitos (ex: $f_x = 120, f_z = 120$).
* $f_y = \infty \implies -\frac{1}{f_y} = 0$.
* Fator homogêneo: $w = 1 - \frac{x}{f_x} - \frac{z}{f_z}$.

Matriz:
$$M_{P2} = 
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & 0 & 0 \\
-\frac{1}{f_x} & 0 & -\frac{1}{f_z} & 1
\end{bmatrix}$$

---

### 4.3 Perspectiva com 3 Pontos de Fuga
* Os três eixos $X$, $Y$ e $Z$ convergem para pontos de fuga finitos (simulando visão aérea "olho de pássaro" ou "olho de minhoca").
* $f_x, f_y, f_z$ são finitos (ex: $f_x = 120, f_y = 120, f_z = 120$).
* Fator homogêneo: $w = 1 - \frac{x}{f_x} - \frac{y}{f_y} - \frac{z}{f_z}$.

Matriz:
$$M_{P3} = 
\begin{bmatrix}
1 & 0 & 0 & 0 \\
0 & 1 & 0 & 0 \\
0 & 0 & 0 & 0 \\
-\frac{1}{f_x} & -\frac{1}{f_y} & -\frac{1}{f_z} & 1
\end{bmatrix}$$

---

## 5. Viewport / Ajuste à Janela Desacoplado

Conforme determinação explícita do enunciado:
> *"Esse ajuste deve acontecer depois da projeção e não deve modificar a matriz matemática sendo demonstrada."*

O procedimento adotado:
1. Os vértices são matematicamente transformados por $M_{\text{proj}}$ e divididos por $w$;
2. A rotina de renderização encontra $(\min X, \max X, \min Y, \max Y)$ dos pontos projetados;
3. Configura-se `gluOrtho2D` na janela de exibição para enquadrar esses limites com margem de $30\%$;
4. Desta forma, a matemática das projeções é puramente canônica e transparente para verificação teórica.
