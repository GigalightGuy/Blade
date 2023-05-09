#pragma once

#include "BladeVulkanUtils.hpp"
#include "VulkanBuffer.hpp"

#include "../../Vertex.hpp"
#include "../../../Core/Buffer.hpp"

#include <vulkan/vulkan.h>

#include <vector>

namespace BladeEngine::Graphics::Vulkan
{
	struct VulkanMesh
	{
		VulkanBuffer VertexBuffer;

		VulkanBuffer IndexBuffer;

		uint32_t IndicesCount;

		void Draw(
			VkCommandBuffer commandBuffer, VkPipelineLayout& pipelineLayout, 
			VkDescriptorSet& descriptorSet);
		
		void Dispose(VkDevice device);
	};

	VulkanMesh* LoadMesh(
		VkPhysicalDevice physicalDevice, VkDevice device, 
		VkQueue graphicsQueue, VkCommandPool commandPool, 
		Buffer vertices, 
		Buffer indices);

}
