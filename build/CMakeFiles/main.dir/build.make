# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.10.0/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.10.0/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/joaomateusdefreitasveneroso/Code/software_renderer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/joaomateusdefreitasveneroso/Code/software_renderer/build

# Include any dependencies generated for this target.
include CMakeFiles/main.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/main.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/main.dir/flags.make

CMakeFiles/main.dir/src/main.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/main.cpp.o: ../src/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/main.dir/src/main.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/main.cpp.o -c /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/main.cpp

CMakeFiles/main.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/main.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/main.cpp > CMakeFiles/main.dir/src/main.cpp.i

CMakeFiles/main.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/main.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/main.cpp -o CMakeFiles/main.dir/src/main.cpp.s

CMakeFiles/main.dir/src/main.cpp.o.requires:

.PHONY : CMakeFiles/main.dir/src/main.cpp.o.requires

CMakeFiles/main.dir/src/main.cpp.o.provides: CMakeFiles/main.dir/src/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/main.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/src/main.cpp.o.provides

CMakeFiles/main.dir/src/main.cpp.o.provides.build: CMakeFiles/main.dir/src/main.cpp.o


CMakeFiles/main.dir/src/engine.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/engine.cpp.o: ../src/engine.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/main.dir/src/engine.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/engine.cpp.o -c /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/engine.cpp

CMakeFiles/main.dir/src/engine.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/engine.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/engine.cpp > CMakeFiles/main.dir/src/engine.cpp.i

CMakeFiles/main.dir/src/engine.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/engine.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/engine.cpp -o CMakeFiles/main.dir/src/engine.cpp.s

CMakeFiles/main.dir/src/engine.cpp.o.requires:

.PHONY : CMakeFiles/main.dir/src/engine.cpp.o.requires

CMakeFiles/main.dir/src/engine.cpp.o.provides: CMakeFiles/main.dir/src/engine.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/engine.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/src/engine.cpp.o.provides

CMakeFiles/main.dir/src/engine.cpp.o.provides.build: CMakeFiles/main.dir/src/engine.cpp.o


CMakeFiles/main.dir/src/controls.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/controls.cpp.o: ../src/controls.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/main.dir/src/controls.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/controls.cpp.o -c /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/controls.cpp

CMakeFiles/main.dir/src/controls.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/controls.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/controls.cpp > CMakeFiles/main.dir/src/controls.cpp.i

CMakeFiles/main.dir/src/controls.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/controls.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/controls.cpp -o CMakeFiles/main.dir/src/controls.cpp.s

CMakeFiles/main.dir/src/controls.cpp.o.requires:

.PHONY : CMakeFiles/main.dir/src/controls.cpp.o.requires

CMakeFiles/main.dir/src/controls.cpp.o.provides: CMakeFiles/main.dir/src/controls.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/controls.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/src/controls.cpp.o.provides

CMakeFiles/main.dir/src/controls.cpp.o.provides.build: CMakeFiles/main.dir/src/controls.cpp.o


CMakeFiles/main.dir/src/vbo_indexer.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/vbo_indexer.cpp.o: ../src/vbo_indexer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/main.dir/src/vbo_indexer.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/vbo_indexer.cpp.o -c /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/vbo_indexer.cpp

CMakeFiles/main.dir/src/vbo_indexer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/vbo_indexer.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/vbo_indexer.cpp > CMakeFiles/main.dir/src/vbo_indexer.cpp.i

CMakeFiles/main.dir/src/vbo_indexer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/vbo_indexer.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/vbo_indexer.cpp -o CMakeFiles/main.dir/src/vbo_indexer.cpp.s

CMakeFiles/main.dir/src/vbo_indexer.cpp.o.requires:

.PHONY : CMakeFiles/main.dir/src/vbo_indexer.cpp.o.requires

CMakeFiles/main.dir/src/vbo_indexer.cpp.o.provides: CMakeFiles/main.dir/src/vbo_indexer.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/vbo_indexer.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/src/vbo_indexer.cpp.o.provides

CMakeFiles/main.dir/src/vbo_indexer.cpp.o.provides.build: CMakeFiles/main.dir/src/vbo_indexer.cpp.o


CMakeFiles/main.dir/src/bitmap.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/bitmap.cpp.o: ../src/bitmap.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/main.dir/src/bitmap.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/bitmap.cpp.o -c /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/bitmap.cpp

