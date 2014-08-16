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
## ID: S03218BS9                                                    ##
## +Revision: 1.29.12.1 +                                                ##
## +Date: 2009/03/26 17:45:32 +                                     ##
## local Makefile                                                   ##
######################################################################

OBJS = devaread.$(OBJEXT) devawrite.$(OBJEXT) devclose.$(OBJEXT) 
OBJS += devioctl.$(OBJEXT) devopen.$(OBJEXT) devopenbyname.$(OBJEXT)
OBJS += devread.$(OBJEXT)  devwrite.$(OBJEXT) 

OBJS += fileseek.$(OBJEXT) fileresize.$(OBJEXT)

OBJS += manadd.$(OBJEXT) manager.$(OBJEXT)  manget1st.$(OBJEXT) 
OBJS += mangetbypath.$(OBJEXT) mangetbyname.$(OBJEXT) mangetnext.$(OBJEXT) 
OBJS += mangetroot.$(OBJEXT) 
OBJS += manrm.$(OBJEXT) mannotifyadd.$(OBJEXT) mannotifyrm.$(OBJEXT)

OBJS += netbufalloc.$(OBJEXT) netbufcp.$(OBJEXT) netbufcur.$(OBJEXT)
OBJS += netbufcurreset.$(OBJEXT) netbufdup.$(OBJEXT) netbuffree.$(OBJEXT)
OBJS += netbufpull.$(OBJEXT) netbufpush.$(OBJEXT) netbufput.$(OBJEXT)  
OBJS += netbufrealloc.$(OBJEXT) netbuftrim.$(OBJEXT) 

OBJS += netclose.$(OBJEXT) netopen.$(OBJEXT)

OBJS += objdup.$(OBJEXT) objfree.$(OBJEXT) objnew.$(OBJEXT) 
OBJS += objresolve.$(OBJEXT) objtag.$(OBJEXT) objtimeget.$(OBJEXT)
OBJS += objtimeset.$(OBJEXT) objsizeget.$(OBJEXT) objdesc.$(OBJEXT)

OBJS += stream.$(OBJEXT)

.SUFFIXES: .cc .c .$(OBJEXT)

.c.$(OBJEXT):
	$(CC) $(CFLAGS) $(CFLAGS_SDD) $(INC) $<

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
ifeq ($(__IAR__),2)
library: $(OBJS)
	$(AR) $(LIBGDD) $(OBJS)
else
library: $(OBJS)
	@$(AR) $(ARFLAGS) $(LIBGDD) $(OBJS)
endif
endif

########################################
SRC :=$(OBJS:.$(OBJEXT)=.c) 
SRC += Makefile
RAWOBJS:= $(OBJS:.$(OBJEXT)=.\$$(OBJEXT)) 
_OBJS := $(addprefix "./"$(MODULE)"/",$(RAWOBJS))

delivery:
	@cp $(SRC) $(SRCDIR)
#	@echo "OBJ += $(_OBJS)" >>$(RULES)
	@echo "export CFLAGS_SDD = $(CFLAGS_SDD)" >>$(RULES)
	@echo $(MODULE) >>$(TMP)
