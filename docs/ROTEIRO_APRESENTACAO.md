# Roteiro de Apresentação — 1ª Avaliação de Computação Gráfica

Este roteiro foi elaborado para conduzir uma apresentação fluida e objetiva de **5 a 10 minutos** perante o professor e a turma.

---

## 1. Visão Geral e Estrutura (1 minuto)

> *"Bom dia/tarde, professor. Nosso projeto foi implementado integralmente em C++ com OpenGL clássico e FreeGLUT, utilizando CMake como sistema de build. Dividimos o trabalho em três executáveis independentes, um para cada questão da prova, além de um módulo comum de matemática e álgebra linear homogênea e um validador automatizado."*

Mostre a estrutura de arquivos no terminal ou editor:
```bash
ls -la
tree -L 2 src docs
```

---

## 2. Compilação e Execução dos Testes Automatizados (1 minuto)

Demonstre que o projeto compila do zero sem avisos:
```bash
cmake -S . -B build
cmake --build build
```

Em seguida, execute a suíte de validação:
```bash
./build/test_matematica
```
> *"Aqui podemos conferir que todas as rotinas matemáticas passam com 100% de sucesso: as retas nos 8 octantes, a álgebra das transformações da questão 2 e todas as matrizes e divisões homogêneas da questão 3."*

---

## 3. Demonstração da Questão 1 — Traçado de Retas (2 minutos)

Inicie o programa:
```bash
./build/questao1_retas
```
1. Pressione `ENTER` para aceitar as coordenadas padrão `(10, 10) -> (100, 50)`.
2. Mostre a janela OpenGL com a reta discretizada em azul/ciano e os eixos de referência.
3. No terminal, aponte a lista de pixels impressa:
   ```text
   === EQUACAO ANALITICA ===
   Pixel 000: (10, 10)
   ...
   Total: 91 pixels
   ```
4. Pressione `2` para alternar para o **DDA**: mostre a atualização no terminal e no título da janela.
5. Pressione `3` para alternar para o **Bresenham**:
   > *"Professor, o algoritmo de Bresenham foi implementado utilizando exclusivamente aritmética inteira (soma, subtração e multiplicação por 2 com shifts), sem qualquer divisão ou ponto flutuante durante a iteração."*
6. Pressione `R` e insira uma reta com inclinação íngreme ou vertical, por exemplo: `10 10 30 90`. Mostre que o traçado permanece contíguo sem furos.
7. Pressione `Q` (ou feche a janela) para encerrar (o `ESC` foi ajustado para não interromper a execução acidentalmente).

---

## 4. Demonstração da Questão 2 — Transformações 3D (2 a 3 minutos)

Inicie o programa:
```bash
./build/questao2_transformacoes
```
1. **Apresentação da Inconsistência de forma técnica e neutra:**
   > *"Ao carregar os pontos A, B, C e D literais da prova, o software calcula o produto misto (A-D) . ((B-D) x (C-D)). O resultado obtido é 1000, o que comprova que os quatro pontos da base não são coplanares no espaço original. Conforme orientado no material, não alteramos silenciosamente nenhum dado. Em vez disso, preservamos as coordenadas da prova e utilizamos o vetor normal de Newell para alinhar o plano médio da base com o plano XZ."*
2. **Confirmação dos Três Requisitos no Terminal:**
   * **Requisito 1:** $D$ termina exatamente na origem $(0, 0, 0)$;
   * **Requisito 2:** Os resíduos verticais da base em relação a $Y = 0$ são exibidos (comprovando o paralelismo médio ótimo);
   * **Requisito 3:** $E$ termina com $Y_E = -17.6505 < 0$ (abaixo da base).
3. **Na Janela Gráfica:**
   * Arraste com o botão esquerdo do mouse para girar a câmera orbital e inspecionar o poliedro em 3D.
   * Pressione `O` para ver apenas o original (cinza).
   * Pressione `T` para ver apenas o transformado (verde brilhante).
   * Pressione `A` para ver ambos simultaneamente.
   * Aponte para o eixo vermelho (X) e azul (Z) e mostre que a base está apoiada no plano XZ com o ápice apontando para baixo.
4. Pressione `Q` para encerrar.

---

## 5. Demonstração da Questão 3 — Projeções Geométricas (2 minutos)

Inicie o programa:
```bash
./build/questao3_projecoes
```
1. Pressione `ENTER` para selecionar o Cubo de demonstração.
2. Explique a filosofia de implementação:
   > *"Todas as projeções foram calculadas manualmente via matriz homogênea 4x4 no nosso código C++, sem recorrer ao gluPerspective. Apenas o desenho das linhas 2D resultantes é enviado ao OpenGL."*
3. Pressione as teclas sequencialmente, mostrando no terminal a matriz homogênea e a tabela de conversão:
   * Tecla `1`: **Ortográfica** (plano XY, $z'=0$).
   * Tecla `2`: **Cabinet** (profundidade reduzida em 50%, $\alpha = 45^\circ$).
   * Tecla `3`: **Cavaleira** (profundidade preservada em 100%, $\alpha = 45^\circ$).
   * Tecla `4`: **Perspectiva 1 Ponto** (linhas em Z convergem).
   * Tecla `5`: **Perspectiva 2 Pontos** (linhas em X e Z convergem).
   * Tecla `6`: **Perspectiva 3 Pontos** (linhas em X, Y e Z convergem).
4. Pressione `C` para trocar de modelo: mostre a Casinha 3D ou o Poliedro da Questão 2 projetado sob as mesmas matrizes.
5. Pressione `Q` para sair.

---

## 6. Perguntas Prováveis do Professor e Respostas Rápidas

### P1: *"Por que vocês não usaram `GL_LINES` na Questão 1?"*
**Resposta:** Porque `GL_LINES` delega o algoritmo de discretização de pixels diretamente para a placa de vídeo. Usamos `GL_POINTS` com `glVertex2i` justamente para demonstrar que os pixels gerados na tela são calculados pelo nosso código C++ (Equação Analítica, DDA e Bresenham).

### P2: *"Qual a diferença prática entre a projeção Cavaleira e a Cabinet?"*
**Resposta:** Ambas são projeções oblíquas com ângulo $\alpha$ com a horizontal (geralmente $45^\circ$). Na Cavaleira, o fator de escala de profundidade $l = \frac{1}{\tan(45^\circ)} = 1.0$ (tamanho real). Na Cabinet, $\tan(\beta) = 2$, portanto $l = 0.5$ (profundidade reduzida pela metade), o que resulta em uma proporção visual mais realista e menos alongada.

### P3: *"Por que o Bresenham é considerado superior ao DDA para hardware?"*
**Resposta:** O DDA requer somas em ponto flutuante e chamadas à função `round()` em cada iteração, exigindo uma FPU (Floating-Point Unit). O Bresenham utiliza exclusivamente inteiros, adições, subtrações e multiplicações por 2 (que são simples deslocamentos de bits `<< 1`), tornando-o extremamente rápido e diretamente implementável em silício/microcontroladores simples.

### P4: *"Como vocês garantiram que o vértice E ficasse com Y negativo na Questão 2?"*
**Resposta:** Ao calcular o vetor normal de Newell da base, podemos alinhá-lo com $+Y$ ou com $-Y$. O programa testa a rotação resultante: caso $E_y$ ficasse positivo, a rotação é invertida para alinhar o normal com $-Y$, garantindo geometricamente que o ápice fique invertido ($Y_E = -17.65 < 0$).
