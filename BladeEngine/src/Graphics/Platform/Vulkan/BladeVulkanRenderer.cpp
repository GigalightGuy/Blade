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

	void VulkanRenderer::CreateDebugResources()
	{
		m_UVDebugTexture = new Texture2D("assets/sprites/tex_DebugUVTiles.png");
		m_UVDebugTexture->CreateGPUTexture();
	}

	void VulkanRenderer::Init(Window* window)
	{
		defaultSpriteVertexShader = new Shader("assets/shaders/default.vert", ShaderType::VERTEX);
		defaultSpriteFragmentShader = new Shader("assets/shaders/default.frag", ShaderType::FRAGMENT);

		defaultTextVertexShader = new Shader("assets/shaders/defaultText.vert", ShaderType::VERTEX);
		defaultTextFragmentShader = new Shader("assets/shaders/defaultText.frag", ShaderType::FRAGMENT);

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

		VulkanRenderPass* renderPass = new VulkanRenderPass(
			vkDevice, vkSwapchain->imageFormat, 
			vkSwapchain->FindDepthFormat(vkDevice->physicalDevice));
		m_RenderPasses.push_back(renderPass);

		static const size_t maxQuadCount = 100;
		static const size_t maxCharCount = 1000;

		VulkanShader vkDefaultSpriteShader = VulkanShader(vkDevice->logicalDevice, defaultSpriteVertexShader->data, defaultSpriteFragmentShader->data);
		VulkanGraphicsPipeline* vkSpriteGraphicsPipeline = new VulkanGraphicsPipeline(
			vkDevice->logicalDevice, renderPass->GetRenderPass(), &vkDefaultSpriteShader);
		vkSpriteGraphicsPipeline->CreateDescriptorPools(vkDevice->logicalDevice, maxQuadCount); // :))
		m_GraphicsPipelinesMap[renderPass].push_back(vkSpriteGraphicsPipeline);

		VulkanShader vkDefaultTextShader = VulkanShader(vkDevice->logicalDevice, defaultTextVertexShader->data, defaultTextFragmentShader->data);
		VulkanGraphicsPipeline* vkTextGraphicsPipeline = new VulkanGraphicsPipeline(
			vkDevice->logicalDevice, renderPass->GetRenderPass(), &vkDefaultTextShader);
		vkTextGraphicsPipeline->CreateDescriptorPools(vkDevice->logicalDevice, maxQuadCount); // :))
		m_GraphicsPipelinesMap[renderPass].push_back(vkTextGraphicsPipeline);

		vkSwapchain->CreateFramebuffers(vkDevice->logicalDevice, renderPass->GetRenderPass());


		m_TextQuadCounts.resize(maxQuadCount);

		BufferDescription textVertexBufferDescription;
		textVertexBufferDescription.Usage = BufferUsage::Vertex;
		textVertexBufferDescription.AllocationUsage = BufferAllocationUsage::HostWrite;
		textVertexBufferDescription.KeepMapped = true;
		textVertexBufferDescription.Size = maxCharCount * 4 * sizeof(VertexColorTexture);

		BufferDescription textIndexBufferDescription;
		textIndexBufferDescription.Usage = BufferUsage::Index;
		textIndexBufferDescription.AllocationUsage = BufferAllocationUsage::HostWrite;
		textIndexBufferDescription.KeepMapped = true;
		textIndexBufferDescription.Size = maxCharCount * 6 * sizeof(uint16_t);

		for (int i = 0; i < FRAMES_IN_FLIGHT; i++)
		{
			m_UniformBuffers[i].resize(2 * maxQuadCount);

			m_TextIndexBuffers[i].resize(maxQuadCount);
			m_TextVertexBuffers[i].resize(maxQuadCount);

			for (int j = 0; j < maxQuadCount; j++)
			{
				m_UniformBuffers[i][j * 2 + 0] = CreateUniformBuffer(*m_ResourceAllocator, sizeof(MVP));
				m_UniformBuffers[i][j * 2 + 1] = CreateUniformBuffer(*m_ResourceAllocator, sizeof(MVP));

				m_TextVertexBuffers[i][j] = new VulkanBuffer(textVertexBufferDescription, *m_ResourceAllocator);
				m_TextIndexBuffers[i][j] = new VulkanBuffer(textIndexBufferDescription, *m_ResourceAllocator);
			}
		}
	}

	void VulkanRenderer::BeginDrawing()
	{
	
		m_TextCount = 0;
		m_UniformBufferCount = 0;
	}

	void VulkanRenderer::DrawSprite(Texture2D* texture, 
		const glm::vec3& position, const glm::vec3& rotation, 
		const glm::vec3& scale, const glm::vec4& uvTransform)
	{
		VulkanTexture* vkTexture = (VulkanTexture*)texture->GetGPUTexture();

		VulkanMesh* vkMesh = (VulkanMesh*)Mesh::Quad()->GetGPUMesh();

		ModelData modelData{};
		modelData.position = position;
		modelData.rotation = rotation;
		modelData.scale = scale;

		PushConstantData pushConstant{};
		pushConstant.UVTransform = uvTransform;

		vkTextures.push_back(vkTexture);
		vkMeshes.push_back(vkMesh);
		vkMeshesModelData.push_back(modelData);
		m_PushConstantsData.push_back(pushConstant);
	}

	// TODO: Move this into UniformBuffer wraper class later
	void UpdateUniformBuffer(VulkanBuffer& uniformBuffer, MVP data)
	{
		memcpy(uniformBuffer.Map(), &data, sizeof(data));
	}

	void VulkanRenderer::EndDrawing()
	{
		const auto renderPass = m_RenderPasses[0];
		auto currentPipeline = m_GraphicsPipelinesMap[renderPass][0];

		vkWaitForFences(vkDevice->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
		
		if (currentPipeline->descriptorSets[currentFrame].size() > 0)
			currentPipeline->FreeDescriptorSets(vkDevice->logicalDevice, currentFrame);

		currentPipeline->CreateDescriptorSets(vkDevice->logicalDevice, vkMeshes.size(), currentFrame);

		uint32_t i;
		for (i = 0; i < vkMeshes.size(); i++)
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

			currentPipeline->UpdateDescriptorSet(
				vkDevice->logicalDevice,
				vkTexture->textureImageView,
				vkTexture->textureSampler,
				*m_UniformBuffers[currentFrame][i], currentFrame, i);
		}

		currentPipeline = m_GraphicsPipelinesMap[renderPass][1];

		if (currentPipeline->descriptorSets[currentFrame].size() > 0)
			currentPipeline->FreeDescriptorSets(vkDevice->logicalDevice, currentFrame);

		currentPipeline->CreateDescriptorSets(vkDevice->logicalDevice, m_TextCount, currentFrame);

		// auto debugTexture = (VulkanTexture*)m_UVDebugTexture->GetGPUTexture();

		for (uint32_t j = 0; j < m_TextCount; j++, i++)
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

			currentPipeline->UpdateDescriptorSet(
				vkDevice->logicalDevice,
				vkTexture->textureImageView,
				vkTexture->textureSampler,
				*m_UniformBuffers[currentFrame][i], currentFrame, j);
		}

		DrawFrame();

		vkTextures.clear();
		vkMeshes.clear();
		vkMeshesModelData.clear();
	}

	void VulkanRenderer::DrawString(const std::string& string, Font* font, 
		const glm::vec3& position, const glm::vec3& rotation, const glm::vec3& scale)
	{
		Buffer vertexBuffer;
		vertexBuffer.Allocate(4 * string.size() * sizeof(VertexColorTexture));

		VertexColorTexture* vertexBufferData = vertexBuffer.As<VertexColorTexture>();

		Buffer indexBuffer;
		indexBuffer.Allocate(6 * string.size() * sizeof(uint16_t));

		uint16_t* indexBufferData = indexBuffer.As<uint16_t>();

		struct TextParams
		{
			glm::vec4 Color{ 1.0f };
			float Kerning = 0.0f;
			float LineSpacing = 0.0f;
		};

		TextParams textParams;

		const auto& fontGeometry = font->GetMSDFData()->FontGeometry;
		const auto& metrics = fontGeometry.getMetrics();
		Texture2D* fontAtlas = font->GetAtlasTexture();

		vkTextures.push_back((VulkanTexture*)fontAtlas->GetGPUTexture());

		double x = 0.0;
		double fsScale = 1.0 / (metrics.ascenderY - metrics.descenderY);
		double y = 0.0;

		const float spaceGlyphAdvance = fontGeometry.getGlyph(' ')->getAdvance();

		uint32_t quadCount = 0;

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
				x += 4.0f * (fsScale * spaceGlyphAdvance + textParams.Kerning);
				continue;
			}

			auto glyph = fontGeometry.getGlyph(character);
			if (!glyph)
				glyph = fontGeometry.getGlyph('?');
			if (!glyph)
				return;

			float atlasWidth = (float)(fontAtlas->GetWidth()), atlasHeight = (float)(fontAtlas->GetHeight());

			double al, ab, ar, at;
			glyph->getQuadAtlasBounds(al, ab, ar, at);
			glm::vec2 texCoordMin((float)al, atlasHeight - (float)at);
			glm::vec2 texCoordMax((float)ar, atlasHeight - (float)ab);

			// switched bottom and top bounds
			double pl, pb, pr, pt;
			glyph->getQuadPlaneBounds(pl, pt, pr, pb);
			glm::vec2 quadMin((float)pl, (float)pb);
			glm::vec2 quadMax((float)pr, (float)pt);

			quadMin *= fsScale, quadMax *= fsScale;
			quadMin += glm::vec2(x, y);
			quadMax += glm::vec2(x, y);

			float texelWidth = 1.0f / atlasWidth;
			float texelHeight = 1.0f / atlasHeight;
			texCoordMin *= glm::vec2(texelWidth, texelHeight);
			texCoordMax *= glm::vec2(texelWidth, texelHeight);


			vertexBufferData[quadCount * 4 + 0].position = glm::vec3(quadMin, 0.0f);
			vertexBufferData[quadCount * 4 + 0].color = textParams.Color;
			vertexBufferData[quadCount * 4 + 0].textureCoordinate = texCoordMin;

			vertexBufferData[quadCount * 4 + 1].position = glm::vec3(quadMin.x, quadMax.y, 0.0f);
			vertexBufferData[quadCount * 4 + 1].color = textParams.Color;
			vertexBufferData[quadCount * 4 + 1].textureCoordinate = { texCoordMin.x, texCoordMax.y };

			vertexBufferData[quadCount * 4 + 2].position = glm::vec3(quadMax, 0.0f);
			vertexBufferData[quadCount * 4 + 2].color = textParams.Color;
			vertexBufferData[quadCount * 4 + 2].textureCoordinate = texCoordMax;

			vertexBufferData[quadCount * 4 + 3].position = glm::vec3(quadMax.x, quadMin.y, 0.0f);
			vertexBufferData[quadCount * 4 + 3].color = textParams.Color;
			vertexBufferData[quadCount * 4 + 3].textureCoordinate = { texCoordMax.x, texCoordMin.y };

			static const uint16_t indices[6] {
				0, 1, 2,
				2, 3, 0
			};

			indexBufferData[quadCount * 6 + 0] = indices[0] + quadCount * 4;
			indexBufferData[quadCount * 6 + 1] = indices[1] + quadCount * 4;
			indexBufferData[quadCount * 6 + 2] = indices[2] + quadCount * 4;
			indexBufferData[quadCount * 6 + 3] = indices[3] + quadCount * 4;
			indexBufferData[quadCount * 6 + 4] = indices[4] + quadCount * 4;
			indexBufferData[quadCount * 6 + 5] = indices[5] + quadCount * 4;

			quadCount++;

			if (i < string.size() - 1)
			{
				double advance = glyph->getAdvance();
				char nextCharacter = string[i + 1];
				fontGeometry.getAdvance(advance, character, nextCharacter);

				x += fsScale * advance + textParams.Kerning;
			}
		}

		memcpy(m_TextVertexBuffers[currentFrame][m_TextCount]->Map(), vertexBufferData,
			quadCount * 4 * sizeof(VertexColorTexture));
		memcpy(m_TextIndexBuffers[currentFrame][m_TextCount]->Map(), indexBufferData,
			quadCount * 6 * sizeof(uint16_t));

		ModelData modelData;
		modelData.position = position;
		modelData.rotation = rotation;
		modelData.scale = scale;
		vkMeshesModelData.push_back(modelData);

		m_TextQuadCounts[m_TextCount] = quadCount;
		m_TextCount++;

		vertexBuffer.Release();
		indexBuffer.Release();

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
		return LoadMesh(*m_ResourceAllocator, vkDevice->logicalDevice, vkDevice->graphicsQueue, vkCommandPool, vertices, indices);
	}

	void VulkanRenderer::ReleaseGPUMesh(VulkanMesh* gpuMesh)
	{
		gpuMesh->Dispose(vkDevice->logicalDevice);
		delete gpuMesh;
	}

	void VulkanRenderer::DrawFrame()
	{
		/*vkWaitForFences(vkDevice->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE,
			UINT64_MAX);*/

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
		renderPassInfo.renderPass = m_RenderPasses[0]->GetRenderPass();
		renderPassInfo.framebuffer = vkSwapchain->m_FramebuffersMap[m_RenderPasses[0]->GetRenderPass()][imageIndex];
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
			m_GraphicsPipelinesMap[m_RenderPasses[0]][0]->graphicsPipeline);

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
			const auto& renderPass = m_RenderPasses[0];
			const auto& graphicsPipeline = m_GraphicsPipelinesMap[renderPass][0];

			vkCmdPushConstants(commandBuffers[currentFrame], m_GraphicsPipelinesMap[m_RenderPasses[0]][0]->pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantData), &m_PushConstantsData[i]);

			VkPipelineLayout& pipelineLayout = graphicsPipeline->pipelineLayout;
			VkDescriptorSet& descriptorSet = graphicsPipeline->descriptorSets[currentFrame][i];
			mesh->Draw(commandBuffers[currentFrame], pipelineLayout, descriptorSet);
			i++;
		}

		vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_GraphicsPipelinesMap[m_RenderPasses[0]][1]->graphicsPipeline);

		for (uint32_t j = 0; j < m_TextCount; j++, i++)
		{
			VkPipelineLayout& pipelineLayout = m_GraphicsPipelinesMap[m_RenderPasses[0]][1]->pipelineLayout;
			VkDescriptorSet& descriptorSet = m_GraphicsPipelinesMap[m_RenderPasses[0]][1]->descriptorSets[currentFrame][j];

			VkBuffer vertexBuffers[] = { m_TextVertexBuffers[currentFrame][j]->GetBuffer() };
			VkDeviceSize offsets[] = { 0 };

			vkCmdBindVertexBuffers(commandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);

			vkCmdBindIndexBuffer(commandBuffers[currentFrame], 
				m_TextIndexBuffers[currentFrame][j]->GetBuffer(), 
				0, VK_INDEX_TYPE_UINT16);

			vkCmdBindDescriptorSets(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
				pipelineLayout, 0, 1, &descriptorSet,
				0, nullptr);

			vkCmdDrawIndexed(commandBuffers[currentFrame], m_TextQuadCounts[j] * 6, 1, 0, 0, 0);
		}

		vkCmdEndRenderPass(commandBuffers[currentFrame]);

		BLD_VK_CHECK(vkEndCommandBuffer(commandBuffers[currentFrame]),
			"Failed to record command buffer!");
	}


	void VulkanRenderer::Clear(BladeEngine::Graphics::Color color) {

		backgroundColor = color;
		vkWaitForFences(vkDevice->logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE,
			UINT64_MAX);

		if (m_GraphicsPipelinesMap[m_RenderPasses[0]].size() > 0 
			&& m_GraphicsPipelinesMap[m_RenderPasses[0]][0]->descriptorSets[currentFrame].size() > 0)
			m_GraphicsPipelinesMap[m_RenderPasses[0]][0]->FreeDescriptorSets(vkDevice->logicalDevice, currentFrame);

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


