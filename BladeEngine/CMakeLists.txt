add_library(BladeEngine STATIC 
    src/BladeEngine.hpp
    src/Core/Base.hpp
    src/Core/Platform.hpp
    src/Core/EntryPoint.cpp 
    src/Core/Game.hpp src/Core/Game.cpp
    src/Core/Window.hpp src/Core/Window.cpp
    src/Core/Log.hpp src/Core/Log.cpp
    src/Core/Time.hpp src/Core/Time.cpp
    src/Core/Input.hpp src/Core/Input.cpp
    src/Core/KeyCodes.hpp
    src/Core/Math.hpp
    src/Core/Vec.hpp
    src/Utils/Random.hpp src/Utils/Random.cpp
    src/Renderer/Renderer2D.hpp src/Renderer/Renderer2D.cpp
    src/Renderer/Texture.hpp
    src/Renderer/Camera.hpp src/Renderer/Camera.cpp
    src/Renderer/GraphicsAPI.hpp
    src/ECS/World.hpp src/ECS/World.cpp
    src/ECS/Entity.hpp src/ECS/Entity.cpp
    src/Components/Components.hpp
    vendor/flecs/flecs.h vendor/flecs/flecs.c
    src/Physics/Physics2D.hpp src/Physics/Physics2D.cpp
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
        #src/Platform/Vulkan/VulkanRenderPass.hpp src/Platform/Vulkan/VulkanRenderPass.cpp
        #src/Platform/Vulkan/VulkanPipeline.hpp src/Platform/Vulkan/VulkanPipeline.cpp
        #src/Platform/Vulkan/VulkanCommandPool.hpp src/Platform/Vulkan/VulkanCommandPool.cpp
        src/Platform/Vulkan/VulkanGraphicsAPI.cpp
        src/Platform/Vulkan/VulkanValidationLayers.hpp src/Platform/Vulkan/VulkanValidationLayers.cpp
        )
endif(USING_VULKAN_API)

target_link_libraries(BladeEngine PRIVATE 
    glfw 
    box2d
    )

target_include_directories(BladeEngine PRIVATE
    "vendor/flecs"
    "vendor/GLFW/include" 
    "vendor/spdlog/include" 
    "vendor/glm"
    "vendor/box2d/include"
    )

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
    $<$<CONFIG:DEBUG>:BLADE_DEBUG>
    $<$<CONFIG:RELEASE>:BLADE_RELEASE>
    $<$<CONFIG:RELWITHDEBINFO>:BLADE_DEBUG>
    $<$<CONFIG:MINSIZEREL>:BLADE_RELEASE>)
