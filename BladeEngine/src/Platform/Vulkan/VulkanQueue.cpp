#include "VulkanQueue.hpp"

#include "VulkanDevice.hpp"

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

    VulkanQueue::VulkanQueue(VulkanDevice* logicalDevice, uint32_t queueFamilyIndex, uint32_t queueIndex)
		: m_Device(logicalDevice), m_QueueFamilyIndex(queueFamilyIndex), m_QueueIndex(queueIndex)
    {
        vkGetDeviceQueue(logicalDevice->GetLogicalDevice(), queueFamilyIndex, queueIndex, &m_Queue);
    }
	
	VulkanQueue::~VulkanQueue()
	{
		
	}
    
    
}