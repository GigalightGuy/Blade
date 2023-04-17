#pragma once
#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <utility>
#include <vector>
#include <vulkan/vulkan.h>

namespace BladeEngine {

namespace Graphics {

namespace Vulkan {

struct GraphicsFamily {
  std::optional<uint32_t> graphicsFlagIndex;
  std::optional<uint32_t> presentFlagIndex;

  bool IsComplete();
};

GraphicsFamily GetGraphicsFamily(VkPhysicalDevice physicalDevice,
                                 VkSurfaceKHR surface);

struct ComputeFamily {
  std::optional<uint32_t> computeFlagIndex;

  bool IsComplete();
};

ComputeFamily GetComputeFamily(VkPhysicalDevice physicalDevice);

struct TransferFamily {
  VkQueueFlagBits f;
  std::optional<uint32_t> transferFlagIndex;
  bool IsComplete();
};

TransferFamily GetTransferFamily(VkPhysicalDevice physicalDevice);

} // namespace Vulkan
} // namespace Graphics
} // namespace BladeEngine
