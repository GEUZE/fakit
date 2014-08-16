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
# Makefile include for uploading and debugging
# a Cortex M3 progam on a LPC1788.
# Author: Maurice Bos <m-ou.se@m-ou.se>
#------------------------------------------

BUILD ?= ../toolchain

ifndef UPLOAD_TARGET
	$(error No UPLOAD_TARGET specified.)
endif

.PHONY: upload debug
upload: $(PROGRAM) $(BUILD)/$(UPLOAD_TARGET).gdb
	$(GDB) -batch -x $(BUILD)/$(UPLOAD_TARGET).gdb $($(PROGRAM)_GDBFLAGS) $(GDBFLAGS) $<

debug: $(PROGRAM) $(BUILD)/$(UPLOAD_TARGET).gdb
	$(GDB) -x $(BUILD)/$(UPLOAD_TARGET).gdb $($(PROGRAM)_GDBFLAGS) $(GDBFLAGS) $<

