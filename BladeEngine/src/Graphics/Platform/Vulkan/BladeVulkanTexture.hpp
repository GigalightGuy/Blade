#pragma once

#include "../../Texture2D.hpp"
#include "BladeVulkanSwapchain.hpp"
#include <vulkan/vulkan.h>

namespace BladeEngine::Graphics::Vulkan {

	class VulkanTexture 
	{
	public:
		VulkanTexture(VkPhysicalDevice physicalDevice, VkDevice device,
			VkQueue graphicsQueue, VkCommandPool commandPool,
			Texture2D* texture);

		~VulkanTexture();

		void Dispose(VkDevice device);
		VkImage textureImage;
		VkDeviceMemory textureImageMemory;
		VkImageView textureImageView;
		VkSampler textureSampler;

	private:
		void CreateTextureImage(VkPhysicalDevice physicalDevice, VkDevice device,
			VkQueue graphicsQueue, VkCommandPool commandPool, Texture2D* texture);
		void CreateTextureImageView(VkDevice device, VkFormat format);
		void CreateTextureSampler(
			VkPhysicalDevice physicalDevice, VkDevice device,
			const Texture2D::SamplerConfiguration* samplerConfig);
	};

}
