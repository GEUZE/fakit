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
## +Revision: 1.9.12.1 +                                                 ##
## +Date: 2009/03/26 17:45:32 +                                     ##
## local Makefile                                                   ##
######################################################################

OBJS = sem.$(OBJEXT)

.SUFFIXES: .cc .c .$(OBJEXT)

.c.$(OBJEXT):
	$(CC) $(CFLAGS) $(CFLAGS_SEM) $(INC) $<

all: $(OBJS)

clean:
	@$(RM) $(OBJS)
	@$(RM) *~
	@$(RM) EDOUT
	@$(RM) *.o *.obj
	@$(RM) cc* *.src *.tmp

ifeq ($(__IAR__),1)
_CMD = $(addprefix "fetch-mod ,$(OBJS))
CMD = $(_CMD:.$(OBJEXT)=.$(OBJEXT) $(LIBGDD)")

library: $(OBJS)
	@$(AR) -c $(CMD)
else
library: $(OBJS)
	@$(AR) $(ARFLAGS) $(LIBGDD) $(OBJS)
endif

########################################
SRC :=$(OBJS:.$(OBJEXT)=.c) 
SRC += Makefile
RAWOBJS:= $(OBJS:.$(OBJEXT)=.\$$(OBJEXT)) 
_OBJS := $(addprefix "./"$(MODULE)"/",$(RAWOBJS))

delivery:
	@cp $(SRC) $(SRCDIR)
#	@echo "OBJ += $(_OBJS)" >>$(RULES)
	@echo "export CFLAGS_SEM = $(CFLAGS_SEM)" >>$(RULES)
	@echo $(MODULE) >>$(TMP)
