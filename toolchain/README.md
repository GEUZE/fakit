This folder contains the build files for every processor. The CMake toolchain file contains the compiler and linker directives to build the software for the different processors.

# Directory structure

	/$processor$.cmake - The CMake toolchain file
	/$processor$.ld    - The linker file which gives information about where to ROM and RAM is
	/$processor$.gdb   - A GDB script to upload the code via a J-Link to the processor
