#include <iostream>
#include <optional>
#include <vector>
#include <vulkan/vulkan.h>
namespace BladeEngine {

namespace Vulkan {

struct GraphicsPresentQueueFramilyIndices {
  std::optional<uint32_t> graphicsFamilyIndice;
  uint32_t graphicsFamilyQueueCount;
  std::optional<uint32_t> presentFamilyIndice;
  uint32_t presentFamilyQueueCount;

  VkBool32 isComplete();
};
struct ComputeQueueFamilyIndices {
  std::optional<uint32_t> computeFamilyIndice;
  uint32_t computeFamilyQueueCount;

  VkBool32 isComplete();
};
struct TransferQueueFamilyIndices {
  std::optional<uint32_t> transferFamilyIndice;
  uint32_t transferFamilyQueueCount;

  VkBool32 isComplete();
};
struct ProtectedQueueFamilyIndices {
  std::optional<uint32_t> protectedFamilyIndice;
  uint32_t protectedFamilyQueueCount;

  VkBool32 isComplete();
};

static GraphicsPresentQueueFramilyIndices
FindGraphicsPresentQueueFamilyIndices(VkPhysicalDevice device,
                                      VkSurfaceKHR *surface);

static ComputeQueueFamilyIndices
FindComputeQueueFamilyIndices(VkPhysicalDevice device);

static TransferQueueFamilyIndices
FindTransferQueueFamilyIndices(VkPhysicalDevice device);

static ProtectedQueueFamilyIndices
FindProtectedQueueFamilyIndices(VkPhysicalDevice device);
} // namespace Vulkan
} // namespace BladeEngine