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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.5.0/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.5.0/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test"

# Include any dependencies generated for this target.
include CMakeFiles/webcam.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/webcam.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/webcam.dir/flags.make

CMakeFiles/webcam.dir/webcam.cpp.o: CMakeFiles/webcam.dir/flags.make
CMakeFiles/webcam.dir/webcam.cpp.o: webcam.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/webcam.dir/webcam.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/webcam.dir/webcam.cpp.o -c "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test/webcam.cpp"

CMakeFiles/webcam.dir/webcam.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/webcam.dir/webcam.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test/webcam.cpp" > CMakeFiles/webcam.dir/webcam.cpp.i

CMakeFiles/webcam.dir/webcam.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/webcam.dir/webcam.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test/webcam.cpp" -o CMakeFiles/webcam.dir/webcam.cpp.s

CMakeFiles/webcam.dir/webcam.cpp.o.requires:

.PHONY : CMakeFiles/webcam.dir/webcam.cpp.o.requires

CMakeFiles/webcam.dir/webcam.cpp.o.provides: CMakeFiles/webcam.dir/webcam.cpp.o.requires
	$(MAKE) -f CMakeFiles/webcam.dir/build.make CMakeFiles/webcam.dir/webcam.cpp.o.provides.build
.PHONY : CMakeFiles/webcam.dir/webcam.cpp.o.provides

CMakeFiles/webcam.dir/webcam.cpp.o.provides.build: CMakeFiles/webcam.dir/webcam.cpp.o


# Object files for target webcam
webcam_OBJECTS = \
"CMakeFiles/webcam.dir/webcam.cpp.o"

# External object files for target webcam
webcam_EXTERNAL_OBJECTS =

webcam: CMakeFiles/webcam.dir/webcam.cpp.o
webcam: CMakeFiles/webcam.dir/build.make
webcam: /usr/local/lib/libopencv_videostab.2.4.9.dylib
webcam: /usr/local/lib/libopencv_ts.a
webcam: /usr/local/lib/libopencv_superres.2.4.9.dylib
webcam: /usr/local/lib/libopencv_stitching.2.4.9.dylib
webcam: /usr/local/lib/libopencv_contrib.2.4.9.dylib
webcam: /usr/local/lib/libopencv_nonfree.2.4.9.dylib
webcam: /usr/local/lib/libopencv_ocl.2.4.9.dylib
webcam: /usr/local/lib/libopencv_gpu.2.4.9.dylib
webcam: /usr/local/lib/libopencv_photo.2.4.9.dylib
webcam: /usr/local/lib/libopencv_objdetect.2.4.9.dylib
webcam: /usr/local/lib/libopencv_legacy.2.4.9.dylib
webcam: /usr/local/lib/libopencv_video.2.4.9.dylib
webcam: /usr/local/lib/libopencv_ml.2.4.9.dylib
webcam: /usr/local/lib/libopencv_calib3d.2.4.9.dylib
webcam: /usr/local/lib/libopencv_features2d.2.4.9.dylib
webcam: /usr/local/lib/libopencv_highgui.2.4.9.dylib
webcam: /usr/local/lib/libopencv_imgproc.2.4.9.dylib
webcam: /usr/local/lib/libopencv_flann.2.4.9.dylib
webcam: /usr/local/lib/libopencv_core.2.4.9.dylib
webcam: CMakeFiles/webcam.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable webcam"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/webcam.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/webcam.dir/build: webcam

.PHONY : CMakeFiles/webcam.dir/build

CMakeFiles/webcam.dir/requires: CMakeFiles/webcam.dir/webcam.cpp.o.requires

.PHONY : CMakeFiles/webcam.dir/requires

CMakeFiles/webcam.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/webcam.dir/cmake_clean.cmake
.PHONY : CMakeFiles/webcam.dir/clean

CMakeFiles/webcam.dir/depend:
	cd "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test" "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test" "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test" "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test" "/Users/matthewfonken/Desktop/marbl/xprm/Programs/Combinations/Image Testing/Camera Test/CMakeFiles/webcam.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/webcam.dir/depend
