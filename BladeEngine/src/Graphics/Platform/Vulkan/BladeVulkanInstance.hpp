#pragma once

#include <vulkan/vulkan.h>

#include <cstdint>
#include <iostream>
#include <vector>

namespace BladeEngine::Graphics::Vulkan
{
	class VulkanInstance 
	{
	public:
		VulkanInstance(const char* applicationName, const uint32_t applicationVersion,
			std::vector<const char*> instanceExtensions,
			std::vector<const char*> validationLayers);
		~VulkanInstance();
		void Dispose();
		VkInstance instance;

	private:
		VkDebugUtilsMessengerEXT debugMessenger;
		// Helper Functions
		void PopulateDebugMessengerCreateInfo(
			VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		// Create Functions
		void CreateVulkanInstance();
		void CreateDebugMessenger();
	};

} // namespace BladeEngine
