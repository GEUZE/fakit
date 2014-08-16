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
## ID: S04135BS3                                                    ##
## +Revision: 1.11.12.1 +                                                ##
## +Date: 2009/03/27 12:11:59 +                                     ##
## local Makefile                                                   ##
######################################################################

OBJS = cp.$(OBJEXT)
OBJS += chroot.$(OBJEXT)
OBJS += mkdir.$(OBJEXT)
OBJS += mount.$(OBJEXT)
OBJS += mv.$(OBJEXT)
OBJS += setup_root.$(OBJEXT)
OBJS += sync.$(OBJEXT)
OBJS += umount.$(OBJEXT)
#OBJS += mkfatfs.$(OBJEXT)

.SUFFIXES: .cc .c .$(OBJEXT)

.c.$(OBJEXT):
	$(CC) $(CFLAGS) $(CFLAGS_FSSH) $(INC) $<

all: $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f *.$(OBJEXT) *~

ifeq ($(__IAR__),1)
_CMD = $(addprefix "fetch-mod ,$(OBJS))
CMD = $(_CMD:.$(OBJEXT)=.$(OBJEXT) $(LIBSFS)")

library: $(OBJS)
	@$(AR) -c $(CMD)
else
library: $(OBJS)
	@$(AR) $(ARFLAGS) $(LIBSFS) $(OBJS)
endif
########################################
SRC :=$(OBJS:.$(OBJEXT)=.c) 
SRC += Makefile

delivery:
	@cp $(SRC) $(SRCDIR)
	@echo $(MODULE) >>$(TMP)
