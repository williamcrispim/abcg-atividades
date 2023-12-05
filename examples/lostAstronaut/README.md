# Atividade 3

**Integrantes:**

| Nome    | RA |
| -------- | ------- |
| William Crispim da Silva  | 11201722180    |

## Requisitos da Entrega

1. [Link do repositório contendo o código-fonte](https://github.com/williamcrispim/abcg-atividades/tree/main/examples/lostAstronaut).
2. [Link para uma página web com a aplicação rodando em WebAssembly](https://williamcrispim.github.io/abcg-atividades/lostAstronaut/).

* [Link do YouTube para a demostração (problema de compilação WebAssembly no Windows)](https://youtu.be/0s3Egb7TvII).

## Detalhes do Projeto Lost Astronaut

O projeto Lost Astronaut é uma modificação do projeto [starfield](https://hbatagelo.github.io/cg/starfield.html) demonstrado em sala de aula. A principal modificação foi a adição de um astronauta em meio ao espaço, quando interagimos com o mouse sob o astronauta cria-se um efeito parecido com um astronauta perdido no espaço em alta velocidade, pode-se observar uma ilusão de como o astronauta estivesse caindo no espaço, e quando paramos o mesmo, as estrelas voltam ao estado inicial de como estivesse somente vagando pelo espaço.

Busquei com esse projeto aplicar os principais requisitos solicitados na atividade como, por exemplo, gerar os modelos geométricos de forma procedural (geração das estrelas), explorar o uso de composição de transformações geométricas (astronauta) e a implementação de novos shaders ([astronaut.vert](https://github.com/williamcrispim/abcg-atividades/blob/main/examples/lostAstronaut/assets/astronaut.vert), [astronaut.frag](https://github.com/williamcrispim/abcg-atividades/blob/main/examples/lostAstronaut/assets/astronaut.frag) e [star.vert](https://github.com/williamcrispim/abcg-atividades/blob/main/examples/lostAstronaut/assets/star.vert)).

Tive bastante dificuldade para seguir com o projeto da atividade 2 que tratava-se de algo mais complexo, então decidi focar em uma implementação básica para treinar os conceitos aprendidos em aula.

Ao longo do código fui adicionando comentários para melhor localização e também para melhor entendimento de quem irá ler ou dar manutenção.

## Principais Funções ([window.cpp](https://github.com/williamcrispim/abcg-atividades/blob/main/examples/lostAstronaut/window.cpp))

### `generateStar`:
- Esta função é responsável por criar e configurar os dados necessários para renderizar a estrela.
- Define as coordenadas dos vértices, normais e índices da estrela.
- Gera e configura um Vertex Array Object (VAO) e três Vertex Buffer Objects (VBOs) para armazenar as informações da estrela.
- Usa a API OpenGL para configurar os atributos dos VBOs (coordenadas, normais) e os índices dos triângulos.
- Libera os recursos dos VBOs após a configuração.

### `onEvent`:
- Manipula eventos relacionados ao mouse, como movimento, clique e rolagem.
- Atualiza a posição do mouse e responde a diferentes tipos de eventos, como movimento, clique e rolagem.
- Usa um objeto `m_trackBall` para rastrear a interação do usuário com a cena.

### `onCreate`:
- Configurações iniciais quando a janela é criada.
- Define a cor de fundo para preto e habilita o teste de profundidade.
- Carrega e compila shaders para dois programas OpenGL (star e astronaut).
- Gera dados para uma estrela usando a função `generateStar`.
- Carrega um modelo 3D de um arquivo .obj (astronaut) e configura seu Vertex Array Object (VAO).

### `randomizeStar`:
- Gera posições e eixos de rotação aleatórios para estrelas.
- Usada para inicializar as posições das estrelas no início ou ao reiniciar.

### `onUpdate`:
- Atualiza lógica de animação e interação.
- Atualiza o ângulo de rotação, a posição das estrelas e a matriz de modelo para o modelo 3D do astronauta.
- Controla o campo de visão (FOV) com base na rotação da bola de rastreamento.

### `onPaint`:
- Renderiza as estrelas como cubos usando o programa star.
- Renderiza o modelo 3D do astronauta usando o programa astronaut.
- Configura matrizes de visualização, projeção e modelo nos shaders.

## Shaders Modificados ou Criados ([astronaut.vert](https://github.com/williamcrispim/abcg-atividades/blob/main/examples/lostAstronaut/assets/astronaut.vert), [astronaut.frag](https://github.com/williamcrispim/abcg-atividades/blob/main/examples/lostAstronaut/assets/astronaut.frag) e [star.vert](https://github.com/williamcrispim/abcg-atividades/blob/main/examples/lostAstronaut/assets/star.vert))

**astronaut.vert:** Ele recebe posições e normais de vértices, calcula a posição final no espaço de recorte e a normal do fragmento no espaço da câmera. Usa matrizes de transformação para modelagem, visualização e projeção. A normal é ajustada para preservar a orientação correta após as transformações.

**astronaut.frag:** Ele gera um gradiente vertical de cores, misturando as cores da bandeira do Brasil com base na coordenada Y do fragmento. A cor resultante é então misturada com azul, formando a cor final do fragmento. Essa cor é atribuída à saída do shader para ser exibida na tela.

**star.vert:** Ele transforma a posição do vértice para o espaço da câmera e calcula a intensidade da cor com base na profundidade do vértice. A intensidade é combinada com a cor recebida como atributo, resultando na cor final do vértice. A posição final do vértice no espaço de projeção é calculada e atribuída a gl_Position para a renderização correta na tela.

## Requisitos da Atividade

- [x] Gere os modelos geométricos de forma procedural.

- [x] Explore o uso de composição de transformações geométricas.

- [x] Implemente novos shaders.

- [x] Cuide do visual e da apresentação da aplicação.
