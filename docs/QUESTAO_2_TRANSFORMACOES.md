# Documentação Técnica — Questão 2: Transformações Geométricas 3D

Este documento detalha o tratamento geométrico, a formulação matricial e a análise de consistência dos dados da Questão 2 desenvolvida no executável `questao2_transformacoes`.

---

## 1. Dados do Enunciado

O poliedro (pirâmide de base quadrilátera) é definido pelos seguintes vértices literais:
* $A = (0, 0, 0)$
* $B = (30, 5, -5)$
* $C = (25, 5, -20)$
* $D = (5, 0, -25)$
* $E = (15, 20, 12)$ (Ápice superior)

Arestas:
* Base: $AB, BC, CD, DA$
* Conectando ao ápice: $EA, EB, EC, ED$

---

## 2. Inconsistência Matemática: Teste de Coplanaridade da Base

O enunciado solicita:
> *"Polígono formado pela tétrade (A, B, C, D) paralelo ao plano XZ;"*

Para que quatro pontos $A, B, C, D$ possam se situar rigorosamente em um mesmo plano após qualquer transformação rígida (translação e rotação), eles **devem ser coplanares no espaço tridimensional original**.

### 2.1 Demonstração por Produto Misto
Tomando o vértice $D$ como âncora comum, definimos os três vetores que partem de $D$:
$$\vec{u} = A - D = (0 - 5, 0 - 0, 0 - (-25)) = (-5, 0, 25)$$
$$\vec{v} = B - D = (30 - 5, 5 - 0, -5 - (-25)) = (25, 5, 20)$$
$$\vec{w} = C - D = (25 - 5, 5 - 0, -20 - (-25)) = (20, 5, 5)$$

Calculando o produto vetorial $\vec{v} \times \vec{w}$:
$$\vec{v} \times \vec{w} = 
\begin{vmatrix}
\hat{i} & \hat{j} & \hat{k} \\
25 & 5 & 20 \\
20 & 5 & 5
\end{vmatrix} = 
\hat{i}(25 - 100) - \hat{j}(125 - 400) + \hat{k}(125 - 100) = (-75, 275, 25)$$

Calculando o produto escalar (produto misto) $\vec{u} \cdot (\vec{v} \times \vec{w})$:
$$\vec{u} \cdot (\vec{v} \times \vec{w}) = (-5) \cdot (-75) + 0 \cdot (275) + 25 \cdot (25) = 375 + 0 + 625 = \mathbf{1000} \ne 0$$

Como o volume do paralelepípedo formado pelos três vetores é $1000 \ne 0$, **os quatro vértices $A, B, C, D$ são comprovadamente não-coplanares**.

### 2.2 Implicação e Diretriz
Nenhuma transformação afim rígida (isometria) consegue transformar quatro pontos não-coplanares em um plano único sem distorcer o sólido. 
Conforme diretriz do projeto:
1. **Não alteramos silenciosamente os vértices.**
2. O software emite um aviso claro e neutro no terminal evidenciando o produto misto igual a $1000$.
3. Adota-se o **Vetor Normal de Newell** para encontrar o plano médio ótimo da base, demonstrando o alinhamento de forma elegante e transparente.

---

## 3. Sequência de Transformações Geométricas

O objetivo é transformar o sólido satisfazendo:
1. $D \to (0, 0, 0)$;
2. Vetor normal médio da base alinhado ao eixo $Y$ (fazendo a base ficar paralela ao plano $XZ$);
3. $E_y < 0$ (ápice posicionado abaixo da base).

### 3.1 Passo 1: Translação de $D$ para a Origem ($T$)
O vetor de translação necessário é o oposto das coordenadas de $D$:
$$\vec{T} = (-D_x, -D_y, -D_z) = (-5, 0, 25)$$

A matriz homogênea de translação correspondente é:
$$M_T = 
\begin{bmatrix}
1 & 0 & 0 & -5 \\
0 & 1 & 0 & 0 \\
0 & 0 & 1 & 25 \\
0 & 0 & 0 & 1
\end{bmatrix}$$

Aplicando sobre $D$:
$$D' = M_T \cdot D = (0, 0, 0)$$

---

### 3.2 Passo 2: Cálculo da Orientação Representativa da Base (Método de Newell)
Para um polígono fechado $V_0, V_1, \dots, V_{n-1}$, o método de Newell integra as projeções em cada plano coordenado:
$$N_x = \sum_{i=0}^{n-1} (y_i - y_{i+1})(z_i + z_{i+1})$$
$$N_y = \sum_{i=0}^{n-1} (z_i - z_{i+1})(x_i + x_{i+1})$$
$$N_z = \sum_{i=0}^{n-1} (x_i - x_{i+1})(y_i + y_{i+1})$$

