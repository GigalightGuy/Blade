#include "BladeVulkanDevice.hpp"

#include "BladeVulkanQueueFamily.hpp"
#include "VulkanCheck.hpp"

#include <vulkan/vulkan_core.h>

#include <cstdint>
#include <ostream>
#include <set>
#include <vector>

namespace BladeEngine::Graphics::Vulkan {

	VulkanDevice::VulkanDevice(
		VkInstance instance, VkSurfaceKHR surface,
		std::vector<const char*> extensions) 
	{
		PickPhysicalDevice(instance, surface, extensions);
		CreateLogicalDevice(surface, extensions);
	}

	VulkanDevice::~VulkanDevice() { }

	void VulkanDevice::Dispose()
	{
		vkDestroyDevice(logicalDevice, nullptr);
	}


	SwapChainSupportDetails QuerySwapChainSupport(
		VkPhysicalDevice device,
		VkSurfaceKHR surface) 
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface,
			&details.capabilities);
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
				details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
			nullptr);
		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(
				device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	bool VulkanDevice::CheckDeviceExtensionSupport(
		VkPhysicalDevice physicalDevice, 
		std::vector<const char*> extensions) 
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
			nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount,
			availableExtensions.data());

		std::set<std::string> requiredExtensions(extensions.begin(),
			extensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	bool VulkanDevice::IsDeviceSuitable(
		VkPhysicalDevice physicalDevice,
		VkSurfaceKHR surface,
		std::vector<const char*> extensions) 
	{

		GraphicsFamily graphicsFamily = GetGraphicsFamily(physicalDevice, surface);

		bool extensionsSupported =
			CheckDeviceExtensionSupport(physicalDevice, extensions);

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

		bool swapChainAdequate = false;
		if (extensionsSupported) {
			SwapChainSupportDetails swapChainSupport =
				QuerySwapChainSupport(physicalDevice, surface);
			swapChainAdequate = !swapChainSupport.formats.empty() &&
				!swapChainSupport.presentModes.empty();
		}

		return graphicsFamily.IsComplete() && extensionsSupported &&
			swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	void VulkanDevice::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface,
		std::vector<const char*> extensions) {

		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		BLD_CORE_ASSERT(deviceCount != 0, "Failed to find GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (IsDeviceSuitable(device, surface, extensions)) {
				physicalDevice = device;
				break;
			}
		}

		BLD_CORE_ASSERT(physicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU!");
	}

	void VulkanDevice::CreateLogicalDevice(VkSurfaceKHR surface,
		std::vector<const char*> extensions) {
		GraphicsFamily indices = GetGraphicsFamily(physicalDevice, surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFlagIndex.value(),
												  indices.presentFlagIndex.value() };

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

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount =
			static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		// DEPRECATED BY VULKAN 1.1
#if defined(BLD_VK_VALIDATION_LAYERS)
		createInfo.enabledLayerCount =
			static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
#else
		createInfo.enabledLayerCount = 0;
#endif

		/*if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}*/

		BLD_VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice), 
			"Failed to create Logical Device!");

		vkGetDeviceQueue(logicalDevice, indices.graphicsFlagIndex.value(), 0,
			&graphicsQueue);
		vkGetDeviceQueue(logicalDevice, indices.presentFlagIndex.value(), 0,
			&presentQueue);
	}

}

