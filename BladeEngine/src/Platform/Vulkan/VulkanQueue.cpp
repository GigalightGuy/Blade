#include "VulkanQueue.hpp"

namespace BladeEngine::Vulkan
{
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR windowSurface)
    {
        QueueFamilyIndices indices;

		uint32_t queueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.GraphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, windowSurface, &presentSupport);
			if (presentSupport)
			{
				indices.PresentFamily = i;
			}

			if (indices.IsComplete())
				break;

			i++;
		}

		return indices;
    }

    VulkanQueue::VulkanQueue()
    {
        
    }
    
    
}