# This is the toolchain file for the ARM Cortex-M4 processor
# used on the ECU in the DUT14
set(CMAKE_SYSTEM_NAME "Generic") # Keep this on Generic so CMAKE doesn't ask to send this file
set(CMAKE_SYSTEM_PROCESSOR "ARM Cortex-M4")

# Set the compiler
include(CMakeForceCompiler)
CMAKE_FORCE_C_COMPILER( arm-none-eabi-gcc GNU )
CMAKE_FORCE_CXX_COMPILER( arm-none-eabi-g++ GNU )

# Set the flags for the compiler and linker, CMake

set(FLAGS "-MD -MP -Wall -Wextra -fno-exceptions -fno-builtin -mcpu=cortex-m4 -mthumb -Wa,--defsym=RAM_MODE=0 -mfpu=vfpv3xd -mfloat-abi=softfp -DCORE_M4 -g -fdata-sections -ffunction-sections -O3 -fno-strict-aliasing" CACHE STRING "" FORCE)

# Set the compiler flags for this processor, the C compiler and C++ compiler
set(CMAKE_CXX_FLAGS "${FLAGS} -std=c++11" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS   "${FLAGS} -std=c99" CACHE STRING "" FORCE)
set(CMAKE_ASM_FLAGS "${FLAGS}" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "-T${CMAKE_CURRENT_LIST_DIR}/lpc407x_8x/ldscript_rom_gnu.ld" CACHE STRING "" FORCE)

set(TOOLCHAIN_SOURCE_FILES
	"${CMAKE_CURRENT_LIST_DIR}/lpc407x_8x/cmsis/source/startup_LPC407x_8x_177x_8x.s"
	"${CMAKE_CURRENT_LIST_DIR}/lpc407x_8x/cmsis/source/system_LPC407x_8x_177x_8x.c"
	"${CMAKE_CURRENT_LIST_DIR}/lpc407x_8x/sdram.c"
	CACHE STRING "The source files needed to be included for the toolchain" FORCE)
set(TOOLCHAIN_INCLUDE_DIRS
	"${CMAKE_CURRENT_LIST_DIR}/lpc407x_8x/cmsis/include"
	"${CMAKE_CURRENT_LIST_DIR}/lpc407x_8x/"
	CACHE STRING "The include directories for the toolchain" FORCE)

# Add the needed FreeRTOS settings to the toolchain settings
# if necesary
if( USE_FREERTOS )
	include("${CMAKE_CURRENT_LIST_DIR}/freertos.cmake")
	list( APPEND TOOLCHAIN_SOURCE_FILES
		${FREERTOS_SOURCE_FILES}
	"${CMAKE_CURRENT_LIST_DIR}/lpc407x_8x/freertos_port_CM4F/port.c")
	list( APPEND TOOLCHAIN_INCLUDE_DIRS
		${FREERTOS_INCLUDE_DIRS}
		"${CMAKE_CURRENT_LIST_DIR}/lpc407x_8x/freertos_port_CM4F/")
endif( USE_FREERTOS )

# Include a file that stubs the posix functions, the semihositing gdb script
# then puts a breakpoint on a write operation and displays it in the console.
list( APPEND TOOLCHAIN_SOURCE_FILES "${CMAKE_CURRENT_LIST_DIR}/semihosting.c")

# Add a command to upload the program to the target via gdb.
add_custom_target(upload
	COMMAND arm-none-eabi-gdb -x ${CMAKE_CURRENT_LIST_DIR}/lpc4088.gdb -x ${CMAKE_CURRENT_LIST_DIR}/semihosting.gdb ${TARGET_NAME}
	DEPENDS ${TARGET_NAME}
)
