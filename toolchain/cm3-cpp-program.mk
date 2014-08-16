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
# Makefile for Cortex M3 C++ programs
# Author: Maurice Bos <m-ou.se@m-ou.se>
#------------------------------------------

BUILD ?= ../toolchain

include $(BUILD)/cm3.mk

$(PROGRAM)_CPPFLAGS += $(CM3_CPPFLAGS)
$(PROGRAM)_INCLUDES += $(CM3_INCLUDES)
$(PROGRAM)_CXXFLAGS += $(CM3_CXXFLAGS)
$(PROGRAM)_CCFLAGS  += $(CM3_CCFLAGS)
$(PROGRAM)_LDFLAGS  += $(CM3_LDFLAGS)
$(PROGRAM)_SOURCES  += $(CM3_SOURCES) $(BUILD)/cm3-cpp.cpp

include $(BUILD)/cpp-program.mk

