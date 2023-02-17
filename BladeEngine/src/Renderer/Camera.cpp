#include "Camera.hpp"

namespace BladeEngine {
Camera::Camera() {
  position = glm::vec2(0, 0);
  rotation = 0;
  projectionMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.1f, 10.0f);
  viewMatrix = glm::lookAt(glm::vec3(position, 0), glm::vec3(position, 1),
                           glm::vec3(0, 0, rotation));
}
Camera::Camera(glm::vec2 position) {
  this->position = position;
  rotation = 0;
  projectionMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.1f, 10.0f);
  viewMatrix = glm::lookAt(glm::vec3(position, 0), glm::vec3(position, 1),
                           glm::vec3(0, 0, rotation));
}
Camera::Camera(glm::vec2 position, float rotation) {
  this->position = position;
  this->rotation = rotation;
  projectionMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.1f, 10.0f);
  viewMatrix = glm::lookAt(glm::vec3(position, 0), glm::vec3(position, 1),
                           glm::vec3(0, 0, rotation));
}
Camera::Camera(float rotation) {
  position = glm::vec2(0, 0);
  this->rotation = rotation;
  projectionMatrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f, 0.1f, 10.0f);
  viewMatrix = glm::lookAt(glm::vec3(position, 0), glm::vec3(position, 1),
                           glm::vec3(0, 0, rotation));
}
Camera::~Camera() {}

glm::vec2 Camera::GetPosition() { return position; }
float Camera::GetRotation() { return rotation; }

void Camera::SetPosition(glm::vec2 position) { this->position = position; }
void Camera::SetRotation(float rotation) { this->rotation = rotation; }
} // namespace BladeEngine