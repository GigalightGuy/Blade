#include "BladeVulkanMesh.hpp"

#include "BladeVulkanUtils.hpp"

#include <string.h>

namespace BladeEngine::Graphics::Vulkan
{
	VulkanMesh* LoadMesh(
		VulkanResourceAllocator& allocator,
		VkPhysicalDevice physicalDevice, VkDevice device, 
		VkQueue graphicsQueue, VkCommandPool commandPool,
		Buffer vertices,
		Buffer indices)
	{
		VulkanMesh* mesh = new VulkanMesh();

		BufferDescription bufferDescription;
		bufferDescription.AllocationUsage = BufferAllocationUsage::HostWrite;
		bufferDescription.KeepMapped = true;
		bufferDescription.Usage = BufferUsage::TransferSource;

		bufferDescription.Data = vertices.Data;
		bufferDescription.Size = vertices.Size;
		VulkanBuffer vertexStagingBuffer = VulkanBuffer(bufferDescription, allocator);

		bufferDescription.Data = indices.Data;
		bufferDescription.Size = indices.Size;
		VulkanBuffer indexStagingBuffer = VulkanBuffer(bufferDescription, allocator);


		bufferDescription.AllocationUsage = BufferAllocationUsage::DeviceLocal;
		bufferDescription.KeepMapped = false;
		
		bufferDescription.Usage = BufferUsage::Vertex | BufferUsage::TransferDestination;
		bufferDescription.Data = nullptr;
		bufferDescription.Size = vertices.Size;
		mesh->VertexBuffer = new VulkanBuffer(bufferDescription, allocator);

		CopyBuffer(device, graphicsQueue, commandPool, 
			vertexStagingBuffer.GetBuffer(), mesh->VertexBuffer->GetBuffer(),
			vertices.Size);
		

		bufferDescription.Usage = BufferUsage::Index | BufferUsage::TransferDestination;
		bufferDescription.Data = nullptr;
		bufferDescription.Size = indices.Size;
		mesh->IndexBuffer = new VulkanBuffer(bufferDescription, allocator);

		CopyBuffer(device, graphicsQueue, commandPool,
			indexStagingBuffer.GetBuffer(), mesh->IndexBuffer->GetBuffer(),
			vertices.Size);

		//CreateVertexBuffer(vertices, physicalDevice, device, graphicsQueue, commandPool, mesh->VertexBuffer);
		//CreateIndexBuffer(indices, physicalDevice, device, graphicsQueue, commandPool, mesh->IndexBuffer);

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





