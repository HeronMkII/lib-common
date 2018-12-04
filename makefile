SUBDIRS = $(addprefix src/,adc can conversions dac heartbeat pex queue spi stack test timer uart utilities watchdog)
EXAMPLES = $(dir $(wildcard examples/*/.))

.PHONY: all $(SUBDIRS) clean examples harness help

export CC = avr-gcc
export AR = avr-ar
export RANLIB = avr-ranlib
export INCLUDES = -I../../include
export CFLAGS = -Wall -std=gnu99 -g -mmcu=atmega32m1 -Os -mcall-prologues

# Automatically detect the programmer port and set the PYTHON variable
ifeq ($(OS),Windows_NT)
	# One programmer should give 2 ports (either COM3 and COM4 or COM5 and COM6)
	# Programming port is the higher number (COM4 or COM6)
	# Use powershell, list port names, get the second number in the list
	PORT = $(shell powershell "[System.IO.Ports.SerialPort]::getportnames() | select -First 2 | select -Last 1")
	# Windows uses `python` for either Python 2 or 3
	PYTHON = python
else
	PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)
	# macOS/Linux use `python3` to explicitly use Python 3
	PYTHON = python3
endif

all: $(SUBDIRS)

clean:
	@for dir in $(SUBDIRS) ; do \
		$(MAKE) clean -C $$dir ; \
	done

$(SUBDIRS):
	@$(MAKE) -e -C $@

examples:
	@for dir in $(EXAMPLES) ; do \
		cd $$dir ; \
		make clean ; \
		make ; \
		cd ../.. ; \
	done

harness:
	$(PYTHON) ./bin/harness.py -p $(PORT) -d harness_tests

help:
	@echo "usage: make [all | clean | examples | harness | help]"
	@echo ""
	@echo "Running make without any arguments is equivalent to running make all."
	@echo ""
	@echo "all            build the lib-common library"
	@echo "clean          clear the build/ directory and all subdirectories"
	@echo "examples       build all examples (see the examples/makefile)"
	@echo "harness          run the test harness"
	@echo "help           display this help message"
