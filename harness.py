from __future__ import print_function
import sys

import os
import argparse
import subprocess
import time

import serial

CC = "avr-gcc"
CFLAGS = "-std=gnu99 -mmcu=atmega32m1 -Os -mcall-prologues"
MCU = "m32m1"
PROG = "stk500"
INCLUDES = "-I./include/"
LIB = "-L./lib/ -ltest"

def compile(path):
    cmd = " ".join([CC, CFLAGS, "-o " + path + "/main.o", "-c " + path +
        "/main.c", INCLUDES])
    subprocess.call(cmd, shell=True)

def link(path):
    cmd = " ".join([CC, CFLAGS, "-o " + path + "/test.elf", path + "/main.o",
        LIB])
    subprocess.call(cmd, shell=True)

def obj_copy(path):
    cmd = " ".join(["avr-objcopy", "-j .text", "-j .data", "-O ihex", path +
        "/test.elf", path + "/test.hex"])
    subprocess.call(cmd, shell=True)

def upload(path, port):
    cmd = " ".join(["avrdude", "-p", MCU, "-c", PROG, "-P", port, "-U flash:w:"
        + path + "/test.hex"])
    subprocess.call(cmd, shell=True)

def capture_eeprom(path, port):
    cmd = " ".join(["avrdude", "-p", MCU, "-c", PROG, "-P", port,
        "-U eeprom:r:" + path + "/eeprom.bin:r"]);
    subprocess.call(cmd, shell=True)

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

        compile(path)
        link(path)
        obj_copy(path)
        upload(path, port)

        # hack to get the UART port from the regular port
        (head, tail) = os.path.split(port)
        tail = tail[:-1] + str(int(tail[-1]) + 2)
        ser_port = os.path.join(head, tail)

        ser = serial.Serial(ser_port, 9600)
        yn = raw_input("Start? (y/n) ")
        if yn == "y":
            ser.write("COUNT\r\n");
            num = int(ser.readline().strip())
            for i in range(num):
                ser.write("START\r\n");
                while True:
                    line = ser.readline()
                    if line == "DONE\r\n":
                        sys.stdout.write(line)
                        break
                    else:
                        sys.stdout.write(line)
        ser.close()
