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
## ID: S04135BS4                                                    ##
## +Revision: 1.21.12.2 +                                                ##
## +Date: 2009/03/27 12:11:59 +                                     ##
## local Makefile                                                   ##
######################################################################

OBJS  = blkdev.$(OBJEXT)
OBJS += link.$(OBJEXT)
#OBJS += mkdir.$(OBJEXT)
OBJS += mount.$(OBJEXT)
OBJS += rename.$(OBJEXT)
OBJS += setup.$(OBJEXT)
OBJS += sfs_assign.$(OBJEXT) 
OBJS += sfs_close.$(OBJEXT)
OBJS += sfs_create.$(OBJEXT)
OBJS += sfs_delete.$(OBJEXT)
OBJS += sfs_findfirst.$(OBJEXT)
OBJS += sfs_get.$(OBJEXT)
OBJS += sfs_sync.$(OBJEXT) 
OBJS += sfs_mount.$(OBJEXT) 
OBJS += sfs_move.$(OBJEXT) 
OBJS += sfs_open.$(OBJEXT)
OBJS += sfs_read.$(OBJEXT)
OBJS += sfs_resize.$(OBJEXT)
OBJS += sfs_seek.$(OBJEXT)
OBJS += sfs_unassign.$(OBJEXT)
OBJS += sfs_umount.$(OBJEXT) 
OBJS += sfs_write.$(OBJEXT)
OBJS += sync.$(OBJEXT)
OBJS += umount.$(OBJEXT)


.SUFFIXES: .cc .c .$(OBJEXT)

.c.$(OBJEXT):
	$(CC) $(CFLAGS) $(CFLAGS_SFS) $(INC) $<

all: $(OBJS)

clean:
	rm -f $(OBJS)
	rm -f *.$(OBJEXT) *~

dist_clean: 
	rm -f $(OBJS)
	rm -f *.$(OBJEXT) *~

	rm -f $(OBJS)
	rm -f *.$(OBJEXT) *~

ifeq ($(__IAR__),1)
_CMD = $(addprefix "fetch-mod ,$(OBJS))
CMD = $(_CMD:.$(OBJEXT)=.$(OBJEXT) $(LIBSFS)")

library: $(OBJS)
	@$(AR) -c $(CMD)
else
ifeq ($(__IAR__),2)
library: $(OBJS)
	$(AR) $(LIBSFS) $(OBJS)
else
library: $(OBJS)
	@$(AR) $(ARFLAGS) $(LIBSFS) $(OBJS)
endif
endif

########################################
SRC :=$(OBJS:.$(OBJEXT)=.c) 
SRC += Makefile

delivery:
	@cp $(SRC) $(SRCDIR)
	@echo $(MODULE) >>$(TMP)
