#pragma once

#define FRAMES_IN_FLIGHT 3

#include "BladeVulkanInstance.hpp"
#include "BladeVulkanDevice.hpp"
#include "BladeVulkanSwapchain.hpp"
#include "BladeVulkanShader.hpp"
#include "BladeVulkanGraphicsPipeline.hpp"
#include "BladeVulkanMesh.hpp"
#include "BladeVulkanTexture.hpp"

//#define BLADE_VULKAN_API true
#include "../../../Core/Window.hpp"
#include "../../../Core/Camera.hpp"
#include "../../Color.hpp"
#include "../../Shader.hpp"
#include "../../Mesh.hpp"

#include <map>

namespace BladeEngine {
namespace Graphics {
namespace Vulkan {
    
class VulkanRenderer {
public:
    VulkanRenderer(BladeEngine::Camera *camera, Window* window);
    ~VulkanRenderer();
    void Dispose();
    
    void Clear(BladeEngine::Graphics::Color color);

    // Creates Graphics pipeline for default shaders to be used during draw calls
    void BeginDrawing();
    // Create and store Vulkan Texture, store model data, store quad mesh, create and allocate descriptor pools and sets
    void Draw(BladeEngine::Graphics::Texture2D *texture, glm::vec3 position,glm::vec3 rotation,glm::vec3 scale);
    // Create Descriptor Pool, Descriptor sets, update descriptor sets, uniform buffers, drawindexed
    void EndDrawing();

    VulkanTexture* CreateGPUTexture(Texture2D* texture);
    void DestroyGPUTexture(VulkanTexture* gpuTexture);
    

private:
    void Init(Window* window);
    void CreateClearRenderPass();
    void CreateClearFramebuffer();
    
    void CreateCommandPool();
    void CreateCommandBuffers();
    void CreateSyncObjects();
    
    void RecordCommandBuffer();
    
    void DrawFrame();
    
    struct ModelData
    {
        glm::vec3 position;
        glm::vec3 rotation;
        glm::vec3 scale;
    };
    
    uint32_t currentFrame = 0;
    uint32_t imageIndex = -1;
    
    //Init
    BladeEngine::Graphics::Color backgroundColor;
    BladeEngine::Camera *camera;
    BladeEngine::Graphics::Vulkan::VulkanInstance* vkInstance;
    VkSurfaceKHR vkSurface;
    BladeEngine::Graphics::Vulkan::VulkanDevice* vkDevice;
    BladeEngine::Graphics::Vulkan::VulkanSwapchain *vkSwapchain;

    //Clear dependencies
    BladeEngine::Graphics::Vulkan::VulkanSwapchain *vkClearSwapchain;
    VkRenderPass vkClearRenderPass;
    std::vector<VkFramebuffer> vkClearFramebuffers;
    
    BladeEngine::Graphics::Shader *defaultVertexShader;
    BladeEngine::Graphics::Shader *defaultFragmentShader;
    
    //Render Loop
    std::vector<BladeEngine::Graphics::Vulkan::VulkanGraphicsPipeline> graphicsPipelines;
    
    std::vector<BladeEngine::Graphics::Vulkan::VulkanTexture*> vkTextures;
    std::vector<BladeEngine::Graphics::Vulkan::VulkanMesh> vkMeshes;
    std::vector<ModelData> vkMeshesModelData; 
    
    //Draw
    VkCommandPool vkCommandPool;
    std::vector<VkCommandBuffer> commandBuffers;
    
    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
};
} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
