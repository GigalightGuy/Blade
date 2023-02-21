#pragma once

#include <stdint.h>
#include <optional>

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Vulkan
{
    struct QueueFamilyIndices
    {
    	std::optional<uint32_t> GraphicsFamily;
    	std::optional<uint32_t> PresentFamily;

    	bool IsComplete()
    	{
    		return GraphicsFamily.has_value() && PresentFamily.has_value();
    	}
    };

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR windowSurface);

    class VulkanDevice;

    class VulkanQueue
    {
    public:
        VulkanQueue();
        ~VulkanQueue();

    private:
        VkQueue m_Queue;

        uint32_t m_FamilyIndex;
        uint32_t m_QueueIndex;

        VulkanDevice* m_Device;
    };

}