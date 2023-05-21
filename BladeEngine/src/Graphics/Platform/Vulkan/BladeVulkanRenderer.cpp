#include "BladeVulkanRenderer.hpp"

#include "BladeVulkanQueueFamily.hpp"
#include "BladeVulkanUtils.hpp"
#include "VulkanCheck.hpp"

#include "../../Shader.hpp"
#include "../../MSDFData.hpp"

#include <string.h>
#include <utility>


namespace BladeEngine::Graphics::Vulkan
{
	VulkanRenderer::VulkanRenderer(BladeEngine::Camera* camera, Window* window)
	{
		this->camera = camera;
		Init(window);
	}

	VulkanRenderer::~VulkanRenderer()
	{
		delete m_ResourceAllocator;
	}

	void VulkanRenderer::Init(Window* window)
	{
		defaultSpriteVertexShader = new Shader("assets/shaders/default.vert", ShaderType::VERTEX);
		defaultSpriteFragmentShader = new Shader("assets/shaders/default.frag", ShaderType::FRAGMENT);

		std::vector<const char*> extensions = window->GetRequiredExtensions();
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		vkInstance = new VulkanInstance("Blade GDK Program", VK_MAKE_VERSION(1, 0, 0), extensions, { "VK_LAYER_KHRONOS_validation" });
		vkSurface = window->CreateWindowSurface(vkInstance->instance);
		vkDevice = new VulkanDevice(vkInstance->instance, vkSurface,
			{ VK_KHR_SWAPCHAIN_EXTENSION_NAME });

		m_ResourceAllocator = new VulkanResourceAllocator(vkInstance->instance, vkDevice);

		vkSwapchain = new VulkanSwapchain(window, vkDevice->physicalDevice,
			vkDevice->logicalDevice, vkSurface);

		CreateClearRenderPass();
		CreateClearFramebuffer();

		CreateCommandPool();
		CreateCommandBuffers();
		CreateSyncObjects();

		VulkanShader vkDefaultShader = VulkanShader(vkDevice->logicalDevice, defaultSpriteVertexShader->data, defaultSpriteFragmentShader->data);
		VulkanGraphicsPipeline vkGraphicsPipeline = VulkanGraphicsPipeline(vkDevice->physicalDevice, vkDevice->logicalDevice, vkSwapchain, &vkDefaultShader);
		vkGraphicsPipeline.CreateDescriptorPools(vkDevice->logicalDevice, 100); // :))
		graphicsPipelines.push_back(vkGraphicsPipeline);

		static const size_t maxCharCount = 1000;

		Buffer textVertexBuffer, textIndexBuffer;
		textVertexBuffer.Allocate(maxCharCount * 4 * sizeof(VertexColorTexture));
		textIndexBuffer.Allocate(maxCharCount * 6 * sizeof(uint16_t));
		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_UniformBuffers[i].resize(100);

			m_TextIndexBuffers[i].resize(100);
			m_TextVertexBuffers[i].resize(100);

			for (int j = 0; j < 100; j++)
			{
				CreateUniformBuffer(vkDevice->physicalDevice, vkDevice->logicalDevice, sizeof(MVP), *m_UniformBuffers[i][j]);

				CreateVertexBuffer(textVertexBuffer, vkDevice->physicalDevice, vkDevice->logicalDevice, 
					vkDevice->graphicsQueue, vkCommandPool, *m_TextVertexBuffers[i][j]);

				CreateIndexBuffer(textIndexBuffer, vkDevice->physicalDevice, vkDevice->logicalDevice,
					vkDevice->graphicsQueue, vkCommandPool, *m_TextIndexBuffers[i][j]);
			}
		}
		textVertexBuffer.Release();
		textIndexBuffer.Release();
	}

	void VulkanRenderer::BeginDrawing()
	{
		vkSwapchain->AddFramebuffers(vkDevice->logicalDevice, graphicsPipelines.at(graphicsPipelines.size() - 1).renderPass);
	}

	void VulkanRenderer::DrawSprite(Texture2D* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
	{
		VulkanTexture* vkTexture = (VulkanTexture*)texture->GetGPUTexture();

		VulkanMesh* vkMesh = (VulkanMesh*)Mesh::Quad()->GetGPUMesh();

		ModelData modelData{};
		modelData.position = position;
		modelData.rotation = rotation;
		modelData.scale = scale;

		vkTextures.push_back(vkTexture);
		vkMeshes.push_back(vkMesh);
		vkMeshesModelData.push_back(modelData);
	}

	// TODO: Move this into UniformBuffer wraper class later
	void UpdateUniformBuffer(VulkanBuffer& uniformBuffer, MVP data)
	{
		memcpy(uniformBuffer.Map(), &data, sizeof(data));
	}

	void VulkanRenderer::EndDrawing()
	{
		auto& currentPipeline = graphicsPipelines[graphicsPipelines.size() - 1];

		if (currentPipeline.descriptorSets[currentFrame].size() > 0)
			currentPipeline.FreeDescriptorSets(vkDevice->logicalDevice, currentFrame);

		currentPipeline.CreateDescriptorSets(vkDevice->logicalDevice, vkMeshes.size(), currentFrame);

		for (int i = 0; i < vkMeshes.size(); i++)
		{
			MVP mvp{};
			auto data = vkMeshesModelData[i];

			mvp.model = glm::scale(
				glm::rotate(
					glm::translate(glm::mat4(1), data.position),
					data.rotation.z,
					glm::vec3(0, 0, 1)),
				data.scale);

			mvp.view = camera->GetViewMatrix();
			mvp.proj = camera->GetProjectionMatrix();

			UpdateUniformBuffer(*m_UniformBuffers[currentFrame][i], mvp);
			auto vkTexture = vkTextures[i];
			auto vkMesh = vkMeshes[i];

			currentPipeline.UpdateDescriptorSet(
				vkDevice->logicalDevice,
				vkTexture->textureImageView,
				vkTexture->textureSampler,
				*m_UniformBuffers[currentFrame][i], currentFrame, i);
		}

		DrawFrame();

		vkTextures.clear();
		vkMeshes.clear();
		vkMeshesModelData.clear();
	}

	void VulkanRenderer::DrawString(const std::string& string, Font* font, const glm::vec3 position)
	{
		struct TextureParams
		{
			glm::vec4 Color{ 1.0f };
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};

		TextureParams textParams;

		const auto& fontGeometry = font->GetMSDFData()->FontGeometry;
		const auto& metrics = fontGeometry.getMetrics();
		Texture2D* fontAtlas = font->GetAtlasTexture();

		//s_Data.FontAtlasTexture = fontAtlas;

		double x = 0.0;
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
		double y = 0.0;

		const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

		for (size_t i = 0; i < string.size(); i++)
		{
			char character = string[i];
			if (character == '\r')
				continue;

			if (character == '\n')
			{
				x = 0;
				y -= fsScale * metrics.lineHeight + textParams.LineSpacing;
				continue;
			}

			if (character == ' ')
			{
				float advance = spaceGlyphAdvance;
				if (i < string.size() - 1)
				{
					char nextCharacter = string[i + 1];
					double dAdvance;
					fontGeometry.getAdvance(dAdvance, character, nextCharacter);
					advance = (float)dAdvance;
				}

				x += fsScale * advance + textParams.Kerning;
				continue;
			}

			if (character == '\t')
			{
				// NOTE(Yan): is this right?
				x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.Kerning);
				continue;
			}

			auto glyph = fontGeometry.getGlyph(character);
			if (!glyph)
				glyph = fontGeometry.getGlyph('?');
			if (!glyph)
				return;

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin((float)al, (float)ab);
			glm::vec2 texCoordMax((float)ar, (float)at);

			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pb, pr, pt);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / fontAtlas->GetWidth();
			float texelHeight = 1.0f / fontAtlas->GetHeight();
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);

			// render here
			/*s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin, 0.0f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = texCoordMin;
			s_Data.TextVertexBufferPtr->EntityID = entityID;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMin.x, quadMax.y, 0.0f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = { texCoordMin.x, texCoordMax.y };
			s_Data.TextVertexBufferPtr->EntityID = entityID;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax, 0.0f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = texCoordMax;
			s_Data.TextVertexBufferPtr->EntityID = entityID;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextVertexBufferPtr->Position = transform * glm::vec4(quadMax.x, quadMin.y, 0.0f, 1.0f);
			s_Data.TextVertexBufferPtr->Color = textParams.Color;
			s_Data.TextVertexBufferPtr->TexCoord = { texCoordMax.x, texCoordMin.y };
			s_Data.TextVertexBufferPtr->EntityID = entityID;
			s_Data.TextVertexBufferPtr++;

			s_Data.TextIndexCount += 6;
			s_Data.Stats.QuadCount++;*/

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance + textParams.Kerning;
			}
		}
	}

	void VulkanRenderer::WaitDeviceIdle()
	{
		vkDeviceWaitIdle(vkDevice->logicalDevice);
	}

	VulkanTexture* VulkanRenderer::UploadTextureToGPU(Texture2D* texture)
	{
		return new VulkanTexture(vkDevice->physicalDevice, vkDevice->logicalDevice, vkDevice->graphicsQueue, vkCommandPool, texture);
	}

	void VulkanRenderer::ReleaseGPUTexture(VulkanTexture* gpuTexture)
	{
		gpuTexture->Dispose(vkDevice->logicalDevice);
		delete gpuTexture;
	}

	VulkanMesh* VulkanRenderer::UploadMeshToGPU(Buffer vertices, Buffer indices)
	{
		return LoadMesh(*m_ResourceAllocator, vkDevice->physicalDevice, vkDevice->logicalDevice, vkDevice->graphicsQueue, vkCommandPool, vertices, indices);
	}

	void VulkanRenderer::ReleaseGPUMesh(VulkanMesh* gpuMesh)
	{
		gpuMesh->Dispose(vkDevice->logicalDevice);
		delete gpuMesh;
	}

	void VulkanRenderer::DrawFrame()
	{
		vkWaitForFences(vkDevice->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE,
			UINT64_MAX);

		VkResult result = vkAcquireNextImageKHR(
			vkDevice->logicalDevice, vkSwapchain->swapchain, UINT64_MAX, imageAvailableSemaphores[currentFrame],
			VK_NULL_HANDLE, &imageIndex);

		vkResetFences(vkDevice->logicalDevice, 1, &inFlightFences[currentFrame]);

		vkResetCommandBuffer(commandBuffers[currentFrame], 0);

		RecordCommandBuffer();

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		BLD_VK_CHECK(vkQueueSubmit(vkDevice->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]),
			"Failed to submit draw command buffer");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { vkSwapchain->swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(vkDevice->presentQueue, &presentInfo);

		currentFrame = (currentFrame + 1) % FRAMES_IN_FLIGHT;
	}


	void VulkanRenderer::RecordCommandBuffer()
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		BLD_VK_CHECK(vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo),
			"Failed to begin recording command buffer");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = graphicsPipelines[graphicsPipelines.size() - 1].renderPass;
		renderPassInfo.framebuffer = vkSwapchain->renderPassFramebuffersMap[graphicsPipelines[graphicsPipelines.size() - 1].renderPass][imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vkSwapchain->extent;

		std::array<VkClearValue, 2> clearValues{};
		auto color = backgroundColor.GetValues();
		clearValues[0].color = { {color.r,color.g,color.b, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo,
			VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
			graphicsPipelines[graphicsPipelines.size() - 1].graphicsPipeline);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)vkSwapchain->extent.width;
		viewport.height = (float)vkSwapchain->extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = vkSwapchain->extent;
		vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);

		// Loop meshes draw command
		int i = 0;
		for (auto& mesh : vkMeshes)
		{
			VkPipelineLayout& pipelineLayout = graphicsPipelines[graphicsPipelines.size() - 1].pipelineLayout;
			VkDescriptorSet& descriptorSet = graphicsPipelines[graphicsPipelines.size() - 1].descriptorSets[currentFrame][i];
			mesh->Draw(commandBuffers[currentFrame], pipelineLayout, descriptorSet);
			i++;
		}

		vkCmdEndRenderPass(commandBuffers[currentFrame]);

		BLD_VK_CHECK(vkEndCommandBuffer(commandBuffers[currentFrame]),
			"Failed to record command buffer!");
	}


	void VulkanRenderer::Clear(BladeEngine::Graphics::Color color) {

		backgroundColor = color;
		vkWaitForFences(vkDevice->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE,
			UINT64_MAX);

		if (graphicsPipelines.size() > 0 && graphicsPipelines[graphicsPipelines.size() - 1].descriptorSets[currentFrame].size() > 0)
			graphicsPipelines[graphicsPipelines.size() - 1].FreeDescriptorSets(vkDevice->logicalDevice, currentFrame);

		//uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(
			vkDevice->logicalDevice,
			vkSwapchain->swapchain,
			UINT64_MAX,
			imageAvailableSemaphores[currentFrame],
			VK_NULL_HANDLE,
			&imageIndex);

		vkResetFences(vkDevice->logicalDevice, 1, &inFlightFences[currentFrame]);

		vkResetCommandBuffer(commandBuffers[currentFrame], 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		BLD_VK_CHECK(vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo),
			"Failed to begin recording command buffer");

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;

		//TODO: CREATE CLEAR VALUES IN INIT
		renderPassInfo.renderPass = vkClearRenderPass;
		renderPassInfo.framebuffer = vkClearFramebuffers[imageIndex];

		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = vkSwapchain->extent;

		std::array<VkClearValue, 2> clearValues{};
		auto clearColor = color.GetValues();
		clearValues[0].color = { {clearColor.r, clearColor.g, clearColor.b, clearColor.a} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo,
			VK_SUBPASS_CONTENTS_INLINE);



		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)vkSwapchain->extent.width;
		viewport.height = (float)vkSwapchain->extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = vkSwapchain->extent;
		vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);

		vkCmdEndRenderPass(commandBuffers[currentFrame]);

		BLD_VK_CHECK(vkEndCommandBuffer(commandBuffers[currentFrame]),
			"Failed to record command buffer");

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		BLD_VK_CHECK(vkQueueSubmit(vkDevice->graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]),
			"Failed to submit draw command buffer");

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { vkSwapchain->swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &imageIndex;

		result = vkQueuePresentKHR(vkDevice->presentQueue, &presentInfo);

		/*if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			framebufferResized) {
		  framebufferResized = false;
		  recreateSwapChain();
		} else if (result != VK_SUCCESS) {
		  throw std::runtime_error("failed to present swap chain image!");
		}*/

		currentFrame = (currentFrame + 1) % FRAMES_IN_FLIGHT;
	}

	void VulkanRenderer::CreateClearRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = vkSwapchain->imageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 1> attachments = { colorAttachment };

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		renderPassInfo.pAttachments = attachments.data();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(vkDevice->logicalDevice, &renderPassInfo, nullptr, &vkClearRenderPass) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void VulkanRenderer::CreateClearFramebuffer()
	{
		vkClearFramebuffers.resize(vkSwapchain->imageViews.size());

		for (size_t i = 0; i < vkSwapchain->imageViews.size(); i++) {
			std::array<VkImageView, 1> attachments = { vkSwapchain->imageViews[i] };

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = vkClearRenderPass;
			framebufferInfo.attachmentCount =
				static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = vkSwapchain->extent.width;
			framebufferInfo.height = vkSwapchain->extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(vkDevice->logicalDevice, &framebufferInfo, nullptr,
				&vkClearFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void VulkanRenderer::CreateCommandPool()
	{
		GraphicsFamily graphicsFamily = GetGraphicsFamily(vkDevice->physicalDevice, vkSurface);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = graphicsFamily.graphicsFlagIndex.value();

		if (vkCreateCommandPool(vkDevice->logicalDevice, &poolInfo, nullptr, &vkCommandPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics command pool!");
		}
	}

	void VulkanRenderer::CreateCommandBuffers()
	{
		commandBuffers.resize(FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = vkCommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		if (vkAllocateCommandBuffers(vkDevice->logicalDevice, &allocInfo, commandBuffers.data()) !=
			VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void VulkanRenderer::CreateSyncObjects()
	{
		imageAvailableSemaphores.resize(FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(FRAMES_IN_FLIGHT);
		inFlightFences.resize(FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(vkDevice->logicalDevice, &semaphoreInfo, nullptr,
				&imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(vkDevice->logicalDevice, &semaphoreInfo, nullptr,
					&renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(vkDevice->logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]) !=
				VK_SUCCESS) {
				throw std::runtime_error(
					"failed to create synchronization objects for a frame!");
			}
		}
	}
}


