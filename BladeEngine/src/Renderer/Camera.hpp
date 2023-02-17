#pragma once
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace BladeEngine {
/*
    Simple Camera Implementation where the user creates
    a simple 2D orthographic camera projection

    Camera should define the window size for the renderer
*/
class Camera {
private:
  glm::vec2 position;
  float rotation;
  glm::mat4 projectionMatrix;
  glm::mat4 viewMatrix;

public:
  Camera();
  Camera(glm::vec2 position);
  Camera(glm::vec2 position, float rotation);
  Camera(float rotation);
  ~Camera();

  glm::vec2 GetPosition();
  float GetRotation();

  void SetPosition(glm::vec2 position);
  void SetRotation(float rotation);
};
} // namespace BladeEngine