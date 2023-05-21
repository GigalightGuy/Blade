#include "VulkanResourceAllocator.hpp"

#include "VulkanCheck.hpp"

namespace BladeEngine::Graphics::Vulkan {

	

	VulkanResourceAllocator::VulkanResourceAllocator(VkInstance instance, VulkanDevice* device)
	{
		VmaAllocatorCreateInfo createInfo{};
		createInfo.instance = instance;
		createInfo.physicalDevice = device->physicalDevice;
		createInfo.device = device->logicalDevice;

		BLD_VK_CHECK(vmaCreateAllocator(&createInfo, &m_Allocator),
			"Failed to create VMA allocator");

		BLD_CORE_INFO("Created Vulkan Resource Allocator");

	}

	VulkanResourceAllocator::~VulkanResourceAllocator()
	{
		if (m_Allocator)
		{
			vmaDestroyAllocator(m_Allocator);

			BLD_CORE_INFO("Destroyed Vulkan Resource Allocator");
		}
	}

}