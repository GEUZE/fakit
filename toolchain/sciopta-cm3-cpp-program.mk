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
# Makefile for Cortex M3 C++ programs that use Sciopta
# Author: Maurice Bos <m-ou.se@m-ou.se>
#------------------------------------------

BUILD ?= ../toolchain

include $(BUILD)/sciopta-cm3.mk

SCIOPTA_SOURCES += $(BUILD)/sconf.cpp
SCIOPTA_SOURCES += $(BUILD)/sciopta-cm3-startup.cpp

$(PROGRAM)_CPPFLAGS += $(SCIOPTA_CM3_CPPFLAGS)
$(PROGRAM)_INCLUDES += $(SCIOPTA_CM3_INCLUDES)
$(PROGRAM)_CXXFLAGS += $(SCIOPTA_CM3_CXXFLAGS)
$(PROGRAM)_CCFLAGS  += $(SCIOPTA_CM3_CCFLAGS)
$(PROGRAM)_LDFLAGS  += $(SCIOPTA_CM3_LDFLAGS)
$(PROGRAM)_SOURCES  += $(SCIOPTA_CM3_SOURCES) $(BUILD)/cm3-cpp.cpp
$(PROGRAM)_SOURCES  += $(wildcard $($(PROGRAM)_DRIVERS:%=$(CM3_DRIVER_DIR)/source/lpc*_%.c))

include $(BUILD)/cpp-program.mk

