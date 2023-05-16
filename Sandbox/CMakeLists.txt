add_executable(Sandbox src/TestGame.cpp src/TestGame.hpp)

target_link_libraries(Sandbox PRIVATE BladeEngine)

target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/src")
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/flecs") #TEMP
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/miniaudio") #TEMP
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/glm") #TEMP
target_include_directories(Sandbox PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/spdlog/include")

set(ASSETS_SPRITES_DIR assets/sprites)
file(COPY ${ASSETS_SPRITES_DIR}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

set(ASSETS_AUDIOS_DIR assets/audio)
file(COPY ${ASSETS_AUDIOS_DIR}
        DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

		#set(VALIDATOR ${CMAKE_CURRENT_BINARY_DIR}/lib/BladeGDK/Lib/glslang/StandAlone/glslangValidator)
set(ASSETS_SHADERS_DIR assets/shaders)

file(GLOB files ${ASSETS_SHADERS_DIR}/*)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders)

# TODO: Find a way to compile glslangValidator before compiling shaders in CMAKE
foreach(file ${files})
		get_filename_component(FILENAME ${file} NAME)
		execute_process(
				COMMAND glslangValidator -V ${file} -o ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${FILENAME}.spv
		)
endforeach()

target_compile_definitions(Sandbox PRIVATE
    $<$<CONFIG:DEBUG>:BLADE_DEBUG>
    $<$<CONFIG:RELEASE>:BLADE_RELEASE>
    $<$<CONFIG:RELWITHDEBINFO>:BLADE_DEBUG>
    $<$<CONFIG:MINSIZEREL>:BLADE_RELEASE>)
