#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

namespace BladeEngine {

namespace Graphics {

namespace Vulkan {

struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};

SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                              VkSurfaceKHR surface);

class VulkanDevice {
public:
  VulkanDevice(VkInstance instance, VkSurfaceKHR surface,
               std::vector<const char *> extensions);
  ~VulkanDevice();
  void Dispose();  
  
  VkDevice logicalDevice;
  VkPhysicalDevice physicalDevice;
  VkQueue graphicsQueue;
  VkQueue presentQueue;

private:
  // Helper Functions
  bool CheckDeviceExtensionSupport(VkPhysicalDevice physicalDevice,
                                   std::vector<const char *> extensions);
                                   
  bool IsDeviceSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface,
                        std::vector<const char *> extensions);

  void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface,
                          std::vector<const char *> extensions);
  void CreateLogicalDevice(VkSurfaceKHR surface,
                           std::vector<const char *> extensions);
};
} // namespace Vulkan

} // namespace Graphics
} // namespace BladeEngine
