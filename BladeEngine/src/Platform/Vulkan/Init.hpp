#include "Swapchain.hpp"
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include <vulkan/vulkan.h>

namespace BladeEngine::Vulkan 
{
    /*
    Begin by making the structure of a Vulkan supported aplication
    with a Vulkan Instance,picking the Physical Device and the logical device
    */

    static VkInstance vkInstance;
    static VkPhysicalDevice vkPhysicalDevice;
    static VkDevice vkDevice;
    static VkQueue graphicsQueue, presentQueue;

    static const std::vector<const char*> vkPhysicalDeviceExtensions = 
    {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    static void InitBladeVulkan();

    static void CreateInstance(uint32_t apiVersion, uint32_t extensionsCount,
                           const char *const *extensionNames);

    static VkBool32 CheckDeviceExtensionSupport(VkPhysicalDevice device);
    static VkBool32 DeviceIsSuitable(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface);

    static void PickPhysicalDevice();

    static void CreateLogicalDevice();
} // namespace BladeEngine::Vulkan