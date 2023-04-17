#pragma once

#include "../Core/Camera.hpp"
#include "Texture2D.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"
#include "Color.hpp"
#include "../Core/Window.hpp"

namespace BladeEngine::Graphics::Vulkan
{
    class VulkanRenderer;
}

namespace BladeEngine {
namespace Graphics {

class GraphicsManager {
public:
    GraphicsManager(Window* window);
    ~GraphicsManager();

    static void Init(Window* window);
    static void Shutdown();
    
    BladeEngine::Window* GetWindow();
    
    /*Set the main camera of renderer*/
    void SetMainCamera(BladeEngine::Camera* camera);
    
    /*Clear All buffers in renderer*/
    void Clear(BladeEngine::Graphics::Color color);
    /*Begins Drawing commands with default shader*/
    void BeginDrawing();
    /*Begins Drawing commands with custom shader*/
    void BeginDrawing(BladeEngine::Graphics::Shader *vertexShader,BladeEngine::Graphics::Shader * fragmentShader);
    /*Draws a Quad with the selected texture and the current active shader program*/
    void Draw(BladeEngine::Graphics::Texture2D *texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale);
    /*Stops the rendering*/
    void EndDrawing();

    void* CreateGPUTexture(Texture2D* texture);
    void DestroyGPUTexture(void* gpuTexture);

    inline static GraphicsManager* Instance() { return s_Instance; }

private:
    BladeEngine::Camera *mainCamera;
    void InitRenderer(Window* window);
    void Dispose();

private:
    static GraphicsManager* s_Instance;
    
    #if BLADE_VULKAN_API
        Vulkan::VulkanRenderer* vkRenderer;  // This should be somewhere else, no need to include all vulkan stuff in header just because of this
    #endif
};

} // namespace Graphics
} // namespace BladeEngine
