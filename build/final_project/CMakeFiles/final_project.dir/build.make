# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.28.1/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.28.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build

# Include any dependencies generated for this target.
include final_project/CMakeFiles/final_project.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include final_project/CMakeFiles/final_project.dir/compiler_depend.make

# Include the progress variables for this target.
include final_project/CMakeFiles/final_project.dir/progress.make

# Include the compile flags for this target's objects.
include final_project/CMakeFiles/final_project.dir/flags.make

final_project/CMakeFiles/final_project.dir/final_project.cpp.o: final_project/CMakeFiles/final_project.dir/flags.make
final_project/CMakeFiles/final_project.dir/final_project.cpp.o: /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/final_project/final_project.cpp
final_project/CMakeFiles/final_project.dir/final_project.cpp.o: final_project/CMakeFiles/final_project.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object final_project/CMakeFiles/final_project.dir/final_project.cpp.o"
	cd /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build/final_project && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT final_project/CMakeFiles/final_project.dir/final_project.cpp.o -MF CMakeFiles/final_project.dir/final_project.cpp.o.d -o CMakeFiles/final_project.dir/final_project.cpp.o -c /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/final_project/final_project.cpp

final_project/CMakeFiles/final_project.dir/final_project.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/final_project.dir/final_project.cpp.i"
	cd /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build/final_project && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/final_project/final_project.cpp > CMakeFiles/final_project.dir/final_project.cpp.i

final_project/CMakeFiles/final_project.dir/final_project.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/final_project.dir/final_project.cpp.s"
	cd /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build/final_project && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/final_project/final_project.cpp -o CMakeFiles/final_project.dir/final_project.cpp.s

# Object files for target final_project
final_project_OBJECTS = \
"CMakeFiles/final_project.dir/final_project.cpp.o"

# External object files for target final_project
final_project_EXTERNAL_OBJECTS =

final_project/final_project: final_project/CMakeFiles/final_project.dir/final_project.cpp.o
final_project/final_project: final_project/CMakeFiles/final_project.dir/build.make
final_project/final_project: ingredients/libingredients.a
final_project/final_project: /opt/homebrew/lib/libglfw.3.3.dylib
final_project/final_project: final_project/CMakeFiles/final_project.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable final_project"
	cd /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build/final_project && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/final_project.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
final_project/CMakeFiles/final_project.dir/build: final_project/final_project
.PHONY : final_project/CMakeFiles/final_project.dir/build

final_project/CMakeFiles/final_project.dir/clean:
	cd /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build/final_project && $(CMAKE_COMMAND) -P CMakeFiles/final_project.dir/cmake_clean.cmake
.PHONY : final_project/CMakeFiles/final_project.dir/clean

final_project/CMakeFiles/final_project.dir/depend:
	cd /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/final_project /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build/final_project /Users/camdenlyles-smith/Desktop/School/CSCI544/final_project/build/final_project/CMakeFiles/final_project.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : final_project/CMakeFiles/final_project.dir/depend
