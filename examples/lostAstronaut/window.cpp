#include "window.hpp"

#include <glm/gtc/random.hpp>
#include <glm/gtx/fast_trigonometry.hpp>

void Window::generateStar() {
  // Definição das coordenadas dos vértices da estrela
  std::vector<glm::vec3> vertices = {
      {-0.5f, -0.5f, -0.5f}, {-0.5f, 0.5f, -0.5f}, {0.5f, 0.5f, -0.5f},
      {0.5f, -0.5f, -0.5f},  {-0.5f, -0.5f, 0.5f}, {-0.5f, 0.5f, 0.5f},
      {0.5f, 0.5f, 0.5f},    {0.5f, -0.5f, 0.5f},
  };

  // Definição das normais dos vértices da estrela
  std::vector<glm::vec3> normals = {
      {0, 0, -1}, {0, 0, -1}, {0, 0, -1}, {0, 0, -1},
      {0, 0, 1},  {0, 0, 1},  {0, 0, 1},  {0, 0, 1},
  };

  // Definição dos índices que formam os triângulos da estrela
  std::vector<unsigned short> indices = {
      0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4,
  };

  // Geração e configuração do VAO da estrela
  m_starVAO = 0;
  abcg::glGenVertexArrays(1, &m_starVAO);
  abcg::glBindVertexArray(m_starVAO);

  // Geração e configuração dos VBOs da estrela
  GLuint starVBO[3];
  abcg::glGenBuffers(3, starVBO);

  // VBO para as coordenadas dos vértices
  abcg::glBindBuffer(GL_ARRAY_BUFFER, starVBO[0]);
  abcg::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
                     vertices.data(), GL_STATIC_DRAW);
  abcg::glEnableVertexAttribArray(0);
  abcg::glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // VBO para as normais dos vértices
  abcg::glBindBuffer(GL_ARRAY_BUFFER, starVBO[1]);
  abcg::glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
                     normals.data(), GL_STATIC_DRAW);
  abcg::glEnableVertexAttribArray(1);
  abcg::glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

  // VBO para os índices dos triângulos
  abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, starVBO[2]);
  abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     indices.size() * sizeof(unsigned short), indices.data(),
                     GL_STATIC_DRAW);

  // Desassociar do VAO para evitar modificações acidentais
  abcg::glBindVertexArray(0);

  // Liberação de recursos dos VBOs após configuração
  abcg::glDeleteBuffers(3, starVBO);
}

