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
CMAKE_SOURCE_DIR = /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build

# Utility rule file for ComponentWebots_distclean.

# Include the progress variables for this target.
include CMakeFiles/ComponentWebots_distclean.dir/progress.make

CMakeFiles/ComponentWebots_distclean: cmake_distclean_all.cmake
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Remove ALL generated files from make AND CMake"
	/usr/bin/make clean
	/usr/bin/cmake -P /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_distclean_all.cmake
	/usr/bin/cmake -E remove /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/cmake_distclean_all.cmake

ComponentWebots_distclean: CMakeFiles/ComponentWebots_distclean
ComponentWebots_distclean: CMakeFiles/ComponentWebots_distclean.dir/build.make

.PHONY : ComponentWebots_distclean

# Rule to build all files generated by this target.
CMakeFiles/ComponentWebots_distclean.dir/build: ComponentWebots_distclean

.PHONY : CMakeFiles/ComponentWebots_distclean.dir/build

CMakeFiles/ComponentWebots_distclean.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ComponentWebots_distclean.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ComponentWebots_distclean.dir/clean

CMakeFiles/ComponentWebots_distclean.dir/depend:
	cd /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build /home/smartsoft/github/RobMoSys/ComponentWebots/smartsoft/build/CMakeFiles/ComponentWebots_distclean.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ComponentWebots_distclean.dir/depend

