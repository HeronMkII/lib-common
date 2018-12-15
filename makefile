# All libraries (subdirectories) in lib-common
# Need to put uart first because other libraries depend on it (otherwise get error of "No rule to make target...")
SRC = $(addprefix src/,uart adc can conversions dac heartbeat pex queue spi stack test timer utilities watchdog)
EXAMPLES = $(dir $(wildcard examples/*/.))
MANUAL_TESTS = $(dir $(wildcard manual_tests/*/.))

export CC = avr-gcc
export AR = avr-ar
export RANLIB = avr-ranlib
export INCLUDES = -I../../include
export CFLAGS = -Wall -std=gnu99 -g -mmcu=atmega32m1 -Os -mcall-prologues


# PORT - Computer port that the programmer is connected to
# Operating system detection based on https://gist.github.com/sighingnow/deee806603ec9274fd47

# Windows
ifeq ($(OS),Windows_NT)
	PORT = $(shell powershell "[System.IO.Ports.SerialPort]::getportnames() | select -First 2 | select -Last 1")

# Unix
else
	# Get the operating system
	UNAME_S := $(shell uname -s)

	# macOS
	ifeq ($(UNAME_S),Darwin)
		# Automatically find the port (lower number)
		PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)
	endif
	# Linux
	ifeq ($(UNAME_S),Linux)
		PORT = $(shell find /dev -name 'ttyS[0-9]*' | sort | head -n1)
	endif
endif

# If automatic port detection fails,
# uncomment one of these lines and change it to set the port manually
# PORT = COM3						# Windows
# PORT = /dev/tty.usbmodem00208212	# macOS
# PORT = /dev/ttyS3					# Linux


# Set the PYTHON variable
ifeq ($(OS),Windows_NT)
	# Windows uses `python` for either Python 2 or 3
	PYTHON = python
else
	# macOS/Linux use `python3` to explicitly use Python 3
	PYTHON = python3
endif


.PHONY: all $(SRC) clean debug examples harness help manual_tests

all: $(SRC)

clean:
	@for dir in $(SRC) ; do \
		$(MAKE) clean -C $$dir ; \
	done

$(SRC):
	@$(MAKE) -e -C $@

# Print debug information
debug:
	@echo ————————————
	@echo $(SRC)
	@echo ————————————
	@echo $(EXAMPLES)
	@echo ————————————
	@echo $(MANUAL_TESTS)
	@echo ————————————

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
	@echo "usage: make [all | clean | debug | examples | harness | help | manual_tests]"
	@echo ""
	@echo "Running make without any arguments is equivalent to running make all."
	@echo ""
	@echo "all            build the lib-common library"
	@echo "clean          clear the build/ directory and all subdirectories"
	@echo "debug          display debugging information"
	@echo "examples       build all examples (see the examples/makefile)"
	@echo "harness        run the test harness"
	@echo "help           display this help message"
	@echo "manual_tests   build all manual tests (see the manual_tests/makefile)"

manual_tests:
	@for dir in $(MANUAL_TESTS) ; do \
		cd $$dir ; \
		make clean ; \
		make ; \
		cd ../.. ; \
	done
