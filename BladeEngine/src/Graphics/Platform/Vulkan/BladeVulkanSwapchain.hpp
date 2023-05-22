#pragma once
#include "../../../Core/Window.hpp"
#include <vector>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <utility>
#include <map>

namespace BladeEngine {
namespace Graphics {
namespace Vulkan {

class VulkanSwapchain {
public:
  VulkanSwapchain(BladeEngine::Window *window, VkPhysicalDevice physicalDevice,
                  VkDevice device, VkSurfaceKHR surface);
  ~VulkanSwapchain();

  void Dispose(VkDevice device);

  static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &availableFormats);
  static VkPresentModeKHR ChooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &availablePresentModes);
  static VkExtent2D
  ChooseSwapExtent(Window *window, const VkSurfaceCapabilitiesKHR &capabilities);

  //init
  VkSwapchainKHR swapchain;
  std::vector<VkImage> images;
  std::vector<VkImageView> imageViews;
  VkFormat imageFormat;
  VkExtent2D extent;

  VkImage depthImage;
  VkDeviceMemory depthImageMemory;
  VkImageView depthImageView;

  //Add new Framebuffer set on Begin Drawing
  std::unordered_map<VkRenderPass, std::vector<VkFramebuffer>> m_FramebuffersMap;

  
  VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice);
  
  VkImageView CreateImageView(VkDevice device, VkImage image, VkFormat format,
                              VkImageAspectFlags aspectFlags);
  

  void CreateFramebuffers(VkDevice device, VkRenderPass renderPass);

private:
  VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice,
                               const std::vector<VkFormat> &candidates,
                               VkImageTiling tiling,
                               VkFormatFeatureFlags features);
  void CreateDepthResources(VkPhysicalDevice physicalDevice, VkDevice device);

  void CreateSwapchain(BladeEngine::Window *window,
                       VkPhysicalDevice physicalDevice, VkDevice device,
                       VkSurfaceKHR surface);
  void CreateImageViews(VkDevice device);
};

} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
