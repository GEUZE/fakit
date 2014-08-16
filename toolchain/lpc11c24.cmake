# This is the toolchain file for the ARM Cortex-M0 processor
# used on the Sensornodes in the DUT14
set(CMAKE_SYSTEM_NAME "Generic") # Keep this on Generic so CMAKE doesn't ask to send this file
set(CMAKE_SYSTEM_PROCESSOR "ARM Cortex-M0")

# Set the compiler
include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER( arm-none-eabi-gcc GNU )
CMAKE_FORCE_CXX_COMPILER( arm-none-eabi-g++ GNU )

# Set the flags for the compiler and linker, CMake

set(FLAGS "-MD -MP -Wall -Wextra -fno-exceptions -fno-builtin -mcpu=cortex-m0 -mthumb -Wa,--defsym=RAM_MODE=0 -DCORE_M0 -g" CACHE STRING "" FORCE)

# Set the compiler flags for this processor, the C compiler and C++ compiler
set(CMAKE_CXX_FLAGS "${FLAGS} -std=c++11" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS   "${FLAGS} -std=c99" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS "${FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "-T${CMAKE_CURRENT_LIST_DIR}/lpc11xx/ldscript_rom_gnu.ld" CACHE STRING "" FORCE)

set(TOOLCHAIN_SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/lpc11xx/cmsis/source/core_cm0.c"
	"${CMAKE_CURRENT_LIST_DIR}/lpc11xx/cmsis/source/system_LPC11xx.c"
	"${CMAKE_CURRENT_LIST_DIR}/lpc11xx/cmsis/source/lpc11xx_startup.c"
	CACHE STRING "The source files needed to be included for the toolchain" FORCE)
set(TOOLCHAIN_INCLUDE_DIRS
	"${CMAKE_CURRENT_LIST_DIR}/lpc11xx/cmsis/include"
	CACHE STRING "The include directories for the toolchain" FORCE)

# Include a file that stubs the posix functions, the semihositing gdb script
# then puts a breakpoint on a write operation and displays it in the console.
# list( APPEND TOOLCHAIN_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/semihosting.c")

# Make a function to add the upload target
function(register_upload_target NODE_NAME)
	add_custom_target("upload-${NODE_NAME}"
		COMMAND arm-none-eabi-gdb -x ${CMAKE_CURRENT_LIST_DIR}/lpc11c24.gdb ${NODE_NAME}
		DEPENDS ${NODE_NAME}
	)
endfunction()
