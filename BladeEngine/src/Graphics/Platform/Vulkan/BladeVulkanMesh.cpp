#include "BladeVulkanMesh.hpp"
#include "BladeVulkanUtils.hpp"
#include <string.h>

using namespace BladeEngine::Graphics::Vulkan;

BladeEngine::Graphics::Vulkan::VulkanMesh BladeEngine::Graphics::Vulkan::LoadMesh(
    VkPhysicalDevice physicalDevice,VkDevice device,VkQueue graphicsQueue,VkCommandPool commandPool,
    std::vector<BladeEngine::Graphics::VertexColorTexture> vertices,
    std::vector<uint16_t> elements)
{
    VulkanMesh mesh;
    mesh.indices = elements;
    mesh.vertices = vertices;
    CreateVertexBuffer(vertices,physicalDevice,device,graphicsQueue,commandPool, mesh.vertexBuffer,mesh.vertexBufferMemory);
    CreateElementsBuffer(elements,physicalDevice,device,graphicsQueue,commandPool, mesh.elementsBuffer,mesh.elementsBufferMemory);
    CreateUniformBuffer(physicalDevice,device,sizeof(MVP),mesh.uniformBuffer,mesh.uniformBufferMemory,mesh.uniformBufferMapped);
    return mesh;
}

void BladeEngine::Graphics::Vulkan::VulkanMesh::UpdateUniformBuffer(BladeEngine::Graphics::Vulkan::MVP data)
{
      memcpy(uniformBufferMapped, &data, sizeof(data));
}


void BladeEngine::Graphics::Vulkan::VulkanMesh::Draw(VkCommandBuffer commandBuffer,VkPipelineLayout &pipelineLayout, VkDescriptorSet &descriptorSet)
{
    VkBuffer vertexBuffers[] = {vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, elementsBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            pipelineLayout, 0, 1, &descriptorSet,
                            0, nullptr);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0,
                     0, 0);
}

void BladeEngine::Graphics::Vulkan::VulkanMesh::Dispose(VkDevice device)
{
    vkFreeMemory(device,vertexBufferMemory,nullptr);
    vkDestroyBuffer(device,vertexBuffer,nullptr);
    
    vkFreeMemory(device,elementsBufferMemory,nullptr);
    vkDestroyBuffer(device,elementsBuffer,nullptr);
}



