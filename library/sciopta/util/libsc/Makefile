# -*-makefile-*-
######################################################################
##  _______  _______ _________ _______  _______ _________ _______   ##
## (  ____ \(  ____ \\__   __/(  ___  )(  ____ )\__   __/(  ___  )  ##
## | (    \/| (    \/   ) (   | (   ) || (    )|   ) (   | (   ) |  ##
## | (_____ | |         | |   | |   | || (____)|   | |   | (___) |  ##
## (_____  )| |         | |   | |   | ||  _____)   | |   |  ___  |  ##
##       ) || |         | |   | |   | || (         | |   | (   ) |  ##
## /\____) || (____/\___) (___| (___) || )         | |   | )   ( |  ##
## \_______)(_______/\_______/(_______)|/          )_(   |/     \|  ##
##                                                                  ##
## (c) 2002,2003 Sciopta Systems GmbH, Litronic AG/ Schweiz         ##
##                                                                  ##
######################################################################
## ID: S03218BS12                                                   ##
## +Revision: 1.25.12.1 +                                                ##
## +Date: 2009/02/24 10:41:20 +                                     ##
## local Makefile                                                   ##
######################################################################

OBJS = chksum.$(OBJEXT) crc32.$(OBJEXT)
OBJS += dectoi.$(OBJEXT)
OBJS += getopt.$(OBJEXT)
OBJS += int2hex.$(OBJEXT) 
OBJS += libsc.$(OBJEXT)
OBJS += scrand.$(OBJEXT)
#OBJS += except.$(OBJEXT)

.SUFFIXES: .cc .c .$(OBJEXT)

.c.$(OBJEXT):
	$(CC) $(CFLAGS) $(INC) $<

all: $(OBJS)

test: tbags.$(OBJEXT) $(OBJS)
	$(CC) tbags.$(OBJEXT) $(OBJS) -o tbags

clean:
	@$(RM) $(OBJS)
	@$(RM) *~
	@$(RM) EDOUT
	@$(RM) *.o *.obj
	@$(RM) cc* *.src

ifeq ($(__IAR__),1)
_CMD = $(addprefix "fetch-mod ,$(OBJS))
CMD = $(_CMD:.$(OBJEXT)=.$(OBJEXT) $(LIBUTIL)")

library: $(OBJS)
	@$(AR) -c $(CMD)
else
library: $(OBJS)
	@$(AR) $(ARFLAGS) $(LIBUTIL) $(OBJS)
endif

########################################
SRC :=$(OBJS:.$(OBJEXT)=.c)
SRC += Makefile
RAWOBJS:= $(OBJS:.$(OBJEXT)=.\$$(OBJEXT)) 
_OBJS := $(addprefix "./"$(MODULE)"/",$(RAWOBJS))

delivery:
	@cp $(SRC) $(SRCDIR)
#	@echo "OBJ += $(_OBJS)" >>$(RULES)
	@echo $(MODULE) >>$(TMP)
