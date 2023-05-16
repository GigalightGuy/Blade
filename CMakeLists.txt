cmake_minimum_required(VERSION 3.12.0)

project(Blade VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif(MSVC)

add_subdirectory("BladeEngine/vendor/GLFW")
add_subdirectory("BladeEngine/vendor/Box2D")
add_subdirectory("BladeEngine")
add_subdirectory("Sandbox")
#add_subdirectory("Template")
#add_subdirectory("EmptyProject")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
