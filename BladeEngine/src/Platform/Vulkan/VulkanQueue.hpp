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
        VulkanQueue(VulkanDevice* logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex);
        ~VulkanQueue();

        inline const VkQueue GetQueue() const { return m_Queue; }

    private:
        VkQueue m_Queue;

        uint32_t m_QueueFamilyIndex;
        uint32_t m_QueueIndex;

        VulkanDevice* m_Device;
    };

}