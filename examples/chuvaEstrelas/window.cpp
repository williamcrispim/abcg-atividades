#include "window.hpp"
#include "imgui.h"

#include "window.hpp"

void Window::onCreate() {
  auto const *vertexShader{R"gl(#version 300 es

    layout(location = 0) in vec2 inPosition;
    layout(location = 1) in vec4 inColor;

    uniform vec2 translation;
    uniform float scale;

    out vec4 fragColor;

    void main() {
      vec2 newPosition = inPosition * scale + translation;
      gl_Position = vec4(newPosition, 0, 1);
      fragColor = inColor;
    }
  )gl"};

  auto const *fragmentShader{R"gl(#version 300 es

    precision mediump float;  

    in vec4 fragColor;

    out vec4 outColor;

    void main() { outColor = fragColor; }
  )gl"};

  m_program = abcg::createOpenGLProgram(
      {{.source = vertexShader, .stage = abcg::ShaderStage::Vertex},
       {.source = fragmentShader, .stage = abcg::ShaderStage::Fragment}});

  abcg::glClearColor(0, 0, 0, 1);
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  m_randomEngine.seed(
      std::chrono::steady_clock::now().time_since_epoch().count());
}

void Window::onPaint() {
  if (m_timer.elapsed() < m_delay / 1000.0)
    return;
  m_timer.restart();

  // Criação de um polígono regular com número de lados 4
  auto const sides{4};
  setupModel(sides);

  // Limpa o buffer de cor no início de cada quadro
  abcg::glClear(GL_COLOR_BUFFER_BIT);

  // Configuração da viewport
  abcg::glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);

  // Uso do programa OpenGL
  abcg::glUseProgram(m_program);

  // Escolha aleatória de posição (xy) entre (-1,-1) e (1,1)
  std::uniform_real_distribution rd1(-1.0f, 1.0f);
  glm::vec2 const translation{rd1(m_randomEngine), rd1(m_randomEngine)};
  auto const translationLocation{
      abcg::glGetUniformLocation(m_program, "translation")};
  abcg::glUniform2fv(translationLocation, 1, &translation.x);

  // Escolha aleatória de um fator de escala (5% a 8%)
  std::uniform_real_distribution rd2(0.05f, 0.08f);
  auto const scale{rd2(m_randomEngine)};
  auto const scaleLocation{abcg::glGetUniformLocation(m_program, "scale")};
  abcg::glUniform1f(scaleLocation, scale);

  // Renderização
  abcg::glBindVertexArray(m_VAO);
  abcg::glDrawArrays(GL_TRIANGLE_FAN, 0, sides + 2);
  abcg::glBindVertexArray(0);

  // Desativa o programa OpenGL
  abcg::glUseProgram(0);
}

void Window::onPaintUI() {
  // A classe pai mostrará o botão de tela cheia e o medidor de FPS
  abcg::OpenGLWindow::onPaintUI();

  // Our own ImGui widgets go below
  {
    // Painel de controle
    // Defina a posição inicial da próxima janela (x, y)
    // ImVec2(FLT_MAX, FLT_MAX) especifica que não há restrições máximas,
    // permitindo que o usuário redimensione a janela para qualquer tamanho.
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 125),
                                        ImVec2(FLT_MAX, FLT_MAX));
    ImGui::SetNextWindowPos(ImVec2(5, 80), ImGuiCond_FirstUseEver);
    ImGui::Begin("Painel de Controle");

    // Editor de cores
    ImGui::Text("Cor das estrelas:");
    ImGui::ColorEdit3("", m_clearColor.data());

    // Caixa de entrada de texto
    // Buffer para armazenar o texto inserido
    static char buffer[256] = "";
    ImGui::Text("Digite uma mensagem:");
    ImGui::InputText("", buffer, sizeof(buffer));

    // Fim do painel de controle
    ImGui::End();

    // Mostre o texto na tela
    // Defina a posição inicial da próxima janela (x, y)
    // ImVec2(FLT_MAX, FLT_MAX) especifica que não há restrições máximas,
    // permitindo que o usuário redimensione a janela para qualquer tamanho.
    ImGui::SetNextWindowSizeConstraints(ImVec2(200, 100),
                                        ImVec2(FLT_MAX, FLT_MAX));

    ImGui::SetNextWindowPos(ImVec2(250, 250), ImGuiCond_FirstUseEver);
    ImGui::Begin("Mensagem");

    ImGui::Text("%s", buffer);
    // Window end
    ImGui::End();
  }
}

void Window::onResize(glm::ivec2 const &size) {
  // Atualiza o tamanho da viewport
  m_viewportSize = size;

  // Limpa o buffer de cor
  abcg::glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onDestroy() {
  // Limpa os recursos alocados
  abcg::glDeleteProgram(m_program);
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);
}

void Window::setupModel(int sides) {
  // Libera recursos anteriores, se houver
  abcg::glDeleteBuffers(1, &m_VBOPositions);
  abcg::glDeleteBuffers(1, &m_VBOColors);
  abcg::glDeleteVertexArrays(1, &m_VAO);

  // Seleção de cores aleatórias para o gradiente radial
  glm::vec3 const color{m_clearColor[0], m_clearColor[1], m_clearColor[2]};
  // Garante que o número mínimo de lados seja 3
  sides = std::max(3, sides);

  std::vector<glm::vec2> positions;
  std::vector<glm::vec3> colors;

  // Centro do polígono
  positions.emplace_back(0, 0);
  colors.push_back(color);

  // Vértices da borda
  auto const step{M_PI * 2 / sides};
  for (auto const angle : iter::range(0.0, M_PI * 2, step)) {
    positions.emplace_back(std::cos(angle), std::sin(angle));
    colors.push_back(color);
  }

  // Duplicação do segundo vértice
  positions.push_back(positions.at(1));
  colors.push_back(color);

  // Geração do VBO de posições
  abcg::glGenBuffers(1, &m_VBOPositions);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(glm::vec2),
                     positions.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Geração do VBO de cores
  abcg::glGenBuffers(1, &m_VBOColors);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3),
                     colors.data(), GL_STATIC_DRAW);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Obtenção da localização dos atributos no programa
  auto const positionAttribute{
      abcg::glGetAttribLocation(m_program, "inPosition")};
  auto const colorAttribute{abcg::glGetAttribLocation(m_program, "inColor")};

  // Criação do VAO
  abcg::glGenVertexArrays(1, &m_VAO);

  // Vinculação dos atributos de vértices ao VAO atual
  abcg::glBindVertexArray(m_VAO);

  abcg::glEnableVertexAttribArray(positionAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOPositions);
  abcg::glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  abcg::glEnableVertexAttribArray(colorAttribute);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, m_VBOColors);
  abcg::glVertexAttribPointer(colorAttribute, 3, GL_FLOAT, GL_FALSE, 0,
                              nullptr);
  abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Fim da vinculação ao VAO atual
  abcg::glBindVertexArray(0);
}