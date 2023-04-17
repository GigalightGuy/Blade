#include "BladeVulkanRenderer.hpp"
#include "BladeVulkanQueueFamily.hpp"
#include "../../Shader.hpp"
#include "BladeVulkanUtils.hpp"
#include <string.h>
#include <utility>

using namespace BladeEngine::Graphics::Vulkan;

BladeEngine::Graphics::Vulkan::VulkanRenderer::VulkanRenderer(BladeEngine::Camera *camera, Window* window)
{
    this->camera = camera;
    Init(window);
}

BladeEngine::Graphics::Vulkan::VulkanRenderer::~VulkanRenderer()
{
}

void BladeEngine::Graphics::Vulkan::VulkanRenderer::Init(Window* window)
{
    defaultVertexShader = new BladeEngine::Graphics::Shader("../assets/shaders/default.vert",BladeEngine::Graphics::ShaderType::VERTEX);
    defaultFragmentShader = new BladeEngine::Graphics::Shader("../assets/shaders/default.frag",BladeEngine::Graphics::ShaderType::FRAGMENT);
    
    std::vector<const char *> extensions = window->GetRequiredExtensions();
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    vkInstance = new Vulkan::VulkanInstance("Blade GDK Program", VK_MAKE_VERSION(1, 0, 0), extensions, {"VK_LAYER_KHRONOS_validation"});
    vkSurface = window->CreateWindowSurface(vkInstance->instance);
    vkDevice = new Vulkan::VulkanDevice(vkInstance->instance, vkSurface,
                                      {VK_KHR_SWAPCHAIN_EXTENSION_NAME});
    
    vkSwapchain = new Vulkan::VulkanSwapchain(window, vkDevice->physicalDevice,
                                            vkDevice->logicalDevice, vkSurface);
    
    CreateClearRenderPass();
    CreateClearFramebuffer();
    
    CreateCommandPool();
    CreateCommandBuffers();
    CreateSyncObjects();

    BladeEngine::Graphics::Vulkan::VulkanShader vkDefaultShader = VulkanShader(vkDevice->logicalDevice,defaultVertexShader->data,defaultFragmentShader->data);
    BladeEngine::Graphics::Vulkan::VulkanGraphicsPipeline vkGraphicsPipeline = VulkanGraphicsPipeline(vkDevice->physicalDevice,vkDevice->logicalDevice,vkSwapchain,&vkDefaultShader);
    vkGraphicsPipeline.CreateDescriptorPools(vkDevice->logicalDevice, 100); // :))
    graphicsPipelines.push_back(vkGraphicsPipeline);
}

void BladeEngine::Graphics::Vulkan::VulkanRenderer::BeginDrawing()
{
    vkSwapchain->AddFramebuffers(vkDevice->logicalDevice,graphicsPipelines.at(graphicsPipelines.size()-1).renderPass);
}

void BladeEngine::Graphics::Vulkan::VulkanRenderer::Draw(BladeEngine::Graphics::Texture2D* texture, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    VulkanTexture* vkTexture = (VulkanTexture*)texture->GetGPUTexture();

    BladeEngine::Graphics::Mesh mesh = Quad(1,1);
    std::vector<VertexColorTexture> vertices;
    int i = 0;
    for(i = 0; i < mesh.vertices.size();i++)
    {
        VertexColorTexture vertice{};
        vertice.color = glm::vec4(1);
        vertice.position = mesh.vertices[i];
        vertice.textureCoordinate = mesh.uvs[i];
        vertices.push_back(vertice);
    }
    std::vector<uint16_t> elements;
    for(i = 0; i < mesh.elements.size();i++)
    {
        elements.push_back(mesh.elements[i]);
    }

    BladeEngine::Graphics::Vulkan::VulkanMesh vkMesh = LoadMesh(vkDevice->physicalDevice,vkDevice->logicalDevice,vkDevice->graphicsQueue,vkCommandPool,vertices,elements);
    
    ModelData modelData{};
    modelData.position = position;
    modelData.rotation = rotation;
    modelData.scale = scale;
    
    vkTextures.push_back(vkTexture);
    vkMeshes.push_back(vkMesh);
    vkMeshesModelData.push_back(modelData);
}

void BladeEngine::Graphics::Vulkan::VulkanRenderer::EndDrawing()
{
    auto& currentPipeline = graphicsPipelines[graphicsPipelines.size()-1];

    if (currentPipeline.descriptorSets[currentFrame].size() > 0)
        currentPipeline.FreeDescriptorSets(vkDevice->logicalDevice, currentFrame);

    currentPipeline.CreateDescriptorSets(vkDevice->logicalDevice,vkMeshes.size(), currentFrame);
    
    for(int i = 0; i < vkMeshes.size(); i++)
    {
        BladeEngine::Graphics::Vulkan::MVP mvp{};
        auto data = vkMeshesModelData[i];

        mvp.model = glm::scale(
                        glm::rotate(
                            glm::translate(glm::mat4(1), data.position),
                            data.rotation.z,
                            glm::vec3(0,0,1)), 
                        data.scale);
        
        mvp.view = camera->GetViewMatrix();
        mvp.proj = camera->GetProjectionMatrix();
        
        vkMeshes[i].UpdateUniformBuffer(mvp);
        auto vkTexture = vkTextures[i];
        auto vkMesh = vkMeshes[i];
        currentPipeline.UpdateDescriptorSet(
            vkDevice->logicalDevice,
            vkTexture->textureImageView,
            vkTexture->textureSampler,
            vkMesh,currentFrame,i);
    }
    
    DrawFrame();

    vkTextures.clear();
    vkMeshes.clear();
    vkMeshesModelData.clear();
}

