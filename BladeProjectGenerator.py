import os
import git
import shutil

def MoveShadersToProject(projectName):
    source = "temp/Sandbox/assets/shaders"
    destination = os.path.join(projectName,"assets")
    shutil.move(source,destination)

def MoveDebugUVTexture(projectName):
    source = "temp/Sandbox/assets/sprites/tex_DebugUVTiles.png"
    destination = os.path.join(projectName,"assets/sprites")
    shutil.move(source,destination)

def MoveBladeEngineToProject(projectName):
    source = "temp/BladeEngine"
    destination = projectName
    shutil.move(source,destination)

def GenerateBaseDirectories(projectName):
    src = os.path.join(projectName,"src")
    components = os.path.join(projectName,"src/components")
    systems = os.path.join(projectName,"src/systems")

    assets = os.path.join(projectName,"assets")
    sprites = os.path.join(projectName,"assets/sprites")
    audios = os.path.join(projectName,"assets/audio")
    fonts = os.path.join(projectName,"assets/fonts")
#    shaders = os.path.join(projectName,"assets/shaders")

    os.mkdir(src)
    os.mkdir(components)
    os.mkdir(systems)

    os.mkdir(assets)
    os.mkdir(sprites)
    os.mkdir(audios)
    os.mkdir(fonts)
#   os.mkdir(shaders)

def GenerateBaseFiles(projectName):
    cmakePath = projectName

    cppPath = os.path.join(projectName,"src")
    hppPath = os.path.join(projectName,"src")

    open(projectName+"/CMakeLists.txt","x")
    open(cppPath+"/"+projectName+".cpp","x")
    open(hppPath+"/"+projectName+".hpp","x")

def LoadEmptyBaseFiles(projectName):
    cmake = (
"cmake_minimum_required(VERSION 3.12)\n"
f"project({projectName})\n"
"""
set(CMAKE_CXX_STANDARD 17)

if(MSVC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")
endif(MSVC)

add_subdirectory("BladeEngine/vendor/GLFW")
add_subdirectory("BladeEngine/vendor/Box2D")
add_subdirectory("BladeEngine/vendor/msdf-atlas-gen")
add_subdirectory("BladeEngine")
"""
f"add_executable({projectName} src/{projectName}.cpp src/{projectName}.hpp)"
"""
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE BladeEngine)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/src")
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/flecs") #TEMP
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/miniaudio") #TEMP
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/glm") #TEMP
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/BladeEngine/vendor/spdlog/include")

set(ASSETS_SPRITES_DIR assets/sprites)
file(COPY ${ASSETS_SPRITES_DIR}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

set(ASSETS_AUDIOS_DIR assets/audio)
file(COPY ${ASSETS_AUDIOS_DIR}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

set(ASSETS_FONTS_DIR assets/fonts)
file(COPY ${ASSETS_FONTS_DIR}
    DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/assets)

set(ASSETS_SHADERS_DIR assets/shaders)

file(GLOB files ${ASSETS_SHADERS_DIR}/*)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders)

foreach(file ${files})
	get_filename_component(FILENAME ${file} NAME)
	execute_process(
	    COMMAND glslangValidator -V ${file} -o ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${FILENAME}.spv
	)
endforeach()

target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    $<$<CONFIG:DEBUG>:BLADE_DEBUG>
    $<$<CONFIG:RELEASE>:BLADE_RELEASE>
    $<$<CONFIG:RELWITHDEBINFO>:BLADE_DEBUG>
    $<$<CONFIG:MINSIZEREL>:BLADE_RELEASE>)""")

    cmakeFile = open(projectName+"/CMakeLists.txt","a")
    cmakeFile.write(cmake)
    cmakeFile.close()

    hpp = (
    """#pragma once

#include "Core/Game.hpp"

namespace BladeEngine
{"""
    f"class {projectName} : public Game"
    """{
    private:
        virtual void LoadGameResources() override;
        virtual void UnloadGameResources() override;

        virtual void SetupWorld() override;
    };
}""")

    hppFile = open(projectName+"/src/"+f"{projectName}.hpp","a")
    hppFile.write(hpp)
    hppFile.close()

    cpp = f'#include "{projectName}.hpp"\n'\
        """#include "Audio/AudioClip.hpp"
#include "Audio/AudioManager.hpp"
#include "Audio/AudioSource.hpp"
#include "BladeEngine.hpp"
#include "Utils/Random.hpp"
#include <sstream>

namespace BladeEngine {\n"""\
f"void {projectName}::LoadGameResources()"\
"""{}\n"""\
f"void {projectName}::UnloadGameResources()"\
"""{}\n"""\
f"void {projectName}::SetupWorld()"\
"""{}\n"""\
"Game* CreateGameInstance() { return new "\
f"{projectName}();"\
"}\n"\
"}"

    cppFile = open(projectName+"/src/"+f"{projectName}.cpp","a")
    cppFile.write(cpp)
    cppFile.close()

title = """\
______  _             _        ______             _              _
| ___ \| |           | |       | ___ \           (_)            | |
| |_/ /| |  __ _   __| |  ___  | |_/ /_ __  ___   _   ___   ___ | |_
| ___ \| | / _` | / _` | / _ \ |  __/| '__|/ _ \ | | / _ \ / __|| __|
| |_/ /| || (_| || (_| ||  __/ | |   | |  | (_) || ||  __/| (__ | |_
\____/ |_| \__,_| \__,_| \___| \_|   |_|   \___/ | | \___| \___| \__|
|  __ \                               | |       _/ |
| |  \/  ___  _ __    ___  _ __  __ _ | |_  ___|__/ __
| | __  / _ \| '_ \  / _ \| '__|/ _` || __|/ _ \ | '__|
| |_\ \|  __/| | | ||  __/| |  | (_| || |_| (_) || |
 \____/ \___||_| |_| \___||_|   \__,_| \__|\___/ |_|"""

menu = """\

                 - MENU ----------------
                | (1) - Empty Project   |
                | (0) - QUIT            |
                 -----------------------

        """
print(title)
print(menu)

projectType = input("Select a template or type 0 to quit: ")

if projectType != '0':

    projectName = input("Project Name: ")
    os.mkdir(projectName)

    if projectType == '1':
        git.Repo.clone_from("https://github.com/GigalightGuy/Blade.git","temp",recursive=True)
        GenerateBaseDirectories(projectName)
        GenerateBaseFiles(projectName)
        LoadEmptyBaseFiles(projectName)
        MoveShadersToProject(projectName)
        MoveBladeEngineToProject(projectName)
        MoveDebugUVTexture(projectName)
        shutil.rmtree("temp")
        print("Empty Project Generation\n")
else:
    print("Quit Project Generator")
