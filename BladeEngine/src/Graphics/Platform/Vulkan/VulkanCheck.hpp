#pragma once

#include "../../../Core/Base.hpp"

#include <vulkan/vulkan.hpp>

#define BLD_VK_CHECK(x, ...)								\
	do														\
	{														\
		VkResult err = x;									\
		BLD_CORE_ASSERT(err == VK_SUCCESS, __VA_ARGS__);	\
	} while (0)