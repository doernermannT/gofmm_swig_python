# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1/build

# Utility rule file for coverage-html.

# Include the progress variables for this target.
include CMakeFiles/coverage-html.dir/progress.make

CMakeFiles/coverage-html:
	gcovr -r /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1 -e deps -e example -e test --html -o /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1/doc/html/coverage.html
	gcovr -r /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1 -e deps -e example -e test --html --html-details -o /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1/doc/html/coverage_details.html

coverage-html: CMakeFiles/coverage-html
coverage-html: CMakeFiles/coverage-html.dir/build.make

.PHONY : coverage-html

# Rule to build all files generated by this target.
CMakeFiles/coverage-html.dir/build: coverage-html

.PHONY : CMakeFiles/coverage-html.dir/build

CMakeFiles/coverage-html.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/coverage-html.dir/cmake_clean.cmake
.PHONY : CMakeFiles/coverage-html.dir/clean

CMakeFiles/coverage-html.dir/depend:
	cd /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1 /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1 /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1/build /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1/build /home/getianyi/Documents/Master/Fall_2020/MasterThesis/hmlp/hmlp-1/build/CMakeFiles/coverage-html.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/coverage-html.dir/depend
