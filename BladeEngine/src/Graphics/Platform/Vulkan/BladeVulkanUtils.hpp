#pragma once

#include "../../Vertex.hpp"
#include "../../../Core/Buffer.hpp"
#include "VulkanBuffer.hpp"

#include <vulkan/vulkan.h>

#include <iostream>
#include <vector>

namespace BladeEngine::Graphics::Vulkan
{

	struct MVP {
		alignas(16) glm::mat4 model;
		alignas(16) glm::mat4 view;
		alignas(16) glm::mat4 proj;
	};

	void CreateBuffer(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		VkDeviceSize size,
		VkBufferUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkBuffer& buffer,
		VkDeviceMemory& bufferMemory
	);
	void CopyBuffer(
		VkDevice device,
		VkQueue graphicsQueue,
		VkCommandPool commandPool,
		VkBuffer srcBuffer,
		VkBuffer dstBuffer,
		VkDeviceSize size
	);
	VkCommandBuffer BeginSingleTimeCommands(
		VkDevice device,
		VkCommandPool commandPool
	);
	void EndSingleTimeCommands(
		VkDevice device,
		VkQueue graphicsQueue,
		VkCommandPool commandPool,
		VkCommandBuffer commandBuffer
	);
	uint32_t FindMemoryType(
		VkPhysicalDevice physicalDevice,
		uint32_t typeFilter,
		VkMemoryPropertyFlags properties
	);
	void TransitionImageLayout(
		VkDevice device,
		VkQueue graphicsQueue,
		VkCommandPool commandPool,
		VkImage image,
		VkFormat format,
		VkImageLayout oldLayout,
		VkImageLayout newLayout
	);
	void CopyBufferToImage(
		VkDevice device,
		VkQueue graphicsQueue,
		VkCommandPool commandPool,
		VkBuffer buffer,
		VkImage image,
		uint32_t width,
		uint32_t height
	);
	void CreateImage(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		uint32_t width,
		uint32_t height,
		VkFormat format,
		VkImageTiling tiling,
		VkImageUsageFlags usage,
		VkMemoryPropertyFlags properties,
		VkImage& image,
		VkDeviceMemory& imageMemory
	);


	VulkanBuffer* CreateVertexBuffer(
		VulkanResourceAllocator& allocator,
		Buffer vertices,
		VkDevice device,
		VkQueue graphicsQueue,
		VkCommandPool commandPool);

	VulkanBuffer* CreateIndexBuffer(
		VulkanResourceAllocator& allocator,
		Buffer indices,
		VkDevice device,
		VkQueue graphicsQueue,
		VkCommandPool commandPool);

	VulkanBuffer* CreateUniformBuffer(
		VulkanResourceAllocator& allocator,
		uint64_t size);


	template<typename T>
	VkVertexInputBindingDescription GetBindingDescription(
		uint32_t shaderIndex);

	template<>
	VkVertexInputBindingDescription GetBindingDescription<VertexColor>(uint32_t shaderIndex);

	template<>
	VkVertexInputBindingDescription GetBindingDescription<VertexTexture>(uint32_t shaderIndex);

	template<>
	VkVertexInputBindingDescription GetBindingDescription<VertexColorTexture>(uint32_t shaderIndex);



	template<typename T>
	std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions(
		uint32_t shaderIndex);

	template<>
	std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexColor>(uint32_t shaderIndex);

	template<>
	std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexTexture>(uint32_t shaderIndex);

	template<>
	std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexColorTexture>(uint32_t shaderIndex);
}
