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
include Source/kwsys/CMakeFiles/cmsysTestDynload.dir/depend.make

# Include the progress variables for this target.
include Source/kwsys/CMakeFiles/cmsysTestDynload.dir/progress.make

# Include the compile flags for this target's objects.
include Source/kwsys/CMakeFiles/cmsysTestDynload.dir/flags.make

Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o: Source/kwsys/CMakeFiles/cmsysTestDynload.dir/flags.make
Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o: /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Source/kwsys/testDynload.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/CMakeFiles $(CMAKE_PROGRESS_1)
	@echo "Building C object Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o"
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Source/kwsys && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/cmsysTestDynload.dir/testDynload.o   -c /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Source/kwsys/testDynload.c

Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.i: cmake_force
	@echo "Preprocessing C source to CMakeFiles/cmsysTestDynload.dir/testDynload.i"
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Source/kwsys && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -E /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Source/kwsys/testDynload.c > CMakeFiles/cmsysTestDynload.dir/testDynload.i

Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.s: cmake_force
	@echo "Compiling C source to assembly CMakeFiles/cmsysTestDynload.dir/testDynload.s"
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Source/kwsys && /usr/bin/gcc  $(C_DEFINES) $(C_FLAGS) -S /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Source/kwsys/testDynload.c -o CMakeFiles/cmsysTestDynload.dir/testDynload.s

Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o.requires:
.PHONY : Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o.requires

Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o.provides: Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o.requires
	$(MAKE) -f Source/kwsys/CMakeFiles/cmsysTestDynload.dir/build.make Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o.provides.build
.PHONY : Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o.provides

Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o.provides.build: Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o
.PHONY : Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o.provides.build

# Object files for target cmsysTestDynload
cmsysTestDynload_OBJECTS = \
"CMakeFiles/cmsysTestDynload.dir/testDynload.o"

# External object files for target cmsysTestDynload
cmsysTestDynload_EXTERNAL_OBJECTS =

Source/kwsys/libcmsysTestDynload.so: Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o
Source/kwsys/libcmsysTestDynload.so: Source/kwsys/CMakeFiles/cmsysTestDynload.dir/build.make
Source/kwsys/libcmsysTestDynload.so: Source/kwsys/CMakeFiles/cmsysTestDynload.dir/link.txt
	@echo "Linking C shared module libcmsysTestDynload.so"
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Source/kwsys && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cmsysTestDynload.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Source/kwsys/CMakeFiles/cmsysTestDynload.dir/build: Source/kwsys/libcmsysTestDynload.so
.PHONY : Source/kwsys/CMakeFiles/cmsysTestDynload.dir/build

Source/kwsys/CMakeFiles/cmsysTestDynload.dir/requires: Source/kwsys/CMakeFiles/cmsysTestDynload.dir/testDynload.o.requires
.PHONY : Source/kwsys/CMakeFiles/cmsysTestDynload.dir/requires

Source/kwsys/CMakeFiles/cmsysTestDynload.dir/clean:
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Source/kwsys && $(CMAKE_COMMAND) -P CMakeFiles/cmsysTestDynload.dir/cmake_clean.cmake
.PHONY : Source/kwsys/CMakeFiles/cmsysTestDynload.dir/clean

Source/kwsys/CMakeFiles/cmsysTestDynload.dir/depend:
	cd /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/src/Source/kwsys /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Source/kwsys /home/dhruv/Comp_Arch_Projects/FinalProject/snipersim/benchmarks/parsec/parsec-2.1/pkgs/tools/cmake/obj/amd64-linux.gcc-sniper/Source/kwsys/CMakeFiles/cmsysTestDynload.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Source/kwsys/CMakeFiles/cmsysTestDynload.dir/depend

