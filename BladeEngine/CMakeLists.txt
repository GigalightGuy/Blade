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
    src/Renderer/Swapchain.hpp
    src/Renderer/Camera.cpp
    src/Platform/Vulkan/Init.cpp
    src/Platform/Vulkan/QueueFamily.cpp
    src/Platform/Vulkan/Swapchain.cpp
    )

if(WIN32)
    target_sources(BladeEngine PRIVATE
        #src/Platform/Vulkan/VulkanTexture.cpp
        src/Platform/Vulkan/Swapchain.cpp
        )
endif(WIN32)

if(UNIX)
    target_sources(BladeEngine PRIVATE
        #src/Platform/Vulkan/VulkanTexture.cpp
        src/Platform/Vulkan/Swapchain.cpp
        )
endif(UNIX)

target_link_libraries(BladeEngine PRIVATE 
    glfw)

target_include_directories(BladeEngine PRIVATE
    "vendor/GLFW/include" 
    "vendor/spdlog/include" 
    "vendor/glm")

# temp REQUIRED VULKAN IN SYSTEM INCLUDE LIBRARY 
find_package(Vulkan REQUIRED)
target_include_directories(BladeEngine PRIVATE ${Vulkan_INCLUDE_DIRS})
target_link_libraries(BladeEngine PRIVATE ${Vulkan_LIBRARIES})

set_target_properties(BladeEngine PROPERTIES LINKER_LANGUAGE CXX)

target_compile_definitions(BladeEngine PRIVATE 
    $<$<CONFIG:Debug>:BLADE_DEBUG>
    $<$<CONFIG:Release>:BLADE_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:BLADE_DEBUG>
    $<$<CONFIG:MinSizeRel>:BLADE_RELEASE>)
