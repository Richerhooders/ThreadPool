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
include CMakeFiles/testthreadfinal.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/testthreadfinal.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/testthreadfinal.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/testthreadfinal.dir/flags.make

CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.o: CMakeFiles/testthreadfinal.dir/flags.make
CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.o: /home/rodgergraser7u/projects/ThreadPool/test/testthreadfinal.cc
CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.o: CMakeFiles/testthreadfinal.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/rodgergraser7u/projects/ThreadPool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.o -MF CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.o.d -o CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.o -c /home/rodgergraser7u/projects/ThreadPool/test/testthreadfinal.cc

CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rodgergraser7u/projects/ThreadPool/test/testthreadfinal.cc > CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.i

CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rodgergraser7u/projects/ThreadPool/test/testthreadfinal.cc -o CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.s

# Object files for target testthreadfinal
testthreadfinal_OBJECTS = \
"CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.o"

# External object files for target testthreadfinal
testthreadfinal_EXTERNAL_OBJECTS =

/home/rodgergraser7u/projects/ThreadPool/bin/testthreadfinal: CMakeFiles/testthreadfinal.dir/test/testthreadfinal.cc.o
/home/rodgergraser7u/projects/ThreadPool/bin/testthreadfinal: CMakeFiles/testthreadfinal.dir/build.make
/home/rodgergraser7u/projects/ThreadPool/bin/testthreadfinal: /home/rodgergraser7u/projects/ThreadPool/lib/libThreadPool.so
/home/rodgergraser7u/projects/ThreadPool/bin/testthreadfinal: CMakeFiles/testthreadfinal.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/rodgergraser7u/projects/ThreadPool/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable /home/rodgergraser7u/projects/ThreadPool/bin/testthreadfinal"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/testthreadfinal.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/testthreadfinal.dir/build: /home/rodgergraser7u/projects/ThreadPool/bin/testthreadfinal
.PHONY : CMakeFiles/testthreadfinal.dir/build

CMakeFiles/testthreadfinal.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/testthreadfinal.dir/cmake_clean.cmake
.PHONY : CMakeFiles/testthreadfinal.dir/clean

CMakeFiles/testthreadfinal.dir/depend:
	cd /home/rodgergraser7u/projects/ThreadPool/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rodgergraser7u/projects/ThreadPool /home/rodgergraser7u/projects/ThreadPool /home/rodgergraser7u/projects/ThreadPool/build /home/rodgergraser7u/projects/ThreadPool/build /home/rodgergraser7u/projects/ThreadPool/build/CMakeFiles/testthreadfinal.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/testthreadfinal.dir/depend

