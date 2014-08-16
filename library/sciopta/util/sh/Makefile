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
## ID: S03218BS14                                                   ##
## +Revision: 1.33.12.2 +                                                ##
## +Date: 2009/03/26 17:45:33 +                                     ##
## local Makefile                                                   ##
######################################################################

OBJS += cat.$(OBJEXT) cmds.$(OBJEXT) sh_dump.$(OBJEXT) 
OBJS += echo.$(OBJEXT) sh_error.$(OBJEXT) sh_exit.$(OBJEXT)
OBJS += fg.$(OBJEXT)
OBJS += sh_hello.$(OBJEXT) helper.$(OBJEXT)
OBJS += if.$(OBJEXT)
OBJS += launcher.$(OBJEXT) login.$(OBJEXT) ls.$(OBJEXT)
OBJS += mid.$(OBJEXT) more.$(OBJEXT)
OBJS += pid.$(OBJEXT)
OBJS += rm.$(OBJEXT)
OBJS += sh.$(OBJEXT)
OBJS += test.$(OBJEXT) trace.$(OBJEXT)
OBJS += while.$(OBJEXT) wrapper.$(OBJEXT)    

.SUFFIXES: .cc .c .$(OBJEXT)

.c.$(OBJEXT):
	$(CC) $(CFLAGS) $(CFLAGS_SH) $(INC) $<

all: $(OBJS) 

clean:
	@$(RM) $(OBJS)
	@$(RM) *~
	@$(RM) EDOUT
	@$(RM) cc* *.src
	@$(RM) *.o *.obj

ifeq ($(__IAR__),1)
_CMD = $(addprefix "fetch-mod ,$(OBJS))
CMD = $(_CMD:.$(OBJEXT)=.$(OBJEXT) $(LIBSH)")

library: $(OBJS)
	@$(AR) -c $(CMD)
else
ifeq ($(__IAR__),2)
library: $(OBJS)
	@$(AR) $(LIBSH) $(OBJS)
else
library: $(OBJS)
	@$(AR) $(ARFLAGS) $(LIBSH) $(OBJS)
endif
endif

########################################
SRC :=$(OBJS:.$(OBJEXT)=.c)
SRC += Makefile
RAWOBJS:= $(OBJS:.$(OBJEXT)=.\$$(OBJEXT)) 
_OBJS := $(addprefix "./"$(MODULE)"/",$(RAWOBJS))

delivery:
	@cp $(SRC) $(SRCDIR)
	@echo $(MODULE) >>$(TMP)
