add_library(BladeEngine STATIC 
src/Core/EntryPoint.cpp 
src/BladeEngine.hpp
src/Core/Game.hpp src/Core/Game.cpp 
src/Core/World.hpp src/Core/World.cpp
src/Core/Window.hpp src/Core/Window.cpp
src/Core/Log.hpp src/Core/Log.cpp
src/Core/Input.hpp src/Core/Input.cpp
src/Core/KeyCodes.hpp)

target_link_libraries(BladeEngine PRIVATE glfw)

target_include_directories(BladeEngine PRIVATE "vendor/GLFW/include" "vendor/spdlog/include")

set_target_properties(BladeEngine PROPERTIES LINKER_LANGUAGE CXX)