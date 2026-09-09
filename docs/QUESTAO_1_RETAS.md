# Documentação Técnica — Questão 1: Traçado de Retas

Este documento apresenta os fundamentos teóricos, detalhes matemáticos e especificidades de implementação dos três algoritmos clássicos de rasterização de segmentos de reta desenvolvidos no executável `questao1_retas`.

---

## 1. Contextualização e Justificativa do Uso de `GL_POINTS`

Em Computação Gráfica moderna ou em bibliotecas de alto nível, um segmento de reta pode ser renderizado diretamente com primitivas vetoriais (como `glBegin(GL_LINES)`). No entanto, o hardware gráfico executa internamente a rasterização (conversão de coordenadas contínuas em fragmentos/pixels discretos da tela).

O objetivo desta avaliação acadêmica é **implementar diretamente os algoritmos fundamentais de rasterização na CPU**, determinando a coordenada inteira exata $(x, y)$ de cada pixel. 

Por essa razão:
* **Não utilizamos `GL_LINES`**: isso delegaria a rasterização ao driver de vídeo, anulando a verificação do algoritmo.
* **Utilizamos `GL_POINTS` com `glVertex2i(x, y)`**: cada pixel calculado pelo nosso código é enviado individualmente ao framebuffer OpenGL, permitindo comparar pixel por pixel a saída gráfica com a lista numérica gerada no terminal.

---

## 2. Algoritmo 1 — Equação Analítica da Reta

### 2.1 Formulação Matemática
A equação fundamental da reta no plano cartesiano contínuo é dada por:
$$y = m \cdot x + b$$
onde:
* $m = \frac{\Delta y}{\Delta x} = \frac{y_2 - y_1}{x_2 - x_1}$ representa a inclinação angular (coeficiente angular);
* $b = y_1 - m \cdot x_1$ representa o intercepto no eixo das ordenadas (coeficiente linear).

### 2.2 Tratamento de Casos e Arredondamento
Na implementação discreta direta, iterar cegamente em $x$ para qualquer valor de $m$ causa falhas graves:
1. **Reta Vertical ($\Delta x = 0$):** Ocorre divisão por zero ao calcular $m$. Tratamento: itera-se diretamente ao longo do eixo $y$ ($y_1 \to y_2$), mantendo $x = x_1$ fixo.
2. **Inclinação Suave ($|m| \le 1$):** O eixo de maior variação é o eixo $X$. Itera-se com passo unitário $\Delta x = \pm 1$ de $x_1$ a $x_2$. Para cada $x$, calcula-se:
   $$y = \text{round}(m \cdot x + b)$$
3. **Inclinação Íngreme ($|m| > 1$):** Se iterássemos em $x$, múltiplos pixels em $y$ seriam pulados, gerando descontinuidade ("buracos" visíveis na reta). Portanto, inverte-se o papel dos eixos: o eixo condutor passa a ser $Y$. Itera-se com passo unitário $\Delta y = \pm 1$ de $y_1$ a $y_2$, calculando $x$ pela relação inversa:
   $$x = \text{round}\left(\frac{y - b}{m}\right)$$

### 2.3 Características Computacionais
* Requer operações de ponto flutuante (divisões e multiplicações de números reais em cada iteração).
* Requer chamada à função de arredondamento `round()` em cada passo, tornando-o computacionalmente mais oneroso em processadores sem unidade de ponto flutuante rápida.

---

## 3. Algoritmo 2 — DDA (Digital Differential Analyzer)

### 3.1 Formulação Matemática
O algoritmo DDA explora a propriedade incremental da reta, baseando-se em equações a diferenças finitas:
$$\Delta y = m \cdot \Delta x \iff y_{k+1} = y_k + m \cdot \Delta x$$

Em vez de recalcular $m \cdot x + b$ do zero a cada ponto, o DDA soma incrementos constantes a cada iteração:
1. Determina-se a quantidade de passos necessários para cobrir a distância máxima nos dois eixos:
   $$\text{steps} = \max(|\Delta x|, |\Delta y|)$$