void Window::onEvent(SDL_Event const &event) {
  glm::ivec2 mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

  if (event.type == SDL_MOUSEMOTION) {
    m_trackBall.mouseMove(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONDOWN &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mousePress(mousePosition);
  }
  if (event.type == SDL_MOUSEBUTTONUP &&
      event.button.button == SDL_BUTTON_LEFT) {
    m_trackBall.mouseRelease(mousePosition);
  }
  if (event.type == SDL_MOUSEWHEEL) {
    m_zoom += (event.wheel.y > 0 ? -1.0f : 1.0f) / 5.0f;
    m_zoom = glm::clamp(m_zoom, -1.5f, 1.0f);
  }
}

void Window::onCreate() {
  auto const assetsPath{abcg::Application::getAssetsPath()};

  // Configura a cor de fundo para preto e habilita o teste de profundidade
  abcg::glClearColor(0, 0, 0, 1);
  abcg::glEnable(GL_DEPTH_TEST);

  m_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "star.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "star.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  generateStar();

  m2_program =
      abcg::createOpenGLProgram({{.source = assetsPath + "astronaut.vert",
                                  .stage = abcg::ShaderStage::Vertex},
                                 {.source = assetsPath + "astronaut.frag",
                                  .stage = abcg::ShaderStage::Fragment}});

  // Carrega o modelo do objeto "astronaut" a partir de um arquivo .obj
  m2_model.loadObj(assetsPath + "astronaut.obj");
  // Configura o VAO para o programa principal
  m2_model.setupVAO(m_program);

  // Armazena o número de triângulos no modelo para renderização
  m_trianglesToDraw = m2_model.getNumTriangles();

  // Configura a matriz de visualização para a câmera
  glm::vec3 const eye{0.0f, 0.0f, 0.0f};
  glm::vec3 const at{0.0f, 0.0f, -1.0f};
  glm::vec3 const up{0.0f, 1.0f, 0.0f};
  m_viewMatrix = glm::lookAt(eye, at, up);

  // Inicializa as estrelas com posições aleatórias
  for (auto &star : m_stars) {
    randomizeStar(star);
  }
}

void Window::randomizeStar(Star &star) {
  std::uniform_real_distribution<float> distPosXY(-20.0f, 20.0f);
  std::uniform_real_distribution<float> distPosZ(-100.0f, 0.0f);
  star.m_position =
      glm::vec3(distPosXY(m_randomEngine), distPosXY(m_randomEngine),
                distPosZ(m_randomEngine));

  star.m_rotationAxis = glm::sphericalRand(1.0f);
}

void Window::onUpdate() {
  // Obtém o tempo decorrido desde a última atualização da janela
  auto const deltaTime{gsl::narrow_cast<float>(getDeltaTime())};

  // Atualiza o ângulo de rotação com base no tempo decorrido
  m_angle = glm::wrapAngle(m_angle + glm::radians(90.0f) * deltaTime);

  // Atualiza a posição das estrelas no eixo z
  for (auto &star : m_stars) {
    star.m_position.z += deltaTime * 10.0f;

    if (star.m_position.z > 0.1f) {
      randomizeStar(star);
      star.m_position.z = -100.0f;
    }
  }

  // Verifica se a matriz de modelo mudou em relação à rotação
  if (m2_modelMatrix != m_trackBall.getRotation()) {
    m_FOV++;
    // Limita o FOV dentro de um intervalo específico
    m_FOV = glm::clamp(m_FOV, 5.0f, 179.0f);
  } else {
    // Se não houve mudança, define o FOV para um valor padrão
    m_FOV = 25.0f;
  }

  // Atualiza a matriz de modelo com a rotação
  m2_modelMatrix = m_trackBall.getRotation();

  // Atualiza a matriz de visualização com base na posição da câmera
  m2_viewMatrix =
      glm::lookAt(glm::vec3(0.0f, 0.0f, 4.0f + m_zoom),
                  glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
}

void Window::onPaint() {
  abcg::glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  abcg::glUseProgram(m_program);

  // Obtém os locais das variáveis uniformes nos shaders
  auto const viewMatrixLoc{abcg::glGetUniformLocation(m_program, "viewMatrix")};
  auto const projMatrixLoc{abcg::glGetUniformLocation(m_program, "projMatrix")};
  auto const modelMatrixLoc{
      abcg::glGetUniformLocation(m_program, "modelMatrix")};
  auto const colorLoc{abcg::glGetUniformLocation(m_program, "color")};

  // Configura as matrizes de visualização e projeção e a cor global no programa
  // principal
  abcg::glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, &m_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, &m_projMatrix[0][0]);
  abcg::glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);

  // Renderiza as estrelas usando o programa principal
  for (auto &star : m_stars) {
    // Configura a matriz de modelo para a estrela atual
    glm::mat4 modelMatrix{1.0f};
    modelMatrix = glm::translate(modelMatrix, star.m_position);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f));
    modelMatrix = glm::rotate(modelMatrix, m_angle, star.m_rotationAxis);

    // Atualiza a matriz de modelo no shader
    abcg::glUniformMatrix4fv(modelMatrixLoc, 1, GL_FALSE, &modelMatrix[0][0]);

    // Gera uma cor aleatória para a estrela
    glm::vec3 starColor = glm::mix(glm::vec3(1.0f), glm::vec3(1.0f, 1.0f, 0.0f),
                                   glm::linearRand(0.0f, 1.0f));
    // Atualiza a cor no shader
    abcg::glUniform4f(colorLoc, starColor.r, starColor.g, starColor.b, 1.0f);

    // Renderiza o cubo associado à estrela
    abcg::glBindVertexArray(m_starVAO);
    abcg::glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    abcg::glBindVertexArray(0);
  }

  abcg::glUseProgram(m2_program);

  // Atualiza as matrizes de visualização, projeção e modelo no programa
  // secundário
  abcg::glUniformMatrix4fv(abcg::glGetUniformLocation(m2_program, "viewMatrix"),
                           1, GL_FALSE, &m2_viewMatrix[0][0]);
  abcg::glUniformMatrix4fv(abcg::glGetUniformLocation(m2_program, "projMatrix"),
                           1, GL_FALSE, &m2_projMatrix[0][0]);
  abcg::glUniformMatrix4fv(
      abcg::glGetUniformLocation(m2_program, "modelMatrix"), 1, GL_FALSE,
      &m2_modelMatrix[0][0]);
  abcg::glUniform4f(abcg::glGetUniformLocation(m2_program, "color"), 1.0f, 1.0f,
                    1.0f, 1.0f);

  // Renderiza o modelo do astronauta
  m2_model.render(431212);

  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  abcg::OpenGLWindow::onPaintUI();
  {
    auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                      gsl::narrow<float>(m_viewportSize.y)};

    // Sempre assume projeção em Perspectiva
    m_projMatrix = glm::perspective(glm::radians(m_FOV), aspect, 0.01f, 100.0f);
  }

  {
    static std::size_t currentIndex{};
    std::vector<std::string> const rotationComboItems{"CCW", "CW"};

    if (currentIndex == 0) {
      abcg::glFrontFace(GL_CCW);
    } else {
      abcg::glFrontFace(GL_CW);
    }

    static std::size_t projectionIndex{};
    std::vector<std::string> const projectionComboItems{"Perspective",
                                                        "Orthographic"};

    if (projectionIndex == 0) {
      auto const aspect{gsl::narrow<float>(m_viewportSize.x) /
                        gsl::narrow<float>(m_viewportSize.y)};
      m2_projMatrix = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 5.0f);
    } else {
      m2_projMatrix = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 5.0f);
    }
  }
}

void Window::onResize(glm::ivec2 const &size) {
  m_viewportSize = size;
  m_trackBall.resizeViewport(size);
}

void Window::onDestroy() {
  m_model.destroy();
  m2_model.destroy();
  abcg::glDeleteVertexArrays(1, &m_starVAO);
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteProgram(m2_program);
}
