# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dev_rubs/projects/Blade

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dev_rubs/projects/Blade/build

# Utility rule file for docs.

# Include the progress variables for this target.
include BladeEngine/vendor/GLFW/docs/CMakeFiles/docs.dir/progress.make

BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/include/GLFW/glfw3.h
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/include/GLFW/glfw3native.h
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/main.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/news.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/quick.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/moving.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/compile.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/build.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/intro.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/context.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/monitor.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/window.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/input.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/vulkan.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/compat.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/internal.dox
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/DoxygenLayout.xml
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/header.html
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/footer.html
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/extra.css
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/spaces.svg
BladeEngine/vendor/GLFW/docs/html/index.html: ../BladeEngine/vendor/GLFW/docs/Doxyfile.in
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/dev_rubs/projects/Blade/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating HTML documentation"
	cd /home/dev_rubs/projects/Blade/build/BladeEngine/vendor/GLFW/docs && /usr/bin/doxygen

docs: BladeEngine/vendor/GLFW/docs/html/index.html
docs: BladeEngine/vendor/GLFW/docs/CMakeFiles/docs.dir/build.make

.PHONY : docs

# Rule to build all files generated by this target.
BladeEngine/vendor/GLFW/docs/CMakeFiles/docs.dir/build: docs

.PHONY : BladeEngine/vendor/GLFW/docs/CMakeFiles/docs.dir/build

BladeEngine/vendor/GLFW/docs/CMakeFiles/docs.dir/clean:
	cd /home/dev_rubs/projects/Blade/build/BladeEngine/vendor/GLFW/docs && $(CMAKE_COMMAND) -P CMakeFiles/docs.dir/cmake_clean.cmake
.PHONY : BladeEngine/vendor/GLFW/docs/CMakeFiles/docs.dir/clean

BladeEngine/vendor/GLFW/docs/CMakeFiles/docs.dir/depend:
	cd /home/dev_rubs/projects/Blade/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dev_rubs/projects/Blade /home/dev_rubs/projects/Blade/BladeEngine/vendor/GLFW/docs /home/dev_rubs/projects/Blade/build /home/dev_rubs/projects/Blade/build/BladeEngine/vendor/GLFW/docs /home/dev_rubs/projects/Blade/build/BladeEngine/vendor/GLFW/docs/CMakeFiles/docs.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : BladeEngine/vendor/GLFW/docs/CMakeFiles/docs.dir/depend
