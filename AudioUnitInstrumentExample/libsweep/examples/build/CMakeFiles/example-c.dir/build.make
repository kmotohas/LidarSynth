# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.7

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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kmotohas/work/sweep-sdk/libsweep/examples

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kmotohas/work/sweep-sdk/libsweep/examples/build

# Include any dependencies generated for this target.
include CMakeFiles/example-c.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/example-c.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/example-c.dir/flags.make

CMakeFiles/example-c.dir/example.c.o: CMakeFiles/example-c.dir/flags.make
CMakeFiles/example-c.dir/example.c.o: ../example.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kmotohas/work/sweep-sdk/libsweep/examples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/example-c.dir/example.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/example-c.dir/example.c.o   -c /Users/kmotohas/work/sweep-sdk/libsweep/examples/example.c

CMakeFiles/example-c.dir/example.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/example-c.dir/example.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/kmotohas/work/sweep-sdk/libsweep/examples/example.c > CMakeFiles/example-c.dir/example.c.i

CMakeFiles/example-c.dir/example.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/example-c.dir/example.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc  $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/kmotohas/work/sweep-sdk/libsweep/examples/example.c -o CMakeFiles/example-c.dir/example.c.s

CMakeFiles/example-c.dir/example.c.o.requires:

.PHONY : CMakeFiles/example-c.dir/example.c.o.requires

CMakeFiles/example-c.dir/example.c.o.provides: CMakeFiles/example-c.dir/example.c.o.requires
	$(MAKE) -f CMakeFiles/example-c.dir/build.make CMakeFiles/example-c.dir/example.c.o.provides.build
.PHONY : CMakeFiles/example-c.dir/example.c.o.provides

CMakeFiles/example-c.dir/example.c.o.provides.build: CMakeFiles/example-c.dir/example.c.o


# Object files for target example-c
example__c_OBJECTS = \
"CMakeFiles/example-c.dir/example.c.o"

# External object files for target example-c
example__c_EXTERNAL_OBJECTS =

example-c: CMakeFiles/example-c.dir/example.c.o
example-c: CMakeFiles/example-c.dir/build.make
example-c: /usr/local/lib/libsweep.dylib
example-c: CMakeFiles/example-c.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/kmotohas/work/sweep-sdk/libsweep/examples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable example-c"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/example-c.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/example-c.dir/build: example-c

.PHONY : CMakeFiles/example-c.dir/build

CMakeFiles/example-c.dir/requires: CMakeFiles/example-c.dir/example.c.o.requires

.PHONY : CMakeFiles/example-c.dir/requires

CMakeFiles/example-c.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/example-c.dir/cmake_clean.cmake
.PHONY : CMakeFiles/example-c.dir/clean

CMakeFiles/example-c.dir/depend:
	cd /Users/kmotohas/work/sweep-sdk/libsweep/examples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kmotohas/work/sweep-sdk/libsweep/examples /Users/kmotohas/work/sweep-sdk/libsweep/examples /Users/kmotohas/work/sweep-sdk/libsweep/examples/build /Users/kmotohas/work/sweep-sdk/libsweep/examples/build /Users/kmotohas/work/sweep-sdk/libsweep/examples/build/CMakeFiles/example-c.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/example-c.dir/depend
