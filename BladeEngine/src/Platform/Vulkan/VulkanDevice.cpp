#include "VulkanDevice.hpp"

#include <map>
#include <set>
#include <string>

#include "../../Core/Log.hpp"
#include "VulkanQueue.hpp"

namespace BladeEngine::Vulkan
{
    VulkanDevice::VulkanDevice(VkInstance instance)
        : m_Instance(instance)
    {
        
    }
    
    VulkanDevice::~VulkanDevice()
    {
        
    }

    void VulkanDevice::PickGPU()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
            BLD_CORE_ERROR("Failed to find any GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		std::multimap<int, VkPhysicalDevice> candidates;

		for (const auto& device : devices)
		{
			if (IsDeviceSuitable(device))
			{
				m_GPU = device;
				// TODO: check for multisampling support -m_MSAASamples = GetMaxUsableSampleCount();
				break;
			}
		}

		if (m_GPU == VK_NULL_HANDLE)
		{
			BLD_CORE_ERROR("Failed to find a suitable GPU!");
		}
	}

    bool IsDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = FindQueueFamilies(device, m_Surface);

		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapchainAdequate = false;
		if (extensionsSupported)
		{
			SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device);
			swapchainAdequate = 
				!swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.IsComplete() && extensionsSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy;
	}

	bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
	{
		uint32_t extensionCount = 0;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(k_DeviceExtensions.begin(), k_DeviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}
}