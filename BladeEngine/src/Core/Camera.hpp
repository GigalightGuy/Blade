#pragma once

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace BladeEngine {
/*
    Simple Camera Implementation where the user creates
    a simple 2D orthographic camera projection

    Camera should define the window size for the renderer
*/

enum CameraType
{
    PERSPECTIVE,
    ORTHOGRAPHIC,
};
class Camera {
private:
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    glm::vec2 dimensions;
    glm::vec3 position;
    glm::vec3 rotation;
    
    float scale;
    
    float nearPlane,farPlane;
    
    CameraType type;

    void GenerateMatrices();
public:
    Camera(glm::vec2 dimensions, glm::vec2 position = glm::vec2(0.0f, 0.0f), float rotation = 0.0f,float nearPlane = 0.1f,float farPlane = 100.0f,CameraType cameraType = CameraType::ORTHOGRAPHIC);
    Camera(glm::vec2 dimensions, glm::vec3 position = glm::vec3(0.0f, 0.0f,0.0f), glm::vec3 rotation= glm::vec3(0.0f, 0.0f,0.0f),float nearPlane = 0.1f,float farPlane = 100.0f ,CameraType cameraType = CameraType::PERSPECTIVE);
    ~Camera();
    
    void Resize(glm::vec2 dimensions);
    void SetPosition(glm::vec2 position);
    void SetPosition(glm::vec3 position);
    void SetRotation(float rotation);
    void SetRotation(glm::vec3 rotation);
    
    float Aspect();
    float GetWidth();
    float GetHeight();
    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetViewMatrix();
};
} // namespace BladeEngine
