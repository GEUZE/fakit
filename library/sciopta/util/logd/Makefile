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
## ID: S03218BS13                                                   ##
## +Revision: 1.18.12.2 +                                                ##
## +Date: 2009/03/26 17:45:33 +                                     ##
## local Makefile                                                   ##
######################################################################

OBJS  = logd.$(OBJEXT) 
OBJS += kprintf.$(OBJEXT)

.SUFFIXES: .cc .c .$(OBJEXT)

%.$(OBJEXT): %.c
	$(CC) $(CFLAGS) $(INC) $<

all: $(OBJS)

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
ifeq ($(__IAR__),2)
library: $(OBJS)
	$(AR) $(LIBUTIL) $(OBJS)
else
library: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBUTIL) $(OBJS)
endif
endif

########################################
SRC:=$(OBJS:.$(OBJEXT)=.c) 
SRC += Makefile
RAWOBJS:= $(OBJS:.$(OBJEXT)=.\$$(OBJEXT)) 
_OBJS := $(addprefix "./"$(MODULE)"/",$(RAWOBJS))

delivery:
	@cp $(SRC) $(SRCDIR)
	@echo $(MODULE) >>$(TMP)
