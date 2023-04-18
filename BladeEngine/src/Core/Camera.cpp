#include "Camera.hpp"

using namespace BladeEngine;

BladeEngine::Camera::Camera(glm::vec2 dimensions, glm::vec2 position,
                            float rotation, float nearPlane, float farPlane,
                            BladeEngine::CameraType cameraType) {
  this->dimensions = dimensions;
  this->rotation = glm::vec3(0, 0, rotation);
  this->position = glm::vec3(position, 10);
  this->type = cameraType;
  this->nearPlane = nearPlane;
  this->farPlane = farPlane;

  this->scale = 0.5f;

  GenerateMatrices();
}

BladeEngine::Camera::Camera(glm::vec2 dimensions, glm::vec3 position,
                            glm::vec3 rotation, float nearPlane, float farPlane,
                            BladeEngine::CameraType cameraType) {
  this->dimensions = dimensions;
  this->rotation = rotation;
  this->position = position;
  this->type = cameraType;
  this->nearPlane = nearPlane;
  this->farPlane = farPlane;

  this->scale = 1.0f / 6.0f;

  GenerateMatrices();
}

BladeEngine::Camera::~Camera() {}

void BladeEngine::Camera::Resize(glm::vec2 dimensions) {
  this->dimensions = dimensions;
  GenerateMatrices();
}

void BladeEngine::Camera::SetPosition(glm::vec2 position) {
  this->position = glm::vec3(position, 0);
  GenerateMatrices();
}

void BladeEngine::Camera::SetPosition(glm::vec3 position) {
  this->position = position;
  GenerateMatrices();
}

void BladeEngine::Camera::SetRotation(float rotation) {
  this->rotation = glm::vec3(0, 0, rotation);
  GenerateMatrices();
}

void BladeEngine::Camera::SetRotation(glm::vec3 rotation) {
  this->rotation = rotation;
  GenerateMatrices();
}

float BladeEngine::Camera::Aspect() { return (dimensions.x / dimensions.y); }

float BladeEngine::Camera::GetHeight() { return dimensions.y; }

float BladeEngine::Camera::GetWidth() { return dimensions.x; }

glm::mat4 BladeEngine::Camera::GetViewMatrix() { return viewMatrix; }

glm::mat4 BladeEngine::Camera::GetProjectionMatrix() {
  auto proj = projectionMatrix;

  proj[1][1] *= -1; // Vulkan Specific alteration of projection matrix

  return proj;
}

void BladeEngine::Camera::GenerateMatrices() {
  viewMatrix = glm::lookAt(position, glm::vec3(0, 20, 0), glm::vec3(0, 1, 0));

  if (type == CameraType::PERSPECTIVE) {
    float fov = 360.0f * scale;
    projectionMatrix =
        glm::perspective(glm::radians(fov), Aspect(), nearPlane, farPlane);
  } else {
    float h = 100 * scale;
    float w = Aspect() * h;
    projectionMatrix = glm::ortho(-(w / 2.0f), (w / 2.0f), -(h / 2.0f),
                                  h / 2.0f, nearPlane, farPlane);
  }
}