2. Calculam-se os incrementos fracionários por passo:
   $$\Delta x_{\text{inc}} = \frac{\Delta x}{\text{steps}}, \quad \Delta y_{\text{inc}} = \frac{\Delta y}{\text{steps}}$$
3. A partir do ponto inicial $(x, y) = (x_1, y_1)$, executa-se o laço acumulador:
   $$\text{plot}(\text{round}(x), \text{round}(y))$$
   $$x \leftarrow x + \Delta x_{\text{inc}}, \quad y \leftarrow y + \Delta y_{\text{inc}}$$

### 3.2 Casos Especiais
* **Ponto Único ($x_1 = x_2$ e $y_1 = y_2$):** $\text{steps} = 0$. O algoritmo plota apenas $(x_1, y_1)$ e encerra sem efetuar divisões por zero.
* **Qualquer Quadrante/Octante:** Como os sinais de $\Delta x$ e $\Delta y$ são preservados nas variáveis reais, o algoritmo percorre nativamente retas orientadas em qualquer direção (esquerda para direita, direita para esquerda, cima para baixo ou baixo para cima).

### 3.3 Características Computacionais
* Reduz as multiplicações a simples adições sucessivas em ponto flutuante.
* Ainda depende de registradores de ponto flutuante e de arredondamentos sucessivos `round()`, o que pode acarretar pequenos desvios cumulativos de precisão numérica em retas muito extensas.

---

## 4. Algoritmo 3 — Bresenham (Aritmética Inteira)

### 4.1 Conceito Fundamental
Publicado por Jack E. Bresenham em 1965, este algoritmo determina a trajetória discreta ótima minimizando a distância entre a reta contínua teórica e os centros dos pixels vizinhos.

Sua grande vantagem e genialidade é que **todas as decisões são tomadas exclusivamente com adições, subtrações e multiplicações por 2 (que equivalem a deslocamento de bits `<< 1`), sem nenhuma operação de ponto flutuante ou divisão durante o laço.**

### 4.2 Generalização para Todos os Octantes
A formulação geral para qualquer par de pontos $(x_1, y_1)$ e $(x_2, y_2)$ utiliza a variável de erro inicial $err = \Delta x - \Delta y$, onde:
* $dx = |x_2 - x_1|$
* $dy = |y_2 - y_1|$
* $sx = \begin{cases} +1, & \text{se } x_1 < x_2 \\ -1, & \text{caso contrário} \end{cases}$
* $sy = \begin{cases} +1, & \text{se } y_1 < y_2 \\ -1, & \text{caso contrário} \end{cases}$

O laço iterativo funciona da seguinte maneira:
```cpp
int err = dx - dy;
while (true) {
    plot(x, y);
    if (x == x2 && y == y2) break;

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
```

### 4.3 Princípio da Decisão Inteira
* $e2 > -dy$: indica que o pixel deve avançar na direção de $X$. O erro é decrementado de $dy$.
* $e2 < dx$: indica que o pixel deve avançar na direção de $Y$. O erro é incrementado de $dx$.
* Quando ambas as condições são verdadeiras no mesmo passo, o algoritmo realiza um passo diagonal, garantindo uma discretização contígua de 8-conectividade sem falhas.

---

## 5. Comparativo dos Métodos

| Critério | Equação Analítica | DDA | Bresenham |
| :--- | :--- | :--- | :--- |
| **Operações na iteração** | Multiplicação + Soma (Float) | Soma (Float) | Soma + Deslocamento (Inteiro puro) |
| **Necessita `round()`** | Sim | Sim | Não |
| **Custo computacional** | Alto | Médio | Mínimo (ideal para hardware) |
| **Suporte a octantes** | Requer ramificação condicional | Universal automático | Universal automático |
| **Acúmulo de erro** | Nulo (cálculo absoluto) | Possível deriva por float | Nulo (exato na malha discreta) |
| **Garantia dos extremos** | Exata | Exata | Exata |
