#-*-makefile-*-
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
## ID: S03217BS30                                                   ##
## +Revision: 1.18 +                                                 ##
## +Date: 2008/11/11 10:00:53 +                                     ##
## Global Sciopta Makefile to build all libs                        ##
######################################################################

HOSTS= i386 ppc coldfire arm 68hc12
MODULES = sfs ips util gdd 
pwd=`pwd`
all:
	@(if [ $(TARGET)x = x ]; then\
	 echo "Error:Valid targets are: clean distclean $(HOSTS)";\
	 exit -1;fi)

	$(MAKE) -f make/Makefile.gnu real_all

i386:
	$(MAKE) -f make/Makefile.gnu TARGET=i386 real_all
ppc:
	$(MAKE) -f make/Makefile.gnu TARGET=ppc real_all

real_all:
	@for i in $(MODULES); do $(MAKE) -C $$i -f Makefile.$(TARGET) MODULE=$ii all || exit; done

clean:
	@for i in $(MODULES); do $(MAKE) -C $$i -f Makefile MODULE=$ii clean; done

dist_clean:
	@for i in $(MODULES); do $(MAKE) -C $$i -f Makefile MODULE=$ii dist_clean; done

.PHONY: $(MODULES) delivery_lib

delivery_lib: $(MODULES)

$(MODULES):
	@( case $(TARGET) in \
		*m16c*) echo Nothing to do for $(TARGET);; \
		*) \
	   	echo Building $@;\
		$(MAKE) -s -C $@ -f Makefile.$(TARGET) MODULE=$@ delivery_lib\
		;;\
	esac)
