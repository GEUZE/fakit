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
# Generic Makefile for a C++ program
# Author: Maurice Bos <m-ou.se@m-ou.se>
#------------------------------------------

ifeq ($(VERBOSE),)
	MAKEFLAGS = -s
endif

CXXFLAGS ?= -O3 -g -Wall -Wno-narrowing
CCFLAGS ?= $(CXXFLAGS)

CXX_ANALYZE ?= clang++ --analyze
CC_ANALYZE ?= clang --analyze

$(PROGRAM)_TARGET ?= $(PROGRAM)

$($(PROGRAM)_TARGET):

.PHONY: analyze
analyze:

-include $(wildcard .build/*.d)

define target
$(eval obj := .build/$(subst /,-,$(subst ..,XX,$(src))).o)
$(eval $(PROGRAM)_OBJECTS += $(obj))
$(eval cc := $(if $(patsubst %.c,,$(suffix $(src))),CXX,CC))
$(eval c := $(if $(patsubst %.c,,$(suffix $(src))),CXX,C))
$(obj): $(src)
	@echo -e "\033[1;33mCompiling $(src).\033[m"
	@mkdir -p .build
	$($(cc)) -MD -MP -c $($(PROGRAM)_CPPFLAGS) $(CPPFLAGS) $($(PROGRAM)_INCLUDES) $(INCLUDES) $($(PROGRAM)_$(cc)FLAGS) $($(cc)FLAGS) $$< -o $$@
endef

define analyzetarget
$(eval name := $(subst /,-,$(subst ..,XX,$(src))))
$(eval c := $(if $(patsubst %.c,,$(suffix $(src))),CXX,CC))
.PHONY: analyze-$(name)
analyze-$(name): $(src)
	@echo -e "\033[1;31mAnalyzing $(src).\033[m"
	@mkdir -p .analyze
	$($(c)_ANALYZE) $($(PROGRAM)_ANALYZE_FLAGS) $(ANALYZE_FLAGS) $($(PROGRAM)_CPPFLAGS) $(CPPFLAGS) $($(PROGRAM)_INCLUDES) $(INCLUDES) $$< -o .analyze/$(name).plist || true
analyze: analyze-$(name)
endef

$(foreach src,$($(PROGRAM)_SOURCES),$(eval $(target)))
$(foreach src,$(filter-out $(BUILD)/% ../% /% $($(PROGRAM)_ANALYZE_SKIP) $(ANALYZE_SKIP),$(filter %.cpp %.c,$($(PROGRAM)_SOURCES))),$(eval $(analyzetarget)))

$($(PROGRAM)_TARGET): $($(PROGRAM)_OBJECTS)
	@echo -e "\033[1;32mLinking $(PROGRAM).\033[m"
	$(CXX) $($(PROGRAM)_CXXFLAGS) $(CXXFLAGS) $($(PROGRAM)_LDFLAGS) $(LDFLAGS) $($(PROGRAM)_OBJECTS) $($(PROGRAM)_LIBS) -o $@

.PHONY: clean
clean:
	@echo -e "\033[1;37mCleaning $(PROGRAM).\033[m"
	rm -rf .build
	rm -f $($(PROGRAM)_TARGET)

