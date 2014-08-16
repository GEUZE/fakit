#   _____   _    _  _______  __  _  _
#  |  __ \ | |  | ||__   __|/_ || || |
#  | |  | || |  | |   | |    | || || |_
#  | |  | || |  | |   | |    | ||__   _|
#  | |__| || |__| |   | |    | |   | |
#  |_____/  \____/    |_|    |_|   |_|
#
# Delft University of Technology
# Racing Team 2013-2014
#------------------------------------------
# Makefile include for using SCIOPTA and LPC17xx CMSIS
# Author: Maurice Bos <m-ou.se@m-ou.se>
#------------------------------------------

BUILD ?= ../toolchain

include $(BUILD)/cm3.mk

SCIOPTA ?= $(BUILD)/../library/sciopta
SCIOPTA_CPPFLAGS =
SCIOPTA_INCLUDES = -I$(SCIOPTA)/include -I$(SCIOPTA)/include/sciopta/arm -I$(BUILD)
SCIOPTA_CXXFLAGS =
SCIOPTA_CCFLAGS  =
SCIOPTA_LDFLAGS  = -T$(BUILD)/sciopta-cm3.ld
SCIOPTA_SOURCES  = $(SCIOPTA)/lib/arm/krn/sciopta.S
SCIOPTA_SOURCES += $(SCIOPTA)/bsp/arm/src/gnu/cortexm3_exception.S
SCIOPTA_SOURCES += $(SCIOPTA)/bsp/arm/src/gnu/cortexm3_vector.S
SCIOPTA_SOURCES += $(SCIOPTA)/bsp/arm/src/gnu/cortexm3_cstartup.S

SCIOPTA_CM3_CPPFLAGS = $(CM3_CPPFLAGS) $(SCIOPTA_CPPFLAGS)
SCIOPTA_CM3_INCLUDES = $(CM3_INCLUDES) $(SCIOPTA_INCLUDES)
SCIOPTA_CM3_CXXFLAGS = $(CM3_CXXFLAGS) $(SCIOPTA_CXXFLAGS)
SCIOPTA_CM3_CCFLAGS  = $(CM3_CCFLAGS)  $(SCIOPTA_CCFLAGS)
SCIOPTA_CM3_LDFLAGS  = $(filter-out -T%,$(CM3_LDFLAGS)) $(SCIOPTA_LDFLAGS)
SCIOPTA_CM3_SOURCES  = $(filter-out %/startup_LPC$(CM3_LPC).s,$(CM3_SOURCES)) $(SCIOPTA_SOURCES)

