#include "BladeVulkanTexture.hpp"

#include "../../../Core/Base.hpp"

#include "VulkanCheck.hpp"
#include "BladeVulkanUtils.hpp"

namespace BladeEngine::Graphics::Vulkan {

	VkFormat GetVulkanFormat(TextureFormat format)
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

		BLD_CORE_ASSERT(false, "Unknown Texture Format");

		return VK_FORMAT_R8_SRGB;
	}

	VkFilter GetVulkanFilter(SamplerFilter filter)
	{
		switch (filter)
		{
		case SamplerFilter::Linear:
			return VK_FILTER_LINEAR;
		case SamplerFilter::Nearest:
			return VK_FILTER_NEAREST;
		}

		BLD_CORE_ASSERT(false, "Unknown Texture Format");

		return VK_FILTER_LINEAR;
	}

	VkSamplerAddressMode GetVulkanSamplerAddressMode(SamplerAddressMode addressMode)
	{
		switch (addressMode)
		{
		case SamplerAddressMode::ClampToBorder:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case SamplerAddressMode::ClampToEdges:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case SamplerAddressMode::Repeat:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case SamplerAddressMode::MirroredRepeat:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		}

		BLD_CORE_ASSERT(false, "Unknown Sampler Address Mode");

		return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
	}

	VulkanTexture::VulkanTexture(
		VkPhysicalDevice physicalDevice, VkDevice device,
		VkQueue graphicsQueue, VkCommandPool commandPool,
		Texture2D* texture)
	{
		CreateTextureImage(physicalDevice, device, graphicsQueue, commandPool, texture);
		CreateTextureImageView(device, GetVulkanFormat(texture->GetFormat()));
		CreateTextureSampler(physicalDevice, device, texture->GetSamplerConfiguration());
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
		VkFormat textureFormat = GetVulkanFormat(texture->GetFormat());

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
			textureFormat, VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImage, textureImageMemory);

		TransitionImageLayout(
			device, graphicsQueue, commandPool, textureImage, textureFormat,
			VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

		CopyBufferToImage(device, graphicsQueue, commandPool,
			stagingBuffer, textureImage,
			static_cast<uint32_t>(texture->GetWidth()),
			static_cast<uint32_t>(texture->GetHeight()));

		TransitionImageLayout(device, graphicsQueue, commandPool,
			textureImage, textureFormat,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	VkImageView CreateImageView(
		VkDevice device, VkImage image,
		VkFormat format, VkImageAspectFlags aspectFlags) 
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		BLD_VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &imageView),
			"Failed to create texture image view");

		return imageView;
	}

	void VulkanTexture::CreateTextureImageView(VkDevice device, VkFormat format)
	{
		textureImageView = CreateImageView(
			device, textureImage, format, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void VulkanTexture::CreateTextureSampler(
		VkPhysicalDevice physicalDevice,
		VkDevice device,
		const Texture2D::SamplerConfiguration* samplerConfig)
	{
		VkFilter filter = GetVulkanFilter(samplerConfig->Filter);
		VkSamplerAddressMode addressMode = GetVulkanSamplerAddressMode(samplerConfig->AdressMode);

		VkPhysicalDeviceProperties properties{};
		vkGetPhysicalDeviceProperties(physicalDevice, &properties);

		VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = filter;
		samplerInfo.minFilter = filter;
		samplerInfo.addressModeU = addressMode;
		samplerInfo.addressModeV = addressMode;
		samplerInfo.addressModeW = addressMode;
		samplerInfo.anisotropyEnable = VK_TRUE;
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

		BLD_VK_CHECK(vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler),
			"Failed to create texture sampler!");
	}

}
