# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.6

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canoncical targets will work.
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
CMAKE_COMMAND = /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Bootstrap.cmk/cmake

# The command to remove a file.
RM = /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Bootstrap.cmk/cmake -E remove -f

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper

# Include any dependencies generated for this target.
include Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/depend.make

# Include the progress variables for this target.
include Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/progress.make

# Include the compile flags for this target's objects.
include Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/flags.make

Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o: Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/flags.make
Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o: /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Utilities/cmxmlrpc/synch_client.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/CMakeFiles $(CMAKE_PROGRESS_1)
	@echo "Building C object Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o"
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Utilities/cmxmlrpc && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/xrtest.dir/synch_client.o   -c /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Utilities/cmxmlrpc/synch_client.c

Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.i: cmake_force
	@echo "Preprocessing C source to CMakeFiles/xrtest.dir/synch_client.i"
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Utilities/cmxmlrpc && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Utilities/cmxmlrpc/synch_client.c > CMakeFiles/xrtest.dir/synch_client.i

Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.s: cmake_force
	@echo "Compiling C source to assembly CMakeFiles/xrtest.dir/synch_client.s"
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Utilities/cmxmlrpc && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Utilities/cmxmlrpc/synch_client.c -o CMakeFiles/xrtest.dir/synch_client.s

Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o.requires:
.PHONY : Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o.requires

Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o.provides: Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o.requires
	$(MAKE) -f Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/build.make Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o.provides.build
.PHONY : Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o.provides

Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o.provides.build: Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o
.PHONY : Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o.provides.build

# Object files for target xrtest
xrtest_OBJECTS = \
"CMakeFiles/xrtest.dir/synch_client.o"

# External object files for target xrtest
xrtest_EXTERNAL_OBJECTS =

bin/xrtest: Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o
bin/xrtest: Utilities/cmxmlrpc/libcmXMLRPC.a
bin/xrtest: Utilities/cmcurl/libcmcurl.a
bin/xrtest: Utilities/cmexpat/libcmexpat.a
bin/xrtest: Utilities/cmzlib/libcmzlib.a
bin/xrtest: Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/build.make
bin/xrtest: Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/link.txt
	@echo "Linking C executable ../../bin/xrtest"
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Utilities/cmxmlrpc && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/xrtest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/build: bin/xrtest
.PHONY : Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/build

Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/requires: Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/synch_client.o.requires
.PHONY : Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/requires

Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/clean:
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Utilities/cmxmlrpc && $(CMAKE_COMMAND) -P CMakeFiles/xrtest.dir/cmake_clean.cmake
.PHONY : Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/clean

Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/depend:
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Utilities/cmxmlrpc /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Utilities/cmxmlrpc /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Utilities/cmxmlrpc/CMakeFiles/xrtest.dir/depend

