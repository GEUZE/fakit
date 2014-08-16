#  _____   _    _ _________ _   _  _
#  |  __ \ | |  | |__   __//_ || || |
#  | |  | || |  | |  | |    | || || |_
#  | |  | || |  | |  | |    | ||__   _|
#  | |__| || |__| |  | |    | |   | |
#  |_____/  \____/   |_|    |_|   |_|
#
# Delft University of Technology
# Racing Team 2013-2014
#------------------------------------------
# Makefile for the User Interface
#------------------------------------------

PROGRAM = ui

ui_CPPFLAGS += -I../library/sciopta++/

ui_ANALYZE_FLAGS += -m32
ui_ANALYZE_SKIP += setup.cpp

# Sources that do not belong in any process.
ui_SOURCES += setup.cpp
ui_SOURCES += systick.cpp
ui_SOURCES += system_settings.c

# Include all .cpp and .c files from directories specified in the file 'processes'
$(foreach p,$(shell cat processes),$(eval ui_SOURCES += $(wildcard $p/*.cpp) $(wildcard $p/*.c)))
ui: processes

ui_LDFLAGS = -T../toolchain/lpc1778.ld

UPLOAD_TARGET = lpc1778
include ../toolchain/semihosting.mk
include ../toolchain/lpc177x_8x.mk
include ../toolchain/sciopta-cm3-cpp-program.mk
include ../toolchain/upload.mk

