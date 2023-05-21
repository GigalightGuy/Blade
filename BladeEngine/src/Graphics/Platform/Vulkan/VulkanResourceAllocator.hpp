#pragma once

#include "BladeVulkanDevice.hpp"

#include "vk_mem_alloc.h"

namespace BladeEngine::Graphics::Vulkan {

	class VulkanResourceAllocator
	{
	public:
		VulkanResourceAllocator(VkInstance instance, VulkanDevice* device);
		~VulkanResourceAllocator();

		VulkanResourceAllocator(const VulkanResourceAllocator&) = delete;
		VulkanResourceAllocator& operator=(const VulkanResourceAllocator&) = delete;
		VulkanResourceAllocator(VulkanResourceAllocator&&) = delete;
		VulkanResourceAllocator& operator=(VulkanResourceAllocator&&) = delete;

		operator VmaAllocator() { return m_Allocator; }

	private:
		VmaAllocator m_Allocator = VK_NULL_HANDLE;
	};

}