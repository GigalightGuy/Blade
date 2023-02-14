cmake_minimum_required(VERSION 3.12.0)

project(Blade VERSION 0.1.0)

if(MSVC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif(MSVC)

add_subdirectory("BladeEngine/vendor/GLFW")
add_subdirectory("BladeEngine")
add_subdirectory("Sandbox")