#pragma once
#include <cstdint>
#include <iostream>
#include <vector>
#include <vulkan/vulkan.h>

namespace BladeEngine {

namespace Graphics {

namespace Vulkan {

class VulkanInstance {
public:
  VulkanInstance(const char *applicationName, const uint32_t applicationVersion,
                 std::vector<const char *> instanceExtensions,
                 std::vector<const char *> validationLayers);
  ~VulkanInstance();
  void Dispose();
  VkInstance instance;

private:
  VkDebugUtilsMessengerEXT debugMessenger;
  // Helper Functions
  void PopulateDebugMessengerCreateInfo(
      VkDebugUtilsMessengerCreateInfoEXT &createInfo);

  // Create Functions
  void CreateVulkanInstance();
  void CreateDebugMessenger();
};

} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
