#pragma once
#include <vulkan/vulkan.hpp>
#include <stdint.h>

namespace BladeEngine::Vulkan
{
    class VulkanDevice;

    class VulkanQueue
    {
    public:
        VulkanQueue(/* args */);
        ~VulkanQueue();
    private:
        VkQueue Queue;
        uint32_t FamilyIndex;
        uint32_t QueueIndex;
        VulkanDevice* Device;
    };

}