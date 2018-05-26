#!/bin/python

import os
import argparse
import subprocess

import serial

CC = "avr-gcc"
CFLAGS = "-std=gnu99 -mmcu=atmega32m1 -Os -mcall-prologues"
MCU = "m32m1"
PROG = "stk500"

INCLUDES = "-I./include/"
LIB = "-L./lib/ -lspi -lcan -luart"

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test harness")
    parser.add_argument('-p', '--port', required=True)
    parser.add_argument('-d', '--test-dir', required=True)

    args = parser.parse_args()

    test_path = args.test_dir
    port = args.port

    for path, _, files in os.walk(test_path):
        if path == test_path:
            continue

        # compile and link 32m1 program using avr-gcc
        compile_cmd = " ".join([CC, CFLAGS, "-o " + path + "/main.o", "-c " + path + "/main.c", INCLUDES])
        print(compile_cmd)
        subprocess.call(compile_cmd, shell=True)
        link_cmd = " ".join([CC, CFLAGS, "-o " + path + "/test.elf", path + "/main.o", LIB])
        subprocess.call(link_cmd, shell=True)
        # create an Intel hex executable
        objcpy_cmd = " ".join(["avr-objcopy", "-j .text", "-j .data", "-O ihex",
                path + "/test.elf", path + "/test.hex"])
        subprocess.call(objcpy_cmd, shell=True)
        # upload prog to connected 32m1 using avr-dude, capture output
        upload_cmd = " ".join(["avrdude", "-p", MCU, "-c", PROG, "-P",
                str(port), "-U flash:w:" + path + "/test.hex"])
        subprocess.call(upload_cmd, shell=True)

        # collect EEPROM memory contents
        eeprom_fetch = " ".join(["avrdude", "-p", MCU, "-c", PROG, "-P",
                str(port), "-U eeprom:r:" + path + "/eeprom.bin:r"]);
        subprocess.call(eeprom_fetch, shell=True)


