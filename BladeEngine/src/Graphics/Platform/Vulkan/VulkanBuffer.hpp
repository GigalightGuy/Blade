#pragma once

#include "vulkan/vulkan.hpp"

namespace BladeEngine::Graphics::Vulkan
{
	struct VulkanBuffer
	{
		VkBuffer BufferHandle;
		VkDeviceMemory MemoryHandle;
		void* MappedMemory;
	};
}
