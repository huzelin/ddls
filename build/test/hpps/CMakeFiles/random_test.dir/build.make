# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_COMMAND = /usr/local/cmake/bin/cmake

# The command to remove a file.
RM = /usr/local/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/app/HPPS/hpps

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/app/HPPS/hpps/build

# Include any dependencies generated for this target.
include test/hpps/CMakeFiles/random_test.dir/depend.make

# Include the progress variables for this target.
include test/hpps/CMakeFiles/random_test.dir/progress.make

# Include the compile flags for this target's objects.
include test/hpps/CMakeFiles/random_test.dir/flags.make

test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o: test/hpps/CMakeFiles/random_test.dir/flags.make
test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o: ../test/hpps/common/random_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/app/HPPS/hpps/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o"
	cd /home/app/HPPS/hpps/build/test/hpps && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/random_test.dir/common/random_test.cc.o -c /home/app/HPPS/hpps/test/hpps/common/random_test.cc

test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/random_test.dir/common/random_test.cc.i"
	cd /home/app/HPPS/hpps/build/test/hpps && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/app/HPPS/hpps/test/hpps/common/random_test.cc > CMakeFiles/random_test.dir/common/random_test.cc.i

test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/random_test.dir/common/random_test.cc.s"
	cd /home/app/HPPS/hpps/build/test/hpps && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/app/HPPS/hpps/test/hpps/common/random_test.cc -o CMakeFiles/random_test.dir/common/random_test.cc.s

test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o.requires:

.PHONY : test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o.requires

test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o.provides: test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o.requires
	$(MAKE) -f test/hpps/CMakeFiles/random_test.dir/build.make test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o.provides.build
.PHONY : test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o.provides

test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o.provides.build: test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o


# Object files for target random_test
random_test_OBJECTS = \
"CMakeFiles/random_test.dir/common/random_test.cc.o"

# External object files for target random_test
random_test_EXTERNAL_OBJECTS =

test/hpps/random_test: test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o
test/hpps/random_test: test/hpps/CMakeFiles/random_test.dir/build.make
test/hpps/random_test: libhpps.so
test/hpps/random_test: thirdparty/gtest/libgtest.so
test/hpps/random_test: ../thirdparty/zeromq/src/.libs/libzmq.so
test/hpps/random_test: ../thirdparty/ompi/ompi/.libs/libmpi.so
test/hpps/random_test: test/hpps/CMakeFiles/random_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/app/HPPS/hpps/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable random_test"
	cd /home/app/HPPS/hpps/build/test/hpps && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/random_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/hpps/CMakeFiles/random_test.dir/build: test/hpps/random_test

.PHONY : test/hpps/CMakeFiles/random_test.dir/build

test/hpps/CMakeFiles/random_test.dir/requires: test/hpps/CMakeFiles/random_test.dir/common/random_test.cc.o.requires

.PHONY : test/hpps/CMakeFiles/random_test.dir/requires

test/hpps/CMakeFiles/random_test.dir/clean:
	cd /home/app/HPPS/hpps/build/test/hpps && $(CMAKE_COMMAND) -P CMakeFiles/random_test.dir/cmake_clean.cmake
.PHONY : test/hpps/CMakeFiles/random_test.dir/clean

test/hpps/CMakeFiles/random_test.dir/depend:
	cd /home/app/HPPS/hpps/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/app/HPPS/hpps /home/app/HPPS/hpps/test/hpps /home/app/HPPS/hpps/build /home/app/HPPS/hpps/build/test/hpps /home/app/HPPS/hpps/build/test/hpps/CMakeFiles/random_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/hpps/CMakeFiles/random_test.dir/depend

