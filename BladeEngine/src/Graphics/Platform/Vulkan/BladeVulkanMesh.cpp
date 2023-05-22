#include "BladeVulkanMesh.hpp"

#include "BladeVulkanUtils.hpp"

#include <string.h>

namespace BladeEngine::Graphics::Vulkan
{
	VulkanMesh* LoadMesh(
		VulkanResourceAllocator& allocator,
		VkDevice device, 
		VkQueue graphicsQueue, 
		VkCommandPool commandPool,
		Buffer vertices,
		Buffer indices)
	{
		VulkanMesh* mesh = new VulkanMesh();

		mesh->VertexBuffer = CreateVertexBuffer(allocator, vertices, device, graphicsQueue, commandPool);
		mesh->IndexBuffer = CreateIndexBuffer(allocator, indices, device, graphicsQueue, commandPool);
		mesh->IndicesCount = indices.Size / sizeof(uint16_t);
		
		return mesh;
	}


	void VulkanMesh::Draw(
		VkCommandBuffer commandBuffer, VkPipelineLayout& pipelineLayout, 
		VkDescriptorSet& descriptorSet)
	{
		VkBuffer vertexBuffers[] = { VertexBuffer->GetBuffer() };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffer, IndexBuffer->GetBuffer(), 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
			pipelineLayout, 0, 1, &descriptorSet,
			0, nullptr);

		vkCmdDrawIndexed(commandBuffer, IndicesCount, 1, 0, 0, 0);
	}

	void VulkanMesh::Dispose(VkDevice device)
	{
		delete VertexBuffer;
		delete IndexBuffer;
	}
}





