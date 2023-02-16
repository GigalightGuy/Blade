add_library(BladeEngine STATIC 
    src/Core/EntryPoint.cpp 
    src/BladeEngine.hpp
    src/Core/Game.hpp src/Core/Game.cpp 
    src/Core/World.hpp src/Core/World.cpp
    src/Core/Window.hpp src/Core/Window.cpp
    src/Core/Log.hpp src/Core/Log.cpp
    src/Core/Input.hpp src/Core/Input.cpp
    src/Core/KeyCodes.hpp
    src/Renderer/Renderer2D.hpp
    src/Renderer/Texture.hpp
    src/Renderer/Swapchain.hpp)

if(WIN32)
    target_sources(BladeEngine PRIVATE
        src/Platform/Vulkan/VulkanRenderer2D.cpp
        src/Platform/Vulkan/VulkanTexture.cpp
        src/Platform/Vulkan/VulkanSwapchain.cpp)
endif(WIN32)

if(UNIX)
    target_sources(BladeEngine PRIVATE
        src/Platform/Vulkan/VulkanRenderer2D.cpp
        src/Platform/Vulkan/VulkanTexture.cpp
        src/Platform/Vulkan/VulkanSwapchain.cpp)
endif(UNIX)

target_link_libraries(BladeEngine PRIVATE 
    glfw 
    "$ENV{VULKAN_SDK}/Lib/vulkan-1.lib")

target_include_directories(BladeEngine PRIVATE 
    "$ENV{VULKAN_SDK}/Include"
    "vendor/GLFW/include" 
    "vendor/spdlog/include" 
    "vendor/glm")

set_target_properties(BladeEngine PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(BladeEngine PRIVATE 
    $<$<CONFIG:Debug>:BLADE_DEBUG>
    $<$<CONFIG:Release>:BLADE_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:BLADE_DEBUG>
    $<$<CONFIG:MinSizeRel>:BLADE_RELEASE>)