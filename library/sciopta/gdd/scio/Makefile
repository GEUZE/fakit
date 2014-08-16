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
## ID: S03218BS7                                                    ##
## +Revision: 1.44.12.1 +                                                ##
## +Date: 2009/03/26 17:45:32 +                                     ##
## local Makefile                                                   ##
######################################################################

OBJS = accept.$(OBJEXT) aread.$(OBJEXT) awrite.$(OBJEXT)
OBJS += bind.$(OBJEXT) 
OBJS += chroot.$(OBJEXT) close.$(OBJEXT) closedir.$(OBJEXT) connect.$(OBJEXT)
OBJS += dup.$(OBJEXT)
OBJS += fcntl.$(OBJEXT)
OBJS += fdprintf.$(OBJEXT)
OBJS += inet.$(OBJEXT) ioctl.$(OBJEXT)
OBJS += listen.$(OBJEXT) lseek.$(OBJEXT)
OBJS += mkdir.$(OBJEXT)
OBJS += netdb.$(OBJEXT)
OBJS += open.$(OBJEXT) opendir.$(OBJEXT)
OBJS += perror.$(OBJEXT) pipe.$(OBJEXT)
OBJS += read.$(OBJEXT) readdir.$(OBJEXT) recv.$(OBJEXT) recvfrom.$(OBJEXT) \
        rewinddir.$(OBJEXT) rmdir.$(OBJEXT)
OBJS += send.$(OBJEXT) sendto.$(OBJEXT) socket.$(OBJEXT) sockopt.$(OBJEXT)
OBJS += unlink.$(OBJEXT)
OBJS += write.$(OBJEXT)

.SUFFIXES: .cc .c .$(OBJEXT)

.c.$(OBJEXT):
	$(CC) $(CFLAGS) $(CFLAGS_SCIO) $(INC) $<

all: $(OBJS)

clean:
	$(RM) $(OBJS)
	$(RM) *~
	$(RM) *.o *.obj
	$(RM) EDOUT
	$(RM) cc* *.src *.tmp

ifeq ($(__IAR__),1)
_CMD = $(addprefix "fetch-mod ,$(OBJS))
CMD = $(_CMD:.$(OBJEXT)=.$(OBJEXT) $(LIBGDD)")

library: $(OBJS)
	@$(AR) -c $(CMD)
else
library: $(OBJS)
	$(AR) $(ARFLAGS) $(LIBGDD) $(OBJS)
endif

########################################
SRC :=$(OBJS:.$(OBJEXT)=.c) 
SRC += Makefile
RAWOBJS:= $(OBJS:.$(OBJEXT)=.\$$(OBJEXT)) 
_OBJS := $(addprefix "./"$(MODULE)"/",$(RAWOBJS))

delivery:
	@cp $(SRC) $(SRCDIR)
	@echo "export CFLAGS_SCIO = $(CFLAGS_SCIO)" >>$(RULES)
	@echo $(MODULE) >>$(TMP)
