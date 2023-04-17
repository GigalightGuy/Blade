#pragma once
#include "../../Texture2D.hpp"
#include "BladeVulkanSwapchain.hpp"
#include <vulkan/vulkan.h>

namespace BladeEngine {
namespace Graphics {
namespace Vulkan {

class VulkanTexture {
public:
  VulkanTexture(VkPhysicalDevice physicalDevice, VkDevice device,
                VkQueue graphicsQueue, VkCommandPool commandPool,
                VulkanSwapchain *swapchain, Texture2D *texture);

  ~VulkanTexture();
    
  void Dispose(VkDevice device);
  VkImage textureImage;
  VkDeviceMemory textureImageMemory;
  VkImageView textureImageView;
  VkSampler textureSampler;

private:
  void CreateTextureImage(VkPhysicalDevice physicalDevice, VkDevice device,
                          VkQueue graphicsQueue, VkCommandPool commandPool,Texture2D *texture);
  void CreateTextureImageView(VkDevice device, VulkanSwapchain *swapchain);
  void CreateTextureSampler(VkPhysicalDevice physicalDevice, VkDevice device);
};

} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
