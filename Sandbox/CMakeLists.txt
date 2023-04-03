add_executable(Sandbox src/TestGame.cpp src/TestGame.hpp)

target_link_libraries(Sandbox PRIVATE BladeEngine)

target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/src")
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/flecs") #TEMP
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/spdlog/include")

target_compile_definitions(Sandbox PRIVATE
    $<$<CONFIG:DEBUG>:BLADE_DEBUG>
    $<$<CONFIG:RELEASE>:BLADE_RELEASE>
    $<$<CONFIG:RELWITHDEBINFO>:BLADE_DEBUG>
    $<$<CONFIG:MINSIZEREL>:BLADE_RELEASE>)