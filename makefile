SUBDIRS = $(addprefix src/,uart spi can timer queue stack heartbeat test)
EXAMPLES = $(dir $(wildcard examples/*/.))

.PHONY: all $(SUBDIRS) clean examples tests help

export CC = avr-gcc
export AR = avr-ar
export RANLIB = avr-ranlib
export INCLUDES = -I../../include
export CFLAGS = -Wall -std=gnu99 -g -mmcu=atmega32m1 -Os -mcall-prologues

ifeq ($(OS),Windows_NT)
	PORT = COM3
else
	PORT = /dev/tty.usbmodem00187462
	#PORT = /dev/tty.usbmodem00208212
endif

all: $(SUBDIRS)

clean:
	@$(MAKE) clean -C src/uart
	@$(MAKE) clean -C src/spi
	@$(MAKE) clean -C src/can
	@$(MAKE) clean -C src/timer
	@$(MAKE) clean -C src/queue
	@$(MAKE) clean -C src/stack
	@$(MAKE) clean -C src/heartbeat
	@$(MAKE) clean -C src/test

$(SUBDIRS):
	@$(MAKE) -e -C $@

examples:
	@for dir in $(EXAMPLES) ; do \
		cd $$dir ; \
		make clean ; \
		make ; \
		cd ../.. ; \
	done

tests:
	./bin/harness.py -p $(PORT) -d tests

help:
	@echo "usage: make [all | clean | examples | tests | help]"
	@echo ""
	@echo "Running make without any arguments is equivalent to running make all."
	@echo ""
	@echo "all            build the lib-common library"
	@echo "clean          clear the build/ directory and all subdirectories"
	@echo "examples       build all examples (see the examples/makefile)"
	@echo "tests          run the test harness"
	@echo "help           display this help message"
