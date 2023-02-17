#include "QueueFamily.hpp"

namespace BladeEngine {

namespace Vulkan {

VkBool32 ComputeQueueFamilyIndices::isComplete() {
  return computeFamilyIndice.has_value();
}

VkBool32 GraphicsPresentQueueFramilyIndices::isComplete() {
  return graphicsFamilyIndice.has_value() && presentFamilyIndice.has_value();
}

VkBool32 ProtectedQueueFamilyIndices::isComplete() {
  return protectedFamilyIndice.has_value();
}

VkBool32 TransferQueueFamilyIndices::isComplete() {
  return transferFamilyIndice.has_value();
}

static GraphicsPresentQueueFramilyIndices
FindGraphicsPresentQueueFamilyIndices(VkPhysicalDevice device,
                                      VkSurfaceKHR *surface) {
  uint32_t queueFamilyPropertiesCount;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           nullptr);
  if (queueFamilyPropertiesCount <= 0) {
    std::runtime_error("Failed to get Queue Families from Physical Device");
  }

  GraphicsPresentQueueFramilyIndices queueFamilyIndices;

  std::vector<VkQueueFamilyProperties> queueFamilyProperties;
  queueFamilyProperties.resize(queueFamilyPropertiesCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           queueFamilyProperties.data());
  uint32_t i = 0;
  for (const auto &queueFamilyProperty : queueFamilyProperties) {
    VkBool32 presentable;
    if (queueFamilyProperty.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
      queueFamilyIndices.graphicsFamilyIndice = i;
      queueFamilyIndices.graphicsFamilyQueueCount =
          queueFamilyProperty.queueCount;
    }
    vkGetPhysicalDeviceSurfaceSupportKHR(device, i, *surface, &presentable);
    if (presentable == true) {
      queueFamilyIndices.presentFamilyIndice = i;
      queueFamilyIndices.presentFamilyQueueCount =
          queueFamilyProperty.queueCount;
    }

    if (queueFamilyIndices.isComplete()) {
      break;
    }
    i++;
  }
  return queueFamilyIndices;
}

static ComputeQueueFamilyIndices
FindComputeQueueFamilyIndices(VkPhysicalDevice device) {
  uint32_t queueFamilyPropertiesCount;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           nullptr);
  if (queueFamilyPropertiesCount <= 0) {
    std::runtime_error("Failed to get Queue Families from Physcial Device");
  }

  ComputeQueueFamilyIndices queueFamilyIndices;

  std::vector<VkQueueFamilyProperties> queueFamilyProperties;
  queueFamilyProperties.resize(queueFamilyPropertiesCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           queueFamilyProperties.data());
  uint32_t i = 0;
  for (const auto &queueFamilyProperty : queueFamilyProperties) {
    if (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      queueFamilyIndices.computeFamilyIndice = i;
      queueFamilyIndices.computeFamilyQueueCount =
          queueFamilyProperty.queueCount;
    }
    if (queueFamilyIndices.isComplete()) {
      break;
    }
    i++;
  }
  return queueFamilyIndices;
}

static TransferQueueFamilyIndices
FindTransferQueueFamilyIndices(VkPhysicalDevice device) {
  uint32_t queueFamilyPropertiesCount;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           nullptr);
  if (queueFamilyPropertiesCount <= 0) {
    std::runtime_error("Failed to get Queue Families from Physcial Device");
  }

  TransferQueueFamilyIndices queueFamilyIndices;

  std::vector<VkQueueFamilyProperties> queueFamilyProperties;
  queueFamilyProperties.resize(queueFamilyPropertiesCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           queueFamilyProperties.data());
  uint32_t i = 0;
  for (const auto &queueFamilyProperty : queueFamilyProperties) {
    if (queueFamilyProperty.queueFlags & VK_QUEUE_TRANSFER_BIT) {
      queueFamilyIndices.transferFamilyIndice = i;
      queueFamilyIndices.transferFamilyQueueCount =
          queueFamilyProperty.queueCount;
    }
    if (queueFamilyIndices.isComplete()) {
      break;
    }
    i++;
  }
  return queueFamilyIndices;
}

static ProtectedQueueFamilyIndices
FindProtectedQueueFamilyIndices(VkPhysicalDevice device) {
  uint32_t queueFamilyPropertiesCount;
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           nullptr);
  if (queueFamilyPropertiesCount <= 0) {
    std::runtime_error("Failed to get Queue Families from Physcial Device");
  }

  ProtectedQueueFamilyIndices queueFamilyIndices;

  std::vector<VkQueueFamilyProperties> queueFamilyProperties;
  queueFamilyProperties.resize(queueFamilyPropertiesCount);
  vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyPropertiesCount,
                                           queueFamilyProperties.data());
  uint32_t i = 0;
  for (const auto &queueFamilyProperty : queueFamilyProperties) {
    if (queueFamilyProperty.queueFlags & VK_QUEUE_COMPUTE_BIT) {
      queueFamilyIndices.protectedFamilyIndice = i;
      queueFamilyIndices.protectedFamilyQueueCount =
          queueFamilyProperty.queueCount;
    }
    if (queueFamilyIndices.isComplete()) {
      break;
    }
    i++;
  }
  return queueFamilyIndices;
}
} // namespace Vulkan
} // namespace BladeEngine