Para a base $A(0,0,0) \to B(30,5,-5) \to C(25,5,-20) \to D(5,0,-25)$:
* $N_x = (0-5)(0-5) + (5-5)(-5-20) + (5-0)(-20-25) + (0-0)(-25+0) = 25 + 0 - 225 + 0 = -200$
* $N_y = (0+5)(0+30) + (-5+20)(30+25) + (-20+25)(25+5) + (-25-0)(5+0) = 150 + 825 + 150 - 125 = 1000$
* $N_z = (0-30)(0+5) + (30-25)(5+5) + (25-5)(5+0) + (5-0)(0+0) = -150 + 50 + 100 + 0 = 0$

Vetor resultante de Newell:
$$\vec{N} = (-200, 1000, 0)$$
Normalizando:
$$|\vec{N}| = \sqrt{(-200)^2 + 1000^2 + 0} = \sqrt{1040000} \approx 1019.8039$$
$$\hat{n} \approx (-0.196116, 0.980581, 0.0)$$

---

### 3.3 Passo 3: Rotação para o Eixo $Y$ com Inversão para $E_y < 0$ ($R$)
Desejamos rotacionar $\hat{n}$ para coincidir com o eixo $Y$.
Temos duas possibilidades de alinhamento com o eixo normal do plano $XZ$:
* Alvo $\hat{t}_1 = (0, 1, 0)$
* Alvo $\hat{t}_2 = (0, -1, 0)$

Se alinharmos com $+Y$, o sólido mantém o ápice voltado para cima ($E_y > 0$).
Portanto, para satisfazer rigorosamente a exigência do enunciado:
> *"Vértice E abaixo do polígono ABCD com sua coordenada Y negativa"*

Escolhemos o alvo descendente $\hat{t} = (0, -1, 0)$.

Parâmetros da rotação:
* **Eixo de rotação:** $\vec{a} = \text{normalize}(\hat{n} \times \hat{t}) = (0, 0, 1)$
* **Ângulo de rotação:** $\theta = \arccos(\text{clamp}(\hat{n} \cdot \hat{t}, -1, 1)) \approx 168.6901^\circ$

Matriz de rotação homogênea $M_R$:
$$M_R = 
\begin{bmatrix}
\cos\theta & -\sin\theta & 0 & 0 \\
\sin\theta & \cos\theta & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix} \approx
\begin{bmatrix}
-0.9806 & -0.1961 & 0 & 0 \\
0.1961 & -0.9806 & 0 & 0 \\
0 & 0 & 1 & 0 \\
0 & 0 & 0 & 1
\end{bmatrix}$$

---

### 3.4 Matriz Composta $M = M_R \cdot M_T$
$$M = 
\begin{bmatrix}
-0.9806 & -0.1961 & 0 & 4.9029 \\
0.1961 & -0.9806 & 0 & -0.9806 \\
0 & 0 & 1 & 25.0000 \\
0 & 0 & 0 & 1
\end{bmatrix}$$

---

## 4. Coordenadas Finais e Análise Residual

Multiplicando os vértices originais por $M$:
* $A_{\text{final}} = (4.9029, -0.9806, 25.0000)$
* $B_{\text{final}} = (-25.4951, 0.0000, 20.0000)$
* $C_{\text{final}} = (-20.5922, -0.9806, 5.0000)$
* $D_{\text{final}} = (0.0000, 0.0000, 0.0000)$
* $E_{\text{final}} = (-13.7281, -17.6505, 37.0000)$

### Verificação das Metas:
1. **$D$ na origem:** $(0, 0, 0) \implies$ **SUCESSO EXATO** (erro $< 10^{-12}$).
2. **$E$ abaixo da base:** $E_y = -17.6505 < 0 \implies$ **SUCESSO EXATO**.
3. **Plano $ABCD$ paralelo a $XZ$ ($Y = 0$):**
   * Resíduos verticais: $Y_A = -0.98$, $Y_B = 0.00$, $Y_C = -0.98$, $Y_D = 0.00$.
   * Os resíduos são uniformes e decorrem estritamente da não-coplanaridade do enunciado.

---

## 5. Implementação no Pipeline OpenGL

A visualização gráfica cumpre à risca a exigência:
```cpp
glPushMatrix();
// Sequência ModelView aplicada na ordem inversa das multiplicações:
glRotatef(rotAngleDeg, rotAxis.x, rotAxis.y, rotAxis.z);
glTranslatef(transVec.x, transVec.y, transVec.z);

// Renderiza o wireframe utilizando os vértices ORIGINAIS:
drawOriginalWireframe();
glPopMatrix();
```
Desta forma, os vértices não são modificados artificialmente no código, sendo transformados pelo pipeline do OpenGL.
