SUBDIRS = $(addprefix src/,uart spi can)

.PHONY: all $(SUBDIRS) clean

all: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@
