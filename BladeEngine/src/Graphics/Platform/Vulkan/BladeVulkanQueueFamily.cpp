#include "BladeVulkanQueueFamily.hpp"

using namespace BladeEngine::Graphics::Vulkan;

bool GraphicsFamily::IsComplete() {
  return graphicsFlagIndex.has_value() && presentFlagIndex.has_value();
}

bool ComputeFamily::IsComplete() { return computeFlagIndex.has_value(); }

bool TransferFamily::IsComplete() { return transferFlagIndex.has_value(); }

TransferFamily BladeEngine::Graphics::Vulkan::GetTransferFamily(
    VkPhysicalDevice physicalDevice) {
  TransferFamily family;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {

    if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      family.transferFlagIndex = i;
    }

    if (family.IsComplete()) {
      break;
    }

    i++;
  }
  return family;
}

ComputeFamily BladeEngine::Graphics::Vulkan::GetComputeFamily(
    VkPhysicalDevice physicalDevice) {
  ComputeFamily family;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {

    if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      family.computeFlagIndex = i;
    }

    if (family.IsComplete()) {
      break;
    }

    i++;
  }
  return family;
}

GraphicsFamily BladeEngine::Graphics::Vulkan::GetGraphicsFamily(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) {
  GraphicsFamily family;

  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           nullptr);

  std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount,
                                           queueFamilies.data());

  int i = 0;
  for (const auto &queueFamily : queueFamilies) {

    if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      family.graphicsFlagIndex = i;
    }

    VkBool32 presentSupport = false;
    vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface,
                                         &presentSupport);

    if (presentSupport) {
      family.presentFlagIndex = i;
    }

    if (family.IsComplete()) {
      break;
    }

    i++;
  }
  return family;
}
