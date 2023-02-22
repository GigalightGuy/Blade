#include "VulkanDevice.hpp"

#include <stdint.h>
#include <map>
#include <set>
#include <string>

#include "../../Core/Log.hpp"
#include "VulkanQueue.hpp"
#include "VulkanSwapchain.hpp"
#include "VulkanValidationLayers.hpp"

namespace BladeEngine::Vulkan
{
    VulkanDevice::VulkanDevice(VkInstance instance)
        : m_Instance(instance)
    {
        PickGPU();
		CreateLogicalDevice();
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

    bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices = FindQueueFamilies(device, m_Surface);

		bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapchainAdequate = false;
		if (extensionsSupported)
		{
			SwapchainSupportDetails swapchainSupport = QuerySwapchainSupport(device, m_Surface);
			swapchainAdequate = 
				!swapchainSupport.formats.empty() && !swapchainSupport.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return indices.IsComplete() && extensionsSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy;
	}

	bool VulkanDevice::CheckDeviceExtensionSupport(VkPhysicalDevice device)
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

	void VulkanDevice::CreateLogicalDevice()
	{
		QueueFamilyIndices indices = FindQueueFamilies(m_GPU, m_Surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies =
		{
			indices.GraphicsFamily.value(),
			indices.PresentFamily.value()
		};

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE;

		VkDeviceCreateInfo deviceCreateInfo{};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

		deviceCreateInfo.ppEnabledExtensionNames = k_DeviceExtensions.data();
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(k_DeviceExtensions.size());

#if BLADE_DEBUG
        auto validationLayers = Vulkan::Debug::GetValidationLayers();
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
#else
		deviceCreateInfo.enabledLayerCount = 0;
#endif

		if (vkCreateDevice(m_GPU, &deviceCreateInfo, nullptr, &m_Device) != VK_SUCCESS)
		{
			BLD_CORE_ERROR("Failed to create Vulkan logical device!");
		}

		m_GraphicsQueue = new VulkanQueue(this, indices.GraphicsFamily.value(), 0);
		m_PresentQueue = new VulkanQueue(this, indices.PresentFamily.value(), 0);
	}
}