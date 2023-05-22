#include "GraphicsManager.hpp"
#include "Mesh.hpp"

#if BLADE_VULKAN_API
    #include "Platform/Vulkan/BladeVulkanRenderer.hpp"
#endif

using namespace BladeEngine::Graphics;

GraphicsManager* GraphicsManager::s_Instance;

BladeEngine::Graphics::GraphicsManager::GraphicsManager(Window* window)
{
    mainCamera = new BladeEngine::Camera(20.0f, glm::vec2(0.0f, 0.0f), 0, 0.1f, 100.0f, CameraType::ORTHOGRAPHIC);   
    InitRenderer(window);
}

GraphicsManager::~GraphicsManager()
{
    
}

void GraphicsManager::Init(Window* window)
{
    s_Instance = new GraphicsManager(window);
}

void GraphicsManager::Shutdown()
{
    delete s_Instance;
    s_Instance = nullptr;
}

void BladeEngine::Graphics::GraphicsManager::SetMainCamera(BladeEngine::Camera* camera)
{
    delete mainCamera;
    mainCamera = camera;
}


void BladeEngine::Graphics::GraphicsManager::InitRenderer(Window* window)
{
    //#if BLADE_VULKAN_API
    vkRenderer = new BladeEngine::Graphics::Vulkan::VulkanRenderer(mainCamera, window);
    //#endif
}

void BladeEngine::Graphics::GraphicsManager::Dispose()
{
    //#if BLADE_VULKAN_API
    //vkRenderer->Dispose();
    //#endif
}

void BladeEngine::Graphics::GraphicsManager::Clear(BladeEngine::Graphics::Color color)
{
    vkRenderer->Clear(color);
}


void BladeEngine::Graphics::GraphicsManager::BeginDrawing()
{
    vkRenderer->BeginDrawing();
}

void BladeEngine::Graphics::GraphicsManager::BeginDrawing(
    BladeEngine::Graphics::Shader *vertexShader,
    BladeEngine::Graphics::Shader * fragmentShader)
{
    
}

void BladeEngine::Graphics::GraphicsManager::DrawSprite(
    Texture2D *texture, 
    const glm::vec3& position,
    const glm::vec3& rotation,
    const glm::vec3& scale)
{
    vkRenderer->DrawSprite(texture,position,rotation,scale);
}

void BladeEngine::Graphics::GraphicsManager::DrawString(
    const std::string& string, 
    Font* font, 
    const glm::vec3& position, 
    const glm::vec3& rotation, 
    const glm::vec3& scale)
{
    vkRenderer->DrawString(string, font, position, rotation, scale);
}

void BladeEngine::Graphics::GraphicsManager::EndDrawing()
{
    vkRenderer->EndDrawing();
}

void* GraphicsManager::UploadTextureToGPU(Texture2D* texture)
{
    return vkRenderer->UploadTextureToGPU(texture);
}

void GraphicsManager::ReleaseGPUTexture(void* gpuTexture)
{
    vkRenderer->ReleaseGPUTexture((Vulkan::VulkanTexture*)gpuTexture);
}

void* BladeEngine::Graphics::GraphicsManager::UploadMeshToGPU(Buffer vertices, Buffer indices)
{
    return vkRenderer->UploadMeshToGPU(vertices, indices);
}

void BladeEngine::Graphics::GraphicsManager::ReleaseGPUMesh(void* gpuMesh)
{
    vkRenderer->ReleaseGPUMesh((Vulkan::VulkanMesh*)gpuMesh);
}

void BladeEngine::Graphics::GraphicsManager::WaitDeviceIdle()
{
    vkRenderer->WaitDeviceIdle();
}
