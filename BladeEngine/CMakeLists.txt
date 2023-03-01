add_library(BladeEngine STATIC 
    src/Core/EntryPoint.cpp 
    src/BladeEngine.hpp
    src/Core/Game.hpp src/Core/Game.cpp 
    src/Core/World.hpp src/Core/World.cpp
    src/Core/Window.hpp src/Core/Window.cpp
    src/Core/Log.hpp src/Core/Log.cpp
    src/Core/Input.hpp src/Core/Input.cpp
    src/Core/KeyCodes.hpp
    src/Renderer/Renderer2D.hpp src/Renderer/Renderer2D.cpp
    src/Renderer/Texture.hpp
    src/Renderer/Camera.hpp src/Renderer/Camera.cpp
    src/Renderer/GraphicsAPI.hpp
    )


if(WIN32)
    target_compile_definitions(BladeEngine PRIVATE BLADE_VULKAN_API)
    set(USING_VULKAN_API "True")
endif(WIN32)

if(UNIX)
    target_compile_definitions(BladeEngine PRIVATE BLADE_VULKAN_API)
    set(USING_VULKAN_API "True")
endif(UNIX)

if(USING_VULKAN_API)
    target_sources(BladeEngine PRIVATE
        src/Platform/Vulkan/VulkanDevice.hpp src/Platform/Vulkan/VulkanDevice.cpp
        src/Platform/Vulkan/VulkanTexture.cpp
        src/Platform/Vulkan/VulkanSwapchain.hpp src/Platform/Vulkan/VulkanSwapchain.cpp
        src/Platform/Vulkan/VulkanImage.hpp src/Platform/Vulkan/VulkanImage.cpp
        src/Platform/Vulkan/VulkanQueue.hpp src/Platform/Vulkan/VulkanQueue.cpp
        src/Platform/Vulkan/VulkanRenderPass.hpp src/Platform/Vulkan/VulkanRenderPass.cpp
        #src/Platform/Vulkan/VulkanPipeline.hpp src/Platform/Vulkan/VulkanPipeline.cpp
        src/Platform/Vulkan/VulkanGraphicsAPI.cpp
        src/Platform/Vulkan/VulkanValidationLayers.hpp src/Platform/Vulkan/VulkanValidationLayers.cpp
        )
endif(USING_VULKAN_API)

target_link_libraries(BladeEngine PRIVATE glfw)

target_include_directories(BladeEngine PRIVATE
    "vendor/GLFW/include" 
    "vendor/spdlog/include" 
    "vendor/glm")

if(DEFINED VULKAN_SDK_PATH)
    set(Vulkan_INCLUDE_DIRS "${VULKAN_SDK_PATH}/Include")
    set(Vulkan_LIBRARIES "${VULKAN_SDK_PATH}/Lib")
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED)
    message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")
endif()

if(NOT Vulkan_FOUND)
    message(FATAL_ERROR "Could not find Vulkan library!")
else()
    message(STATUS "Using vulkan lib at: ${Vulkan_LIBRARIES}")
    target_include_directories(BladeEngine PRIVATE ${Vulkan_INCLUDE_DIRS})
    target_link_libraries(BladeEngine PRIVATE ${Vulkan_LIBRARIES})
endif()

set_target_properties(BladeEngine PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(BladeEngine PRIVATE 
    $<$<CONFIG:Debug>:BLADE_DEBUG>
    $<$<CONFIG:Release>:BLADE_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:BLADE_DEBUG>
    $<$<CONFIG:MinSizeRel>:BLADE_RELEASE>)
