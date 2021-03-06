# All libraries (subdirectories/folders) in lib-common
# Need to put uart first because other libraries depend on it (otherwise get error of "No rule to make target...")
LIBNAMES = uart adc can conversions dac heartbeat pex queue spi stack test timer uptime utilities watchdog
# Subfolders in src folder
SRC = $(addprefix src/,$(LIBNAMES))
# Subfolders in build folder
BUILD = $(addprefix build/,$(LIBNAMES))
# Subfolders in examples folder
EXAMPLES = $(dir $(wildcard examples/*/.))
# Subfolders in manual_tests folder
MANUAL_TESTS = $(dir $(wildcard manual_tests/*/.))
# Microcontroller - "32m1" or "64m1"
MCU = 64m1
# AVR device for avrdude uploading - must be prefixed with "m"
DEVICE = m$(MCU)
# Harness testing folder
TEST = harness_tests
# HARNESS_ARGS - can specify from the command line when calling `make`

export CC = avr-gcc
export AR = avr-ar
export RANLIB = avr-ranlib
export INCLUDES = -I../../include
export CFLAGS = -Wall -std=gnu99 -g -mmcu=atmega$(MCU) -Os -mcall-prologues


# Detect operating system - based on https://gist.github.com/sighingnow/deee806603ec9274fd47

# One of these flags will be set to true based on the operating system
WINDOWS := false
MAC_OS := false
LINUX := false

ifeq ($(OS),Windows_NT)
	WINDOWS := true
else
	# Unix - get the operating system
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		MAC_OS := true
	endif
	ifeq ($(UNAME_S),Linux)
		LINUX := true
	endif
endif

# PORT - Computer port that the programmer is connected to
# Try to automatically detect the port
ifeq ($(WINDOWS), true)
	# higher number
	PORT = $(shell powershell "[System.IO.Ports.SerialPort]::getportnames() | sort | select -First 2 | select -Last 1")
	UART = $(shell powershell "[System.IO.Ports.SerialPort]::getportnames() | sort | select -First 1")
endif
ifeq ($(MAC_OS), true)
	# lower number
	PORT = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | head -n1)
	UART = $(shell find /dev -name 'tty.usbmodem[0-9]*' | sort | sed -n 2p)
endif
ifeq ($(LINUX), true)
	# lower number
	PORT = $(shell find /dev -name 'ttyS[0-9]*' | sort | head -n1)
	UART = $(shell find /dev -name 'ttyS[0-9]*' | sort | sed -n 2p)
endif

# Set the PYTHON variable - Python interpreter
# Windows uses `python` for either Python 2 or 3,
# while macOS/Linux use `python3` to explicitly use Python 3
ifeq ($(WINDOWS), true)
	PYTHON := python
endif
ifeq ($(MAC_OS), true)
	PYTHON := python3
endif
ifeq ($(LINUX), true)
	PYTHON := python3
endif


.PHONY: all $(SRC) clean debug examples harness help manual_tests read-eeprom

all: $(SRC)

# Remove all files in the `lib` directory and each of the `build` subdirectories
# Calling `make clean` in each subdirectory on Windows does not properly pass
# the `clean` argument, so manually remove the files
clean:
	rm -f lib/*.*
	@echo "Removing files in build"
	@for dir in $(BUILD) ; do \
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
	@echo $(BUILD)
	@echo ------------
	@echo $(EXAMPLES)
	@echo ------------
	@echo $(MANUAL_TESTS)
	@echo ------------
	@echo $(MCU)
	@echo ------------
	@echo $(DEVICE)
	@echo ------------
	@echo $(TEST)
	@echo ------------
	@echo $(WINDOWS)
	@echo ------------
	@echo $(MAC_OS)
	@echo ------------
	@echo $(LINUX)
	@echo ------------
	@echo $(PORT)
	@echo ------------
	@echo $(PYTHON)
	@echo ------------
	@echo $(HARNESS_ARGS)
	@echo ------------

# For each example program, clean directory then build
# Calling `make clean` in each subdirectory on Windows does not properly pass
# the `clean` argument, so manually remove the files
examples:
	@for dir in $(EXAMPLES) ; do \
		cd $$dir ; \
		rm -f ./*.o ; \
		rm -f ./*.elf ; \
		rm -f ./*.hex ; \
		make ; \
		cd ../.. ; \
	done

# If multi-board testing, PORT2 and UART2 must both be specified
harness:
	$(PYTHON) ./bin/harness.py -m $(MCU) -p $(PORT) $(PORT2) -u $(UART) $(UART2) -d $(TEST) $(HARNESS_ARGS)

help:
	@echo "usage: make [all | clean | debug | examples | harness | help | manual_tests | read-eeprom]"
	@echo "Running make without any arguments is equivalent to running make all."
	@echo "all            build the lib-common library"
	@echo "clean          clear the build directory and all subdirectories"
	@echo "debug          display debugging information"
	@echo "examples       build all examples (see examples/makefile)"
	@echo "harness        run the test harness"
	@echo "help           display this help message"
	@echo "manual_tests   build all manual tests (see manual_tests/makefile)"
	@echo "read-eeprom    read and display the contents of the microcontroller's EEPROM"

# For each manual test program, clean directory then build
# Calling `make clean` in each subdirectory on Windows does not properly pass
# the `clean` argument, so manually remove the files
manual_tests:
	@for dir in $(MANUAL_TESTS) ; do \
		cd $$dir ; \
		rm -f ./*.o ; \
		rm -f ./*.elf ; \
		rm -f ./*.hex ; \
		make ; \
		cd ../.. ; \
	done

# Create a file called eeprom.bin, which contains a raw binary copy of the micro's EEPROM memory.
# View the contents of the binary file in hex
read-eeprom:
	@echo "Reading EEPROM to binary file eeprom.bin..."
	avrdude -p $(DEVICE) -c stk500 -P $(PORT) -U eeprom:r:eeprom.bin:r
	@echo "Displaying eeprom.bin in hex..."
ifeq ($(WINDOWS), true)
	powershell Format-Hex eeprom.bin
else
	hexdump eeprom.bin
endif
