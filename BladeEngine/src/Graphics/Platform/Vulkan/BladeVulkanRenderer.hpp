#pragma once

#define FRAMES_IN_FLIGHT 3

#include "BladeVulkanInstance.hpp"
#include "BladeVulkanDevice.hpp"
#include "VulkanResourceAllocator.hpp"
#include "BladeVulkanSwapchain.hpp"
#include "BladeVulkanShader.hpp"
#include "BladeVulkanGraphicsPipeline.hpp"
#include "BladeVulkanMesh.hpp"
#include "BladeVulkanTexture.hpp"
#include "VulkanRenderPass.hpp"

//#define BLADE_VULKAN_API true
#include "../../../Core/Window.hpp"
#include "../../../Core/Camera.hpp"
#include "../../Color.hpp"
#include "../../Shader.hpp"
#include "../../Mesh.hpp"
#include "../../Font.hpp"

#include <map>

namespace BladeEngine::Graphics::Vulkan {

	class VulkanRenderer 
	{
	public:
		VulkanRenderer(BladeEngine::Camera* camera, Window* window);
		~VulkanRenderer();

		void CreateDebugResources();

		void Dispose();

		void Clear(BladeEngine::Graphics::Color color);

		// Creates Graphics pipeline for default shaders to be used during draw calls
		void BeginDrawing();
		// Create and store Vulkan Texture, store model data, store quad mesh, create and allocate descriptor pools and sets
		void DrawSprite(BladeEngine::Graphics::Texture2D* texture, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale, const glm::vec4& uvTransform);
		// Create Descriptor Pool, Descriptor sets, update descriptor sets, uniform buffers, drawindexed
		void EndDrawing();

		void DrawString(const std::string& string, Font* font, const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale);

		void WaitDeviceIdle();

		void RecreateSwapchain(uint32_t width, uint32_t height);

		VulkanTexture* UploadTextureToGPU(Texture2D* texture);
		void ReleaseGPUTexture(VulkanTexture* gpuTexture);

		VulkanMesh* UploadMeshToGPU(Buffer vertices, Buffer indices);
		void ReleaseGPUMesh(VulkanMesh* gpuMesh);


	private:
		void Init(Window* window);
		void CreateClearRenderPass();
		void CreateClearFramebuffer();

		void CreateCommandPool();
		void CreateCommandBuffers();
		void CreateSyncObjects();

		void RecordCommandBuffer();

		void DrawFrame();

		struct ModelData
		{
			glm::vec3 position;
			glm::vec3 rotation;
			glm::vec3 scale;
		};

		uint32_t currentFrame = 0;
		uint32_t imageIndex = -1;

		//Init
		Color backgroundColor;
		Camera* camera;
		VulkanInstance* vkInstance;
		VulkanDevice* vkDevice;
		VulkanResourceAllocator* m_ResourceAllocator;

		VkSurfaceKHR vkSurface;
		VulkanSwapchain* vkSwapchain;

		//Clear dependencies
		VulkanSwapchain* vkClearSwapchain;
		VkRenderPass vkClearRenderPass;
		std::vector<VkFramebuffer> vkClearFramebuffers;

		Shader* defaultSpriteVertexShader;
		Shader* defaultSpriteFragmentShader;

		Shader* defaultTextVertexShader;
		Shader* defaultTextFragmentShader;

		//Render Loop
		std::vector<VulkanRenderPass*> m_RenderPasses;
		std::unordered_map<VulkanRenderPass*, std::vector<VulkanGraphicsPipeline*>> m_GraphicsPipelinesMap;

		std::vector<VulkanTexture*> vkTextures;
		std::vector<VulkanMesh*> vkMeshes;
		std::vector<ModelData> vkMeshesModelData;
		std::vector<PushConstantData> m_PushConstantsData;

		uint32_t m_TextCount = 0;
		std::vector<uint32_t> m_TextQuadCounts;
		std::vector<VulkanBuffer*> m_TextIndexBuffers[FRAMES_IN_FLIGHT];
		std::vector<VulkanBuffer*> m_TextVertexBuffers[FRAMES_IN_FLIGHT];

		Texture2D* m_UVDebugTexture = nullptr;

		//Draw
		VkCommandPool vkCommandPool;
		std::vector<VkCommandBuffer> commandBuffers;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		uint32_t m_UniformBufferCount = 0;
		std::vector<VulkanBuffer*> m_UniformBuffers[FRAMES_IN_FLIGHT];
	};

}
