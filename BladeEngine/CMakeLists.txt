add_library(BladeEngine STATIC src/Core/EntryPoint.cpp src/Core/Game.hpp src/Core/Game.cpp)

target_link_libraries(BladeEngine PRIVATE glfw)

target_include_directories(BladeEngine PRIVATE "vendor/GLFW/include")

set_target_properties(BladeEngine PROPERTIES LINKER_LANGUAGE CXX)