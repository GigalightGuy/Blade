#include "GraphicsManager.hpp"
#include "Mesh.hpp"

#if BLADE_VULKAN_API
    #include "Platform/Vulkan/BladeVulkanRenderer.hpp"
#endif

using namespace BladeEngine::Graphics;

GraphicsManager* GraphicsManager::s_Instance;

GraphicsManager::GraphicsManager(Window* window)
{
    mainCamera = new Camera(20.0f, glm::vec2(0.0f, 0.0f), 0, 0.1f, 1000.0f, CameraType::ORTHOGRAPHIC);   
    InitRenderer(window);
}

GraphicsManager::~GraphicsManager()
{

}

void GraphicsManager::Init(Window* window)
{
    s_Instance = new GraphicsManager(window);

    s_Instance->vkRenderer->CreateDebugResources();
}

void GraphicsManager::Shutdown()
{
    delete s_Instance;
    s_Instance = nullptr;
}

void GraphicsManager::SetMainCamera(Camera* camera)
{
    delete mainCamera;
    mainCamera = camera;
}


void GraphicsManager::InitRenderer(Window* window)
{
    //#if BLADE_VULKAN_API
    vkRenderer = new Vulkan::VulkanRenderer(mainCamera, window);
    //#endif
}

void GraphicsManager::Dispose()
{
    //#if BLADE_VULKAN_API
    //vkRenderer->Dispose();
    //#endif
}

void GraphicsManager::Clear(Color color)
{
    vkRenderer->Clear(color);
}


void GraphicsManager::BeginDrawing()
{
    vkRenderer->BeginDrawing();
}

void GraphicsManager::BeginDrawing(
    BladeEngine::Graphics::Shader *vertexShader,
    BladeEngine::Graphics::Shader * fragmentShader)
{
    
}

void GraphicsManager::DrawSprite(
    Texture2D *texture, 
    const glm::mat4& transform, 
    const glm::vec4& uvTransform)
{
    vkRenderer->DrawSprite(texture, transform, uvTransform);
}

void GraphicsManager::DrawString(
    const std::string& string, 
    Font* font, 
    const glm::mat4& transform)
{
    vkRenderer->DrawString(string, font, transform);
}

void GraphicsManager::EndDrawing()
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

void* GraphicsManager::UploadMeshToGPU(Buffer vertices, Buffer indices)
{
    return vkRenderer->UploadMeshToGPU(vertices, indices);
}

void GraphicsManager::ReleaseGPUMesh(void* gpuMesh)
{
    vkRenderer->ReleaseGPUMesh((Vulkan::VulkanMesh*)gpuMesh);
}

void GraphicsManager::WaitDeviceIdle()
{
    vkRenderer->WaitDeviceIdle();
}

void GraphicsManager::RecreateSwapchain(uint32_t width, uint32_t height)
{
    vkRenderer->RecreateSwapchain(width, height);
}