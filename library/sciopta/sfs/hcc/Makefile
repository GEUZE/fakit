MODULES= src include effs fatfs effs-tiny capi fatfs_thin fatfs_sthin

export INC += -I$(SCDEV_ROOT)/sfs/hcc/include

all library clean delivery_lib:
	@echo Nothing to do for $@

delivery:
	@mkdir -p $(SRCDIR)
	@cp Makefile $(SRCDIR)
	@for i in $(MODULES) ; do \
		$(MAKE) -C $$i SRCDIR=$(SRCDIR)/$$i MODULE=$(MODULE)/$$i delivery ;\
	done
