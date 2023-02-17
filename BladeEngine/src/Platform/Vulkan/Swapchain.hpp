#include <vector>
#include <vulkan/vulkan.h>

namespace BladeEngine {

namespace Vulkan {
struct SwapChainSupportDetails {
  VkSurfaceCapabilitiesKHR capabilities;
  std::vector<VkSurfaceFormatKHR> formats;
  std::vector<VkPresentModeKHR> presentModes;
};
static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device,
                                                     VkSurfaceKHR surface);

} // namespace Vulkan

} // namespace BladeEngine