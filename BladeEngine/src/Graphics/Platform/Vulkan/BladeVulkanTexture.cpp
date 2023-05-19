#include "BladeVulkanTexture.hpp"

#include <string.h>
#include "BladeVulkanUtils.hpp"

namespace BladeEngine::Graphics::Vulkan {

	VkFormat BladeTextureFormatToVulkanFormat(TextureFormat format)
	{
		switch (format)
		{
		case TextureFormat::R8:
			return VK_FORMAT_R8_SRGB;
		case TextureFormat::RG8:
			return VK_FORMAT_R8G8_SRGB;
		case TextureFormat::RGB8:
			return VK_FORMAT_R8G8B8_SRGB;
		case TextureFormat::RGBA8:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case TextureFormat::RGBA32F:
			return VK_FORMAT_R8_SRGB;
		}

		// TODO: Change types and assert on error

		return VK_FORMAT_R8_SRGB;
		
	}

	VulkanTexture::VulkanTexture(
		VkPhysicalDevice physicalDevice, VkDevice device,
		VkQueue graphicsQueue, VkCommandPool commandPool,
		VulkanSwapchain* swapchain, Texture2D* texture)
	{
		CreateTextureImage(physicalDevice, device, graphicsQueue, commandPool, texture);
		CreateTextureImageView(device, swapchain);
		CreateTextureSampler(physicalDevice, device);
	}

	VulkanTexture::~VulkanTexture() {}

	void VulkanTexture::Dispose(VkDevice device)
	{
		vkDestroySampler(device, textureSampler, nullptr);
		vkDestroyImageView(device, textureImageView, nullptr);
		vkDestroyImage(device, textureImage, nullptr);
		vkFreeMemory(device, textureImageMemory, nullptr);
	}


	void VulkanTexture::CreateTextureImage(
		VkPhysicalDevice physicalDevice,
		VkDevice device, VkQueue graphicsQueue,
		VkCommandPool commandPool,
		Texture2D* texture)
	{
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		uint32_t textureSize = texture->GetSize();

		CreateBuffer(physicalDevice, device, textureSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
			VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, textureSize, 0, &data);
		memcpy(data, texture->GetData(), static_cast<size_t>(textureSize));
		vkUnmapMemory(device, stagingBufferMemory);

		CreateImage(
			physicalDevice, device, texture->GetWidth(), texture->GetHeight(),
			VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

		TransitionImageLayout(
			device, graphicsQueue, commandPool, textureImage, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		CopyBufferToImage(device, graphicsQueue, commandPool,
			stagingBuffer, textureImage,
			static_cast<uint32_t>(texture->GetWidth()),
			static_cast<uint32_t>(texture->GetHeight()));

		TransitionImageLayout(device, graphicsQueue, commandPool,
			textureImage, VK_FORMAT_R8G8B8A8_SRGB,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}
	void VulkanTexture::CreateTextureImageView(
		VkDevice device,
		VulkanSwapchain* swapchain)
	{
		textureImageView = swapchain->CreateImageView(
			device, textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void VulkanTexture::CreateTextureSampler(
		VkPhysicalDevice physicalDevice,
		VkDevice device)
	{

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
	}

}
