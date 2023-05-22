#include "BladeVulkanUtils.hpp"

#include <string.h>

namespace BladeEngine::Graphics::Vulkan {

	void CreateBuffer(
		VkPhysicalDevice physicalDevice, VkDevice device, 
		VkDeviceSize size, VkBufferUsageFlags usage, 
		VkMemoryPropertyFlags properties, VkBuffer& buffer,
		VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(
			physicalDevice, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		//TODO: Study Function
		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void CopyBuffer(
		VkDevice device, VkQueue graphicsQueue, 
		VkCommandPool commandPool, VkBuffer srcBuffer, 
		VkBuffer dstBuffer, VkDeviceSize size)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands(
			device, commandPool);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
		EndSingleTimeCommands(device, graphicsQueue, commandPool,
			commandBuffer);
	}

	VkCommandBuffer BeginSingleTimeCommands(VkDevice device, VkCommandPool commandPool)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void EndSingleTimeCommands(
		VkDevice device, VkQueue graphicsQueue, 
		VkCommandPool commandPool, VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
	}

	uint32_t FindMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags &
				properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void TransitionImageLayout(
		VkDevice device, VkQueue graphicsQueue, 
		VkCommandPool commandPool, VkImage image, 
		VkFormat format, VkImageLayout oldLayout, 
		VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands(device, commandPool);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
			newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
			nullptr, 0, nullptr, 1, &barrier);

		EndSingleTimeCommands(device, graphicsQueue, commandPool, commandBuffer);
	}

	void CopyBufferToImage(
		VkDevice device, VkQueue graphicsQueue, 
		VkCommandPool commandPool, VkBuffer buffer, 
		VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands(device, commandPool);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(commandBuffer, buffer, image,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		EndSingleTimeCommands(device, graphicsQueue, commandPool, commandBuffer);
	}

	void CreateImage(
		VkPhysicalDevice physicalDevice, VkDevice device, 
		uint32_t width, uint32_t height, 
		VkFormat format, VkImageTiling tiling, 
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, 
		VkImage& image, VkDeviceMemory& imageMemory)
	{
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType(
			physicalDevice, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}


	VulkanBuffer* CreateVertexBuffer(
		VulkanResourceAllocator& allocator,
		Buffer vertices,
		VkDevice device,
		VkQueue graphicsQueue,
		VkCommandPool commandPool)
	{
		VulkanBuffer* vertexBuffer;

		BufferDescription bufferDescription;
		bufferDescription.Usage = BufferUsage::TransferSource;
		bufferDescription.AllocationUsage = BufferAllocationUsage::HostWrite;
		bufferDescription.KeepMapped = true;
		bufferDescription.Data = vertices.Data;
		bufferDescription.Size = vertices.Size;
		VulkanBuffer vertexStagingBuffer = VulkanBuffer(bufferDescription, allocator);

		bufferDescription.Usage = BufferUsage::Vertex | BufferUsage::TransferDestination;
		bufferDescription.AllocationUsage = BufferAllocationUsage::DeviceLocal;
		bufferDescription.KeepMapped = false;
		bufferDescription.Data = nullptr;
		bufferDescription.Size = vertices.Size;
		vertexBuffer = new VulkanBuffer(bufferDescription, allocator);

		CopyBuffer(device, graphicsQueue, commandPool,
			vertexStagingBuffer.GetBuffer(), vertexBuffer->GetBuffer(),
			vertices.Size);

		return vertexBuffer;
	}

	VulkanBuffer* CreateIndexBuffer(
		VulkanResourceAllocator& allocator,
		Buffer indices,
		VkDevice device,
		VkQueue graphicsQueue,
		VkCommandPool commandPool)
	{
		VulkanBuffer* indexBuffer;

		BufferDescription bufferDescription;
		bufferDescription.Usage = BufferUsage::TransferSource;
		bufferDescription.AllocationUsage = BufferAllocationUsage::HostWrite;
		bufferDescription.KeepMapped = true;
		bufferDescription.Data = indices.Data;
		bufferDescription.Size = indices.Size;
		VulkanBuffer indexStagingBuffer = VulkanBuffer(bufferDescription, allocator);

		bufferDescription.Usage = BufferUsage::Index | BufferUsage::TransferDestination;
		bufferDescription.AllocationUsage = BufferAllocationUsage::DeviceLocal;
		bufferDescription.KeepMapped = false;
		bufferDescription.Data = nullptr;
		bufferDescription.Size = indices.Size;
		indexBuffer = new VulkanBuffer(bufferDescription, allocator);

		CopyBuffer(device, graphicsQueue, commandPool,
			indexStagingBuffer.GetBuffer(), indexBuffer->GetBuffer(),
			indices.Size);

		return indexBuffer;
	}

	VulkanBuffer* CreateUniformBuffer(
		VulkanResourceAllocator& allocator,
		uint64_t size)
	{
		BufferDescription bufferDescription;
		bufferDescription.KeepMapped = true;
		bufferDescription.AllocationUsage = BufferAllocationUsage::HostWrite;
		bufferDescription.Usage = BufferUsage::Uniform;
		bufferDescription.Size = size;
		bufferDescription.Data = nullptr;

		return new VulkanBuffer(bufferDescription, allocator);
	}

	template<>
	VkVertexInputBindingDescription GetBindingDescription<VertexColor>(uint32_t shaderIndex)
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = shaderIndex;
		bindingDescription.stride = sizeof(VertexColor);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	template<>
	VkVertexInputBindingDescription GetBindingDescription<VertexTexture>(uint32_t shaderIndex)
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = shaderIndex;
		bindingDescription.stride = sizeof(VertexTexture);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	template<>
	VkVertexInputBindingDescription GetBindingDescription<VertexColorTexture>(uint32_t shaderIndex)
	{
		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = shaderIndex;
		bindingDescription.stride = sizeof(VertexColorTexture);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}



	template<>
	std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexColor>(uint32_t shaderIndex)
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.resize(2);

		attributeDescriptions[0].binding = shaderIndex;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexColor, position);

		attributeDescriptions[1].binding = shaderIndex;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexColor, color);
		return attributeDescriptions;
	}

	template<>
	std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexTexture>(uint32_t shaderIndex)
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.resize(2);

		attributeDescriptions[0].binding = shaderIndex;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexTexture, position);

		attributeDescriptions[1].binding = shaderIndex;
		attributeDescriptions[1].location = 2;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexTexture, textureCoordinate);
		return attributeDescriptions;
	}

	template<>
	std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions<VertexColorTexture>(uint32_t shaderIndex)
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.resize(3);

		attributeDescriptions[0].binding = shaderIndex;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(VertexColorTexture, position);

		attributeDescriptions[1].binding = shaderIndex;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(VertexColorTexture, color);

		attributeDescriptions[2].binding = shaderIndex;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[2].offset =
			offsetof(VertexColorTexture, textureCoordinate);

		return attributeDescriptions;
	}
}










