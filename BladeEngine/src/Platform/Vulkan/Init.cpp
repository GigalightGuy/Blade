#include "Init.hpp"
#include "QueueFamily.hpp"
namespace BladeEngine {

namespace Vulkan {
  // TODO: Add default values to structures of Vulkan Instance
  static void CreateInstance(uint32_t apiVersion, uint32_t extensionsCount,
                           const char *const *extensionNames) {
  VkApplicationInfo appInfo{};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.apiVersion = apiVersion;
  appInfo.applicationVersion = 1;

  VkInstanceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  createInfo.enabledExtensionCount = extensionsCount;
  createInfo.ppEnabledExtensionNames = extensionNames;

  createInfo.enabledLayerCount = 0;
  createInfo.ppEnabledLayerNames = nullptr;

  if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
    std::runtime_error("Failed to create Vulkan Instance!");
  }
  std::cout << "Created Instance" << std::endl;
}

static VkBool32 CheckDeviceExtensionSupport(VkPhysicalDevice device) {
  uint32_t extensionCount;
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       nullptr);

  std::vector<VkExtensionProperties> availableExtensions(extensionCount);
  vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount,
                                       availableExtensions.data());

  std::set<std::string> requiredExtensions(vkPhysicalDeviceExtensions.begin(),
                                           vkPhysicalDeviceExtensions.end());

  for (const auto &extension : availableExtensions) {
    requiredExtensions.erase(extension.extensionName);
  }

  return requiredExtensions.empty();
}

// TODO: Add swapchain support for the suitablility of device
static VkBool32 DeviceIsSuitable(VkPhysicalDevice physicalDevice,
                                 VkSurfaceKHR windowSurface) {
  VkPhysicalDeviceProperties properties;
  vkGetPhysicalDeviceProperties(physicalDevice, &properties);

  VkPhysicalDeviceFeatures features;
  vkGetPhysicalDeviceFeatures(physicalDevice, &features);

  GraphicsPresentQueueFramilyIndices GPQueueFamilyIndices =
      FindGraphicsPresentQueueFamilyIndices(physicalDevice, &windowSurface);

  VkBool32 swapChainAdequate = false;
  if (CheckDeviceExtensionSupport(physicalDevice) == VK_TRUE) {
    SwapChainSupportDetails swapChainSupportDetails =
        QuerySwapChainSupport(physicalDevice, windowSurface);
    swapChainAdequate = !swapChainSupportDetails.formats.empty() &&
                        !swapChainSupportDetails.presentModes.empty();
  }

  return features.samplerAnisotropy && GPQueueFamilyIndices.isComplete() &&
         swapChainAdequate;
}

static void PickPhysicalDevice(VkSurfaceKHR windowSurface) {
  uint32_t physicalDeviceCount;
  vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);

  if (physicalDeviceCount <= 0) {
    std::runtime_error("No Vulkan Supported Physical Device Found");
  }
  std::vector<VkPhysicalDevice> physicalDevices;
  physicalDevices.resize(physicalDeviceCount);
  vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount,
                             physicalDevices.data());

  for (const auto &physicalDevice : physicalDevices) {
    if (DeviceIsSuitable(physicalDevice, windowSurface) == VK_TRUE) {
      vkPhysicalDevice = physicalDevice;
      std::cout << "Selected Vulkan Supported Physical Device!" << std::endl;
      return;
    }
  }
  std::runtime_error("Failed to select suitable Physcial Device");
}

static void CreateLogicalDevice(VkSurfaceKHR windowSurface) {
  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

  // Setup Graphics and Present Queues
  GraphicsPresentQueueFramilyIndices GPQueueFamilyIndices =
      FindGraphicsPresentQueueFamilyIndices(vkPhysicalDevice, &windowSurface);
  uint32_t familyCount = 2;
  float queuePriority = 1.0f;
  for (uint32_t i = 0; i < familyCount; i++) {
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    if (i == 0) {
      queueInfo.queueCount = GPQueueFamilyIndices.graphicsFamilyQueueCount;
      queueInfo.queueFamilyIndex =
          GPQueueFamilyIndices.graphicsFamilyIndice.value();
    } else {
      queueInfo.queueCount = GPQueueFamilyIndices.presentFamilyQueueCount;
      queueInfo.queueFamilyIndex =
          GPQueueFamilyIndices.presentFamilyIndice.value();
    }
    queueInfo.pQueuePriorities = &queuePriority;
    queueCreateInfos.push_back(queueInfo);
  }

  // Setup Device Features of the Queues
  VkPhysicalDeviceFeatures deviceFeatures{};
  deviceFeatures.samplerAnisotropy = VK_TRUE;

  VkDeviceCreateInfo createInfo{};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount =
      static_cast<uint32_t>(queueCreateInfos.size());
  createInfo.pQueueCreateInfos = queueCreateInfos.data();
  createInfo.enabledExtensionCount =
      static_cast<uint32_t>(vkPhysicalDeviceExtensions.size());
  createInfo.ppEnabledExtensionNames = vkPhysicalDeviceExtensions.data();
  createInfo.pEnabledFeatures = &deviceFeatures;
  if (vkCreateDevice(vkPhysicalDevice, &createInfo, nullptr, &vkDevice) !=
      VK_SUCCESS) {
    std::runtime_error("Failed to create Logical Device");
  }

  std::cout << "Created Vulkan Logical Device!" << std::endl;
  vkGetDeviceQueue(vkDevice, GPQueueFamilyIndices.graphicsFamilyIndice.value(),
                   0, &graphicsQueue);
  vkGetDeviceQueue(vkDevice, GPQueueFamilyIndices.presentFamilyIndice.value(),
                   0, &presentQueue);
}

} // namespace Vulkan
} // namespace BladeEngine