VulkanTexture* VulkanRenderer::CreateGPUTexture(Texture2D* texture)
{
    return new VulkanTexture(vkDevice->physicalDevice,vkDevice->logicalDevice,vkDevice->graphicsQueue,vkCommandPool,vkSwapchain,texture);
}

void VulkanRenderer::DestroyGPUTexture(VulkanTexture* gpuTexture)
{
    gpuTexture->Dispose(vkDevice->logicalDevice);
    delete gpuTexture;
}

void BladeEngine::Graphics::Vulkan::VulkanRenderer::DrawFrame()
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

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vkDevice->graphicsQueue, 1, &submitInfo,
                      inFlightFences[currentFrame]) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {vkSwapchain->swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(vkDevice->presentQueue, &presentInfo);

    currentFrame = (currentFrame + 1) % FRAMES_IN_FLIGHT;
}


void BladeEngine::Graphics::Vulkan::VulkanRenderer::RecordCommandBuffer()
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = graphicsPipelines[graphicsPipelines.size()-1].renderPass;
    renderPassInfo.framebuffer = vkSwapchain->renderPassFramebuffersMap[graphicsPipelines[graphicsPipelines.size()-1].renderPass][imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vkSwapchain->extent;

    std::array<VkClearValue, 2> clearValues{};
    auto color = backgroundColor.GetValues();
    clearValues[0].color = {{color.r,color.g,color.b, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[currentFrame], &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                      graphicsPipelines[graphicsPipelines.size()-1].graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)vkSwapchain->extent.width;
    viewport.height = (float)vkSwapchain->extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffers[currentFrame], 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = vkSwapchain->extent;
    vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);
    
    // Loop meshses draw command
    int i = 0;
    for(auto& mesh : vkMeshes)
    {
        VkPipelineLayout& pipelineLayout = graphicsPipelines[graphicsPipelines.size()-1].pipelineLayout;
        VkDescriptorSet& descriptorSet = graphicsPipelines[graphicsPipelines.size()-1].descriptorSets[currentFrame][i];
        mesh.Draw(commandBuffers[currentFrame], pipelineLayout, descriptorSet);
        i+=1;
    }
    
    vkCmdEndRenderPass(commandBuffers[currentFrame]);

    if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
}


void BladeEngine::Graphics::Vulkan::VulkanRenderer::Clear(BladeEngine::Graphics::Color color){

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

    if (vkBeginCommandBuffer(commandBuffers[currentFrame], &beginInfo) != VK_SUCCESS) {
      throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    
    //TODO: CREATE CLEAR VALUES IN INIT
    renderPassInfo.renderPass = vkClearRenderPass;
    renderPassInfo.framebuffer = vkClearFramebuffers[imageIndex];
    
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = vkSwapchain->extent;

    std::array<VkClearValue, 2> clearValues{};
    auto clearColor = color.GetValues();
    clearValues[0].color = {{clearColor.r, clearColor.g, clearColor.b, clearColor.a}};
    clearValues[1].depthStencil = {1.0f, 0};

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
    scissor.offset = {0, 0};
    scissor.extent = vkSwapchain->extent;
    vkCmdSetScissor(commandBuffers[currentFrame], 0, 1, &scissor);
    
    vkCmdEndRenderPass(commandBuffers[currentFrame]);

    if (vkEndCommandBuffer(commandBuffers[currentFrame]) != VK_SUCCESS) {
      throw std::runtime_error("failed to record command buffer!");
    }
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(vkDevice->graphicsQueue, 1, &submitInfo,
                      inFlightFences[currentFrame]) != VK_SUCCESS) {
      throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {vkSwapchain->swapchain};
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

void BladeEngine::Graphics::Vulkan::VulkanRenderer::CreateClearRenderPass()
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

    std::array<VkAttachmentDescription, 1> attachments = {colorAttachment};
    
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

void BladeEngine::Graphics::Vulkan::VulkanRenderer::CreateClearFramebuffer()
{
    vkClearFramebuffers.resize(vkSwapchain->imageViews.size());

    for (size_t i = 0; i < vkSwapchain->imageViews.size(); i++) {
      std::array<VkImageView, 1> attachments = {vkSwapchain->imageViews[i]};

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

void BladeEngine::Graphics::Vulkan::VulkanRenderer::CreateCommandPool()
{
    BladeEngine::Graphics::Vulkan::GraphicsFamily graphicsFamily =
      BladeEngine::Graphics::Vulkan::GetGraphicsFamily(vkDevice->physicalDevice,
                                                       vkSurface);
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = graphicsFamily.graphicsFlagIndex.value();

    if (vkCreateCommandPool(vkDevice->logicalDevice, &poolInfo, nullptr,
                            &vkCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics command pool!");
    }
}

void BladeEngine::Graphics::Vulkan::VulkanRenderer::CreateCommandBuffers()
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

void BladeEngine::Graphics::Vulkan::VulkanRenderer::CreateSyncObjects()
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
