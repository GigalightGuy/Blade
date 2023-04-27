#include "GraphicsManager.hpp"
#include "Mesh.hpp"

#if BLADE_VULKAN_API
#include "Platform/Vulkan/BladeVulkanRenderer.hpp"
#endif

using namespace BladeEngine::Graphics;

GraphicsManager *GraphicsManager::s_Instance;

BladeEngine::Graphics::GraphicsManager::GraphicsManager(Window *window) {
  mainCamera = new BladeEngine::Camera(20.0f, glm::vec2(0.0f, 0.0f), 0, 0.1f,
                                       100.0f, CameraType::ORTHOGRAPHIC);
  InitRenderer(window);
}

GraphicsManager::~GraphicsManager() {}

void GraphicsManager::Init(Window *window) {
  s_Instance = new GraphicsManager(window);
}

void GraphicsManager::Shutdown() {
  delete s_Instance;
  s_Instance = nullptr;
}

void BladeEngine::Graphics::GraphicsManager::SetMainCamera(
    BladeEngine::Camera *camera) {
  delete mainCamera;
  mainCamera = camera;
}

void BladeEngine::Graphics::GraphicsManager::InitRenderer(Window *window) {
  //#if BLADE_VULKAN_API
  vkRenderer =
      new BladeEngine::Graphics::Vulkan::VulkanRenderer(mainCamera, window);
  //#endif
}

void BladeEngine::Graphics::GraphicsManager::Dispose() {
  //#if BLADE_VULKAN_API
  // vkRenderer->Dispose();
  //#endif
}

void BladeEngine::Graphics::GraphicsManager::Clear(
    BladeEngine::Graphics::Color color) {
  vkRenderer->Clear(color);
}

void BladeEngine::Graphics::GraphicsManager::BeginDrawing() {
  vkRenderer->BeginDrawing();
}

void BladeEngine::Graphics::GraphicsManager::BeginDrawing(
    BladeEngine::Graphics::Shader *vertexShader,
    BladeEngine::Graphics::Shader *fragmentShader) {}

void BladeEngine::Graphics::GraphicsManager::Draw(
    BladeEngine::Graphics::Texture2D *texture, glm::vec3 position,
    glm::vec3 rotation, glm::vec3 scale) {
  vkRenderer->Draw(texture, position, rotation, scale);
}

void BladeEngine::Graphics::GraphicsManager::EndDrawing() {
  vkRenderer->EndDrawing();
}

void *GraphicsManager::CreateGPUTexture(Texture2D *texture) {
  return vkRenderer->CreateGPUTexture(texture);
}

void GraphicsManager::DestroyGPUTexture(void *gpuTexture) {
  vkRenderer->DestroyGPUTexture((Vulkan::VulkanTexture *)gpuTexture);
}
