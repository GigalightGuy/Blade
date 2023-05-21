set(BLADE_ENGINE_SOURCE_FILES

    src/Core/EntryPoint.cpp
    
    src/Core/Game.cpp
    src/Core/Window.cpp
    src/Core/Log.cpp
    src/Core/Time.cpp
    src/Core/Input.cpp

    src/Audio/AudioClip.cpp
    src/Audio/AudioManager.cpp
    src/Audio/AudioSource.cpp

    src/Utils/Random.cpp

    src/Core/Camera.cpp
    src/Core/Utils.cpp

    src/Graphics/Color.cpp
    src/Graphics/GraphicsManager.cpp
    src/Graphics/Mesh.cpp
    src/Graphics/Shader.cpp
    src/Graphics/Texture2D.cpp
    src/Graphics/Vertex.cpp
    src/Graphics/Font.cpp

    src/ECS/World.cpp
    src/ECS/Entity.cpp

    src/Physics/Physics2D.cpp

)

set(BLADE_ENGINE_VENDOR_SOURCE_FILES

    vendor/flecs/flecs.c
)

set(BLADE_ENGINE_HEADER_FILES

    src/BladeEngine.hpp
    src/Core/Base.hpp
    src/Core/Platform.hpp
    
    src/Core/Game.hpp
    src/Core/Window.hpp
    src/Core/Log.hpp
    src/Core/Time.hpp
    src/Core/Input.hpp

    src/Core/KeyCodes.hpp
    src/Core/Math.hpp
    src/Core/Vec.hpp
    src/Core/Buffer.hpp

    src/Audio/AudioClip.hpp
    src/Audio/AudioManager.hpp
    src/Audio/AudioSource.hpp
    src/Audio/BladeAudio.hpp

    src/Utils/Random.hpp

    src/Core/Camera.hpp
    src/Core/Utils.hpp

    src/Graphics/Color.hpp
    src/Graphics/GraphicsManager.hpp
    src/Graphics/Mesh.hpp
    src/Graphics/Shader.hpp
    src/Graphics/Texture2D.hpp
    src/Graphics/Vertex.hpp
    src/Graphics/Font.hpp
    src/Graphics/MSDFData.hpp

    src/ECS/World.hpp
    src/ECS/Entity.hpp

    src/Components/Components.hpp

    src/Physics/Physics2D.hpp

)

set(BLADE_ENGINE_VENDOR_HEADER_FILES
    
    vendor/flecs/flecs.h

    vendor/miniaudio/miniaudio.h

    vendor/vma/vk_mem_alloc.h
)

if(WIN32)
    set(USING_VULKAN_API "True")
endif(WIN32)

if(UNIX)
    set(USING_VULKAN_API "True")
endif(UNIX)

if(USING_VULKAN_API)
    set(BLADE_ENGINE_GRAPHICS_API_SOURCE_FILES

        src/Graphics/Platform/Vulkan/BladeVulkanUtils.cpp
		src/Graphics/Platform/Vulkan/BladeVulkanInstance.cpp
		src/Graphics/Platform/Vulkan/BladeVulkanQueueFamily.cpp
		src/Graphics/Platform/Vulkan/BladeVulkanDevice.cpp
		src/Graphics/Platform/Vulkan/BladeVulkanSwapchain.cpp
		src/Graphics/Platform/Vulkan/BladeVulkanShader.cpp
        src/Graphics/Platform/Vulkan/BladeVulkanGraphicsPipeline.cpp
        src/Graphics/Platform/Vulkan/BladeVulkanMesh.cpp
        src/Graphics/Platform/Vulkan/BladeVulkanTexture.cpp
		src/Graphics/Platform/Vulkan/BladeVulkanRenderer.cpp
        src/Graphics/Platform/Vulkan/VulkanResourceAllocator.cpp
        src/Graphics/Platform/Vulkan/VulkanBuffer.cpp

        src/Graphics/Platform/Vulkan/VmaImpl.cpp
        
    )

    set(BLADE_ENGINE_GRAPHICS_API_HEADER_FILES

        src/Graphics/Platform/Vulkan/BladeVulkanUtils.hpp
		src/Graphics/Platform/Vulkan/BladeVulkanInstance.hpp
		src/Graphics/Platform/Vulkan/BladeVulkanQueueFamily.hpp	
		src/Graphics/Platform/Vulkan/BladeVulkanDevice.hpp
		src/Graphics/Platform/Vulkan/BladeVulkanSwapchain.hpp
		src/Graphics/Platform/Vulkan/BladeVulkanShader.hpp
        src/Graphics/Platform/Vulkan/BladeVulkanGraphicsPipeline.hpp
        src/Graphics/Platform/Vulkan/BladeVulkanMesh.hpp
        src/Graphics/Platform/Vulkan/BladeVulkanTexture.hpp
		src/Graphics/Platform/Vulkan/BladeVulkanRenderer.hpp
        src/Graphics/Platform/Vulkan/VulkanResourceAllocator.hpp
        src/Graphics/Platform/Vulkan/VulkanBuffer.hpp

        src/Graphics/Platform/Vulkan/VulkanCheck.hpp

    )
endif(USING_VULKAN_API)

add_library(BladeEngine STATIC 
    
    ${BLADE_ENGINE_SOURCE_FILES} ${BLADE_ENGINE_HEADER_FILES}
    ${BLADE_ENGINE_VENDOR_SOURCE_FILES} ${BLADE_ENGINE_VENDOR_HEADER_FILES}
    ${BLADE_ENGINE_GRAPHICS_API_SOURCE_FILES} ${BLADE_ENGINE_GRAPHICS_API_HEADER_FILES}
)


source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Source Files" FILES 
    ${BLADE_ENGINE_SOURCE_FILES} 
    ${BLADE_ENGINE_VENDOR_SOURCE_FILES}
    ${BLADE_ENGINE_GRAPHICS_API_SOURCE_FILES}
)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "Header Files" FILES 
    ${BLADE_ENGINE_HEADER_FILES}
    ${BLADE_ENGINE_VENDOR_HEADER_FILES}
    ${BLADE_ENGINE_GRAPHICS_API_HEADER_FILES}
)

if(USING_VULKAN_API)
    target_compile_definitions(BladeEngine PRIVATE BLADE_VULKAN_API)
endif(USING_VULKAN_API)

target_link_libraries(BladeEngine PRIVATE 
    glfw 
    box2d
    msdf-atlas-gen
)

target_include_directories(BladeEngine PRIVATE
    "vendor/flecs"
    "vendor/GLFW/include" 
    "vendor/spdlog/include" 
    "vendor/glm"
    "vendor/Box2D/include"
    "vendor/stb"
    "vendor/miniaudio"
    "vendor/msdf-atlas-gen/msdf-atlas-gen"
    "vendor/msdf-atlas-gen/msdfgen"
    "vendor/vma"
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
)
