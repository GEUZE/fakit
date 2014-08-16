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
## ID: S03218BS11                                                   ##
## +Revision: 1.25.12.2 +                                                ##
## +Date: 2009/03/26 17:45:33 +                                     ##
## local Makefile                                                   ##
######################################################################

OBJS += array.$(OBJEXT)
OBJS += fifo.$(OBJEXT)
OBJS += libsgl.$(OBJEXT)
OBJS += libdbl.$(OBJEXT)
OBJS += list.$(OBJEXT) 
OBJS += swqueue.$(OBJEXT)  
#OBJS += hash.$(OBJEXT) 
#OBJS += stack.$(OBJEXT)

.SUFFIXES: .cc .c .$(OBJEXT)

.c.$(OBJEXT):
	$(CC) $(CFLAGS) $(CFLAGS_CONTAINER) $(INC) $<

all: $(OBJS)

test: tbags.$(OBJEXT) $(OBJS)
	$(CC) tbags.$(OBJEXT) $(OBJS) -o tbags

clean:
	@$(RM) $(OBJS)
	@$(RM) *~
	@$(RM) *.o *.obj
	@$(RM) EDOUT
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
SRC +=  Makefile
RAWOBJS:= $(OBJS:.$(OBJEXT)=.\$$(OBJEXT)) 
_OBJS := $(addprefix "./"$(MODULE)"/",$(RAWOBJS))

delivery:
	@cp $(SRC) $(SRCDIR)
	@echo $(MODULE) >>$(TMP)
