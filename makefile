# All libraries (subdirectories/folders) in lib-common
# Need to put uart first because other libraries depend on it (otherwise get error of "No rule to make target...")
LIBNAMES = uart adc can conversions dac heartbeat pex queue spi stack test timer utilities watchdog
# Subfolders in src folder
SRC = $(addprefix src/,$(LIBNAMES))
# Subfolders in build folder
BUILD = $(addprefix build/,$(LIBNAMES))
# Subfolders in examples folder
EXAMPLES = $(dir $(wildcard examples/*/.))
# Subfolders in manual_tests folder
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

# Echo the text of the command to print out what we're doing before actually
# executing the command (can't echo an individual command in a for loop)
# TODO - calling make in a subdirectory seems to not pass the `clean` argument -
#	find a fix for this
clean:
	@for dir in $(BUILD) ; do \
		echo rm -f $$dir/*.* ; \
		rm -f $$dir/*.* ; \
	done

# Compile each of the source libraries
# -e: "Environment variables override makefiles."
# -C: "Change to DIRECTORY before doing anything."
# $@: The source file being generated
$(SRC):
	@$(MAKE) -e -C $@

# Print debug information
debug:
	@echo ------------
	@echo $(LIBNAMES)
	@echo ------------
	@echo $(SRC)
	@echo ------------
	@echo $(EXAMPLES)
	@echo ------------
	@echo $(MANUAL_TESTS)
	@echo ------------

# For each example, clean directory then build
# TODO - call `make clean` within each subdirectory
examples:
	@for dir in $(EXAMPLES) ; do \
		cd $$dir ; \
		rm -f ./*.o ; \
		rm -f ./*.elf ; \
		rm -f ./*.hex ; \
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
	@echo "clean          clear the build directory and all subdirectories"
	@echo "debug          display debugging information"
	@echo "examples       build all examples (see examples/makefile)"
	@echo "harness        run the test harness"
	@echo "help           display this help message"
	@echo "manual_tests   build all manual tests (see manual_tests/makefile)"

# For each example, clean directory then build
# TODO - call `make clean` within each subdirectory
manual_tests:
	@for dir in $(MANUAL_TESTS) ; do \
		cd $$dir ; \
		rm -f ./*.o ; \
		rm -f ./*.elf ; \
		rm -f ./*.hex ; \
		make ; \
		cd ../.. ; \
	done
