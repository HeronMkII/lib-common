SUBDIRS = $(addprefix src/,uart spi can timer)

.PHONY: all $(SUBDIRS) clean

export CC = avr-gcc
export AR = avr-ar
export RANLIB = avr-ranlib
export INCLUDES = -I../../include
#export LDFLAGS = -L../../lib
export CFLAGS = -std=gnu99 -g -mmcu=atmega32m1 -Os -mcall-prologues

all: $(SUBDIRS)

clean:
	@$(MAKE) clean -C src/uart
	@$(MAKE) clean -C src/spi
	@$(MAKE) clean -C src/can
	@$(MAKE) clean -C src/timer

$(SUBDIRS):
	@$(MAKE) -e -C $@
