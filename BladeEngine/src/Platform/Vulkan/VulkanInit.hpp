#pragma once
#include "vulkan/vulkan.hpp"
namespace BladeEngine::Vulkan
{
    VkSurfaceKHR windowSurface;
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    const std::vector<const char *> deviceExtensions;
    VkDevice device;

    //TODO: Create Vulkan Instance
    //TODO: Pick best physical device with Vulkan Support and best to be dedicated
    

    //TODO:: Create class VulkanQueue for all the available vulkan queues
        // -> Look at Unreal Queue implementation
    //TODO::Create class VulkanDevice with connection to the VKphysicaldeviceselected with the queues required for rendering and presenting


}