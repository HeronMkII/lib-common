# lib-common

Lib-common contains a common set of libraries to be used across all subsystems.
Lib-common currently provides libraries supporting UART, CAN and SPI.

# Prerequisites

Before using this library you must install git, make and the AVR-GCC toolchain.

# Installing and updating this library from another project

To install this project for use in another project, enter the following in the
command line:

```
$ cd path/to/your/repo
$ git submodule add https://github.com/HeronMkII/lib-common
```

This creates a new directory called `lib-common` inside your repository.  This
new directory contains a git tree that is independent of the git tree in your
repository.

It is important **NOT** to modify this directory or its contents.

To pull the latest changes from the lib-common repository, do

```
$ git submodule update --remote lib-common
```

in your project's root directory.

# How to use this library

Libcommon provides three static libraries, `libuart.a`, `libspi.a`, `libcan.a`.
The header files for each library can be found in the `include` directory.

To access these libraries in your code, you must include the appropriate header
files in your program. For example, to access UART printing, you must include
`uart/uart.h` and `uart/log.h` in your code.

```
#include <uart/uart.h>
#include <uart/log.h>

// later on

void foo() {
    print("bar\n");
}
```

You must also modify the flags passed to the compiler to let it know where
to find the library files and the header files. The following example, taken
from the examples directory, demonstrates how this is done. Here, we are
modifying the project `makefile`.

```
INCLUDES = -I../../include
LIB = -L../../lib -luart -lspi -lcan
CFLAGS = -Wall -g -mmcu=atmega32m1 -Os -mcall-prologues

PGMR = stk500
MCU = m32m1
PORT = /dev/tty.usbmodem00187462

uart_echo: uart_echo.o
	$(CC) $(CFLAGS) -o $@.elf $^ $(LIB)
	avr-objcopy -j .text -j .data -O ihex $@.elf $@.hex

uart_echo.o: uart_echo.c
	$(CC) $(CFLAGS) -c uart_echo.c $(INCLUDES)

# ...
```

The important additions are the `INCLUDES` and `LIB` variables above.

The `-I` flag in the `INCLUDES` variable tells the compiler where to look for
the library header files. When compiling your project, your `INCLUDE` variable
will likely be `INCLUDE = -I./lib-common/include` if your makefile is in the
root directory of your project.

The `-L` flag of the `LIB` variable tells the compiler where to look for the
library files. The `-l` flags tell the compiler which libraries to link the
project against. Specifying `-lname` tells the compiler to try to link against
`libname.a`. Suppose you want to use both the SPI and UART libraries.  When
compiling your project, your `LIB` variable will likely be `LIB =
-L./lib-common/lib -lspi -luart` if your makefile is in the root directory of
your project.

# Contributing to this library

This library will be used by all subsystems. If you push changes onto the
master branch, you could inadvertently break other people's code.

Thus, all changes to this library **MUST** come in the form a pull request.
Otherwise, your commits will be reverted. Before your pull request is accepted,
it must be checked by another team member.

To create a pull request, you must create a new branch of the project before
you make changes:

```
$ git branch descriptive-branch-name
$ git checkout descriptive-branch-name
```

The first line creates a new branch called `descriptive-branch-name`, and the
second line switches to that new branch. To view all branches, run `git branch
-l`. To return to the master branch run `git checkout master`. To push your
branch to this repository, run

```
$ git push origin descriptive-branch-name
```

This will allow you to create a pull request from Github.

# References and resources

To learn more about Git, visit https://git-scm.com/. To learn more about
pull requests see https://help.github.com/articles/creating-a-pull-request/.
