#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#include <random>

#include "abcgOpenGL.hpp"
#include "model.hpp"
#include "trackball.hpp"

class Window : public abcg::OpenGLWindow {
protected:
  void onEvent(SDL_Event const &event) override;
  void onCreate() override;
  void onUpdate() override;
  void onPaint() override;
  void onPaintUI() override;
  void onResize(glm::ivec2 const &size) override;
  void onDestroy() override;

private:
  std::default_random_engine m_randomEngine;

  glm::ivec2 m_viewportSize{};

  Model m_model;
  Model m2_model{};
  int m_trianglesToDraw{};

  TrackBall m_trackBall;
  float m_zoom{};

  struct Star {
    glm::vec3 m_position{};
    glm::vec3 m_rotationAxis{};
  };

  std::array<Star, 500> m_stars;

  float m_angle{};

  // glm::mat4 m_modelMatrix{1.0f};
  glm::mat4 m_viewMatrix{1.0f};
  glm::mat4 m_projMatrix{1.0f};
  glm::mat4 m2_modelMatrix{1.0f};
  glm::mat4 m2_viewMatrix{1.0f};
  glm::mat4 m2_projMatrix{1.0f};
  float m_FOV{30.0f};

  GLuint m_program{};
  GLuint m2_program{};
  // GLuint starVBO[3];
  GLuint m_starVAO{};

  void randomizeStar(Star &star);
  void generateStar();
};

#endif