CMakeFiles/main.dir/src/bitmap.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/bitmap.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/bitmap.cpp > CMakeFiles/main.dir/src/bitmap.cpp.i

CMakeFiles/main.dir/src/bitmap.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/bitmap.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/bitmap.cpp -o CMakeFiles/main.dir/src/bitmap.cpp.s

CMakeFiles/main.dir/src/bitmap.cpp.o.requires:

.PHONY : CMakeFiles/main.dir/src/bitmap.cpp.o.requires

CMakeFiles/main.dir/src/bitmap.cpp.o.provides: CMakeFiles/main.dir/src/bitmap.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/bitmap.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/src/bitmap.cpp.o.provides

CMakeFiles/main.dir/src/bitmap.cpp.o.provides.build: CMakeFiles/main.dir/src/bitmap.cpp.o


CMakeFiles/main.dir/src/water_render_object.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/water_render_object.cpp.o: ../src/water_render_object.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/main.dir/src/water_render_object.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/water_render_object.cpp.o -c /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/water_render_object.cpp

CMakeFiles/main.dir/src/water_render_object.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/water_render_object.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/water_render_object.cpp > CMakeFiles/main.dir/src/water_render_object.cpp.i

CMakeFiles/main.dir/src/water_render_object.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/water_render_object.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/water_render_object.cpp -o CMakeFiles/main.dir/src/water_render_object.cpp.s

CMakeFiles/main.dir/src/water_render_object.cpp.o.requires:

.PHONY : CMakeFiles/main.dir/src/water_render_object.cpp.o.requires

CMakeFiles/main.dir/src/water_render_object.cpp.o.provides: CMakeFiles/main.dir/src/water_render_object.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/water_render_object.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/src/water_render_object.cpp.o.provides

CMakeFiles/main.dir/src/water_render_object.cpp.o.provides.build: CMakeFiles/main.dir/src/water_render_object.cpp.o


CMakeFiles/main.dir/src/objloader.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/objloader.cpp.o: ../src/objloader.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/main.dir/src/objloader.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/objloader.cpp.o -c /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/objloader.cpp

CMakeFiles/main.dir/src/objloader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/objloader.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/objloader.cpp > CMakeFiles/main.dir/src/objloader.cpp.i

CMakeFiles/main.dir/src/objloader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/objloader.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/objloader.cpp -o CMakeFiles/main.dir/src/objloader.cpp.s

CMakeFiles/main.dir/src/objloader.cpp.o.requires:

.PHONY : CMakeFiles/main.dir/src/objloader.cpp.o.requires

CMakeFiles/main.dir/src/objloader.cpp.o.provides: CMakeFiles/main.dir/src/objloader.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/objloader.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/src/objloader.cpp.o.provides

CMakeFiles/main.dir/src/objloader.cpp.o.provides.build: CMakeFiles/main.dir/src/objloader.cpp.o


CMakeFiles/main.dir/src/tangentspace.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/tangentspace.cpp.o: ../src/tangentspace.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/main.dir/src/tangentspace.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/tangentspace.cpp.o -c /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/tangentspace.cpp

CMakeFiles/main.dir/src/tangentspace.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/tangentspace.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/tangentspace.cpp > CMakeFiles/main.dir/src/tangentspace.cpp.i

CMakeFiles/main.dir/src/tangentspace.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/tangentspace.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/tangentspace.cpp -o CMakeFiles/main.dir/src/tangentspace.cpp.s

CMakeFiles/main.dir/src/tangentspace.cpp.o.requires:

.PHONY : CMakeFiles/main.dir/src/tangentspace.cpp.o.requires

CMakeFiles/main.dir/src/tangentspace.cpp.o.provides: CMakeFiles/main.dir/src/tangentspace.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/tangentspace.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/src/tangentspace.cpp.o.provides

CMakeFiles/main.dir/src/tangentspace.cpp.o.provides.build: CMakeFiles/main.dir/src/tangentspace.cpp.o


CMakeFiles/main.dir/src/shaders.cpp.o: CMakeFiles/main.dir/flags.make
CMakeFiles/main.dir/src/shaders.cpp.o: ../src/shaders.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/main.dir/src/shaders.cpp.o"
	/Library/Developer/CommandLineTools/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/main.dir/src/shaders.cpp.o -c /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/shaders.cpp

