cmake_minimum_required(VERSION 3.12.0)

project(Blade VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

if(MSVC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif(MSVC)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_subdirectory("BladeEngine/vendor/GLFW")
add_subdirectory("BladeEngine/vendor/Box2D")
add_subdirectory("BladeEngine")
add_subdirectory("Sandbox")

set_target_properties(glfw PROPERTIES FOLDER "third_party/GLFW")
set_target_properties(uninstall PROPERTIES FOLDER "third_party/GLFW")
set_target_properties(update_mappings PROPERTIES FOLDER "third_party/GLFW")

set_target_properties(box2d PROPERTIES FOLDER "third_party")

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Sandbox)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
