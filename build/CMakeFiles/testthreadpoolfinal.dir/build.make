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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rodgergraser7u/projects/ThreadPool

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rodgergraser7u/projects/ThreadPool/build

# Include any dependencies generated for this target.
include CMakeFiles/testthreadpoolfinal.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/testthreadpoolfinal.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/testthreadpoolfinal.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testthreadpoolfinal.dir/flags.make

CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.o: CMakeFiles/testthreadpoolfinal.dir/flags.make
CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.o: /home/rodgergraser7u/projects/ThreadPool/test/testthreadfinal.cc
CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.o: CMakeFiles/testthreadpoolfinal.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/rodgergraser7u/projects/ThreadPool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.o -MF CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.o.d -o CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.o -c /home/rodgergraser7u/projects/ThreadPool/test/testthreadfinal.cc

CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rodgergraser7u/projects/ThreadPool/test/testthreadfinal.cc > CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.i

CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rodgergraser7u/projects/ThreadPool/test/testthreadfinal.cc -o CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.s

# Object files for target testthreadpoolfinal
testthreadpoolfinal_OBJECTS = \
"CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.o"

# External object files for target testthreadpoolfinal
testthreadpoolfinal_EXTERNAL_OBJECTS =

/home/rodgergraser7u/projects/ThreadPool/bin/testthreadpoolfinal: CMakeFiles/testthreadpoolfinal.dir/test/testthreadfinal.cc.o
/home/rodgergraser7u/projects/ThreadPool/bin/testthreadpoolfinal: CMakeFiles/testthreadpoolfinal.dir/build.make
/home/rodgergraser7u/projects/ThreadPool/bin/testthreadpoolfinal: /home/rodgergraser7u/projects/ThreadPool/lib/libThreadPool.so
/home/rodgergraser7u/projects/ThreadPool/bin/testthreadpoolfinal: CMakeFiles/testthreadpoolfinal.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/rodgergraser7u/projects/ThreadPool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/rodgergraser7u/projects/ThreadPool/bin/testthreadpoolfinal"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testthreadpoolfinal.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/testthreadpoolfinal.dir/build: /home/rodgergraser7u/projects/ThreadPool/bin/testthreadpoolfinal
.PHONY : CMakeFiles/testthreadpoolfinal.dir/build

CMakeFiles/testthreadpoolfinal.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testthreadpoolfinal.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testthreadpoolfinal.dir/clean

CMakeFiles/testthreadpoolfinal.dir/depend:
	cd /home/rodgergraser7u/projects/ThreadPool/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rodgergraser7u/projects/ThreadPool /home/rodgergraser7u/projects/ThreadPool /home/rodgergraser7u/projects/ThreadPool/build /home/rodgergraser7u/projects/ThreadPool/build /home/rodgergraser7u/projects/ThreadPool/build/CMakeFiles/testthreadpoolfinal.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/testthreadpoolfinal.dir/depend
