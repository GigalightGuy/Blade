#include "Camera.hpp"

#include "Window.hpp"

namespace BladeEngine
{
    Camera* Camera::s_MainCamera = nullptr;

    Camera::Camera(float size, glm::vec2 position, float rotation, float nearPlane,float farPlane,BladeEngine::CameraType cameraType)
    {
        this->rotation = glm::vec3(0,0,rotation);
        this->position = glm::vec3(position,100);
        this->type = cameraType;
        this->nearPlane = nearPlane;
        this->farPlane = farPlane;
    
        this->size = size;
        
        GenerateMatrices();

        if (!s_MainCamera) s_MainCamera = this;
    }
    
    Camera::Camera(glm::vec3 position, glm::vec3 rotation,float nearPlane,float farPlane, BladeEngine::CameraType cameraType)
    {
        this->rotation = rotation;
        this->position = position;
        this->type = cameraType;
        this->nearPlane = nearPlane;
        this->farPlane = farPlane;
        
        this->size = 1.0f/6.0f;
    
        GenerateMatrices();

        if (!s_MainCamera) s_MainCamera = this;
    }
    
    Camera::~Camera()
    {
        if (s_MainCamera == this) s_MainCamera = nullptr;
    }
    
    void Camera::Resize(glm::vec2 dimensions)
    {
        this->dimensions = dimensions;
        GenerateMatrices();
    }
    
    void Camera::SetPosition(glm::vec2 position)
    {
        this->position = glm::vec3(position,0);
        GenerateMatrices();
    }
    
    void Camera::SetPosition(glm::vec3 position)
    {
        this->position = position;
        GenerateMatrices();
    }
    
    void Camera::SetRotation(float rotation)
    {
        this->rotation = glm::vec3(0,0,rotation);
        GenerateMatrices();
    }
    
    void Camera::SetRotation(glm::vec3 rotation)
    {    
        this->rotation = rotation;
        GenerateMatrices();
    }
    
    float Camera::Aspect()
    {
        return (dimensions.x/dimensions.y);
    }
    
    float Camera::GetHeight()
    {
        return dimensions.y;
    }
    
    float Camera::GetWidth()
    {
        return dimensions.x;
    }
    
    glm::mat4 Camera::GetViewMatrix()
    {
        return viewMatrix;
    }
    
    glm::mat4 Camera::GetProjectionMatrix()
    {
        auto proj = projectionMatrix;
        
        proj[1][1] *= -1; //Vulkan Specific alteration of projection matrix
    
        return proj;
    }
    
    void Camera::GenerateMatrices()
    {
        viewMatrix = glm::lookAt(
            position,
            glm::vec3(position.x, position.y,0), // TODO
            glm::vec3(0,1,0)
            );
    
        if(type == CameraType::PERSPECTIVE)
        {
            float fov = 360.0f * size;
            projectionMatrix = glm::perspective(glm::radians(fov),Aspect(),nearPlane,farPlane);
        }
        else
        {
            float h = size;
            float w = Window::GetViewportAspectRatio() * h;
            projectionMatrix = glm::ortho(- (w / 2.0f), (w / 2.0f), -(h / 2.0f), h / 2.0f, nearPlane, farPlane);
        }
    }
}


