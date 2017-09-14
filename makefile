SUBDIRS = $(addprefix src/,uart spi can)

.PHONY: all $(SUBDIRS) clean

all: $(SUBDIRS)

clean:
	@$(MAKE) clean -C src/uart
	@$(MAKE) clean -C src/spi
	@$(MAKE) clean -C src/can

$(SUBDIRS):
	@$(MAKE) -C $@
