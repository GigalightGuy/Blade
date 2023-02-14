add_executable(Sandbox src/TestGame.cpp src/TestGame.hpp)

target_link_libraries(Sandbox PRIVATE BladeEngine)

target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/src")
#target_include_directories(Sandbox PRIVATE "BladeEngine/vendor/spdlog/include")