CMakeFiles/main.dir/src/shaders.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/main.dir/src/shaders.cpp.i"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/shaders.cpp > CMakeFiles/main.dir/src/shaders.cpp.i

CMakeFiles/main.dir/src/shaders.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/main.dir/src/shaders.cpp.s"
	/Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/joaomateusdefreitasveneroso/Code/software_renderer/src/shaders.cpp -o CMakeFiles/main.dir/src/shaders.cpp.s

CMakeFiles/main.dir/src/shaders.cpp.o.requires:

.PHONY : CMakeFiles/main.dir/src/shaders.cpp.o.requires

CMakeFiles/main.dir/src/shaders.cpp.o.provides: CMakeFiles/main.dir/src/shaders.cpp.o.requires
	$(MAKE) -f CMakeFiles/main.dir/build.make CMakeFiles/main.dir/src/shaders.cpp.o.provides.build
.PHONY : CMakeFiles/main.dir/src/shaders.cpp.o.provides

CMakeFiles/main.dir/src/shaders.cpp.o.provides.build: CMakeFiles/main.dir/src/shaders.cpp.o


# Object files for target main
main_OBJECTS = \
"CMakeFiles/main.dir/src/main.cpp.o" \
"CMakeFiles/main.dir/src/engine.cpp.o" \
"CMakeFiles/main.dir/src/controls.cpp.o" \
"CMakeFiles/main.dir/src/vbo_indexer.cpp.o" \
"CMakeFiles/main.dir/src/bitmap.cpp.o" \
"CMakeFiles/main.dir/src/water_render_object.cpp.o" \
"CMakeFiles/main.dir/src/objloader.cpp.o" \
"CMakeFiles/main.dir/src/tangentspace.cpp.o" \
"CMakeFiles/main.dir/src/shaders.cpp.o"

# External object files for target main
main_EXTERNAL_OBJECTS =

main: CMakeFiles/main.dir/src/main.cpp.o
main: CMakeFiles/main.dir/src/engine.cpp.o
main: CMakeFiles/main.dir/src/controls.cpp.o
main: CMakeFiles/main.dir/src/vbo_indexer.cpp.o
main: CMakeFiles/main.dir/src/bitmap.cpp.o
main: CMakeFiles/main.dir/src/water_render_object.cpp.o
main: CMakeFiles/main.dir/src/objloader.cpp.o
main: CMakeFiles/main.dir/src/tangentspace.cpp.o
main: CMakeFiles/main.dir/src/shaders.cpp.o
main: CMakeFiles/main.dir/build.make
main: CMakeFiles/main.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX executable main"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/main.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/main.dir/build: main

.PHONY : CMakeFiles/main.dir/build

CMakeFiles/main.dir/requires: CMakeFiles/main.dir/src/main.cpp.o.requires
CMakeFiles/main.dir/requires: CMakeFiles/main.dir/src/engine.cpp.o.requires
CMakeFiles/main.dir/requires: CMakeFiles/main.dir/src/controls.cpp.o.requires
CMakeFiles/main.dir/requires: CMakeFiles/main.dir/src/vbo_indexer.cpp.o.requires
CMakeFiles/main.dir/requires: CMakeFiles/main.dir/src/bitmap.cpp.o.requires
CMakeFiles/main.dir/requires: CMakeFiles/main.dir/src/water_render_object.cpp.o.requires
CMakeFiles/main.dir/requires: CMakeFiles/main.dir/src/objloader.cpp.o.requires
CMakeFiles/main.dir/requires: CMakeFiles/main.dir/src/tangentspace.cpp.o.requires
CMakeFiles/main.dir/requires: CMakeFiles/main.dir/src/shaders.cpp.o.requires

.PHONY : CMakeFiles/main.dir/requires

CMakeFiles/main.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/main.dir/cmake_clean.cmake
.PHONY : CMakeFiles/main.dir/clean

CMakeFiles/main.dir/depend:
	cd /Users/joaomateusdefreitasveneroso/Code/software_renderer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/joaomateusdefreitasveneroso/Code/software_renderer /Users/joaomateusdefreitasveneroso/Code/software_renderer /Users/joaomateusdefreitasveneroso/Code/software_renderer/build /Users/joaomateusdefreitasveneroso/Code/software_renderer/build /Users/joaomateusdefreitasveneroso/Code/software_renderer/build/CMakeFiles/main.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/main.dir/depend
