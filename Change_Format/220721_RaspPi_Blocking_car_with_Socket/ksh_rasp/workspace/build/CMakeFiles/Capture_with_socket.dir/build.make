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
CMAKE_SOURCE_DIR = /home/kshksh/ksh_rasp/workspace

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/kshksh/ksh_rasp/workspace/build

# Include any dependencies generated for this target.
include CMakeFiles/Capture_with_socket.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Capture_with_socket.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Capture_with_socket.dir/flags.make

CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.o: CMakeFiles/Capture_with_socket.dir/flags.make
CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.o: ../Capture_with_socket.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/kshksh/ksh_rasp/workspace/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.o -c /home/kshksh/ksh_rasp/workspace/Capture_with_socket.cpp

CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/kshksh/ksh_rasp/workspace/Capture_with_socket.cpp > CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.i

CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/kshksh/ksh_rasp/workspace/Capture_with_socket.cpp -o CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.s

# Object files for target Capture_with_socket
Capture_with_socket_OBJECTS = \
"CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.o"

# External object files for target Capture_with_socket
Capture_with_socket_EXTERNAL_OBJECTS =

Capture_with_socket: CMakeFiles/Capture_with_socket.dir/Capture_with_socket.cpp.o
Capture_with_socket: CMakeFiles/Capture_with_socket.dir/build.make
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_gapi.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_highgui.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_ml.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_objdetect.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_photo.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_stitching.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_video.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_videoio.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_dnn.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_imgcodecs.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_calib3d.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_features2d.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_flann.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_imgproc.so.4.5.1
Capture_with_socket: /usr/local/lib/arm-linux-gnueabihf/libopencv_core.so.4.5.1
Capture_with_socket: CMakeFiles/Capture_with_socket.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/kshksh/ksh_rasp/workspace/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Capture_with_socket"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Capture_with_socket.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Capture_with_socket.dir/build: Capture_with_socket

.PHONY : CMakeFiles/Capture_with_socket.dir/build

CMakeFiles/Capture_with_socket.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Capture_with_socket.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Capture_with_socket.dir/clean

CMakeFiles/Capture_with_socket.dir/depend:
	cd /home/kshksh/ksh_rasp/workspace/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/kshksh/ksh_rasp/workspace /home/kshksh/ksh_rasp/workspace /home/kshksh/ksh_rasp/workspace/build /home/kshksh/ksh_rasp/workspace/build /home/kshksh/ksh_rasp/workspace/build/CMakeFiles/Capture_with_socket.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Capture_with_socket.dir/depend

