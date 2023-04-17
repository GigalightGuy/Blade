#pragma once
#include "vulkan/vulkan_core.h"
#include <vector>
#include <vulkan/vulkan.h>

namespace BladeEngine {
namespace Graphics {
namespace Vulkan {

struct VulkanCommandPool {
  VkCommandPool commandPool;
  std::vector<VkCommandBuffer> commandBuffer;

  void CreateCommandPool(VkPhysicalDevice physicalDevice, VkDevice device,
                         VkSurfaceKHR surface);
};

static VkCommandBuffer BeginSingleTimeCommands(VkCommandPool commandPool);
static void EndSingleTimeCommands(VkDevice device, VkQueue graphicsQueue,
                                  VkCommandPool commandPool,
                                  VkCommandBuffer commandBuffer);

} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
