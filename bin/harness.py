from __future__ import print_function
import sys

import os
import argparse
import subprocess
import time
import re

import serial

class TestHarness:
    baud_rate = 9600
    sep = 80*"-"

    def __init__(self, port):
        self.port = port
        # hack to get the UART port from the programming port
        (head, tail) = os.path.split(port)
        tail = tail[:-1] + str(int(tail[-1]) + 2)
        self.serial_port = os.path.join(head, tail)
        self.serial = None
        self.suites = []
        self.total_passed = 0
        self.total_failed = 0

    def add_suite(self, suite):
        self.suites.append(suite)

    def run(self):
        for suite in self.suites:
            print("WARNING: The UART TX pin on SCK must be disconnected " +
                "before upload. Ensure the UART TX pin is disconnected " +
                "before proceeding.")
            ans = raw_input("Execute test suite \"%s\"? (y/n) " % suite.name)
            if ans == "y":
                suite.compile()
                suite.link()
                suite.obj_copy()
                suite.upload()

                raw_input("Connect the UART TX pin to SCK. (ok) ")
                print(self.sep)

                self.serial = serial.Serial(self.serial_port, self.baud_rate)

                self.serial.write("COUNT\r\n");
                test_count = int(self.serial.readline().strip())
                suite.update_test_count(test_count)

                for _ in range(test_count):
                    self.serial.write("START\r\n")
                    callback = suite.on_complete()
                    while True:
                        line = self.serial.readline()
                        if line == "DONE\r\n":
                            sys.stdout.write(line)
                            callback()
                            print(self.sep)
                            break
                        elif line[:9] == "ASSERT EQ":
                            self.handle_assert_eq(line, suite)
                        else:
                            sys.stdout.write(line)

                self.serial.close()

    def handle_assert_eq(self, line, suite):
        regex = r"ASSERT EQ (\d+) (\d+)\r\n"
        match = re.search(regex, line);
        a, b = int(match.group(1)), int(match.group(2))
        if a == b:
            suite.passed += 1
        else:
            suite.failed += 1

    def print_summary(self):
        print("Summary: Total passed %d Total failed %d" % (self.total_passed,
            self.total_failed))

class TestSuite:
    # constants
    cc = "avr-gcc"
    cflags = "-std=gnu99 -mmcu=atmega32m1 -Os -mcall-prologues"
    mcu = "m32m1"
    prog = "stk500"
    includes = "-I./include/"
    lib = "-L./lib/ -ltest"

    def __init__(self, path, harness):
        self.path = path
        self.name = os.path.basename(path)
        self.harness = harness
        self.test_count = 0
        self.passed = 0
        self.failed = 0

    def compile(self):
        print("Compiling...")
        cmd = " ".join([self.cc, self.cflags,
            "-o " + self.path + "/main.o",
            "-c " + self.path + "/main.c", self.includes])
        subprocess.call(cmd, shell=True)

    def link(self):
        print("Linking...")
        cmd = " ".join([self.cc, self.cflags,
            "-o " + self.path + "/test.elf",
            self.path + "/main.o",
            self.lib])
        subprocess.call(cmd, shell=True)

    def obj_copy(self):
        cmd = " ".join(["avr-objcopy",
            "-j .text", "-j .data",
            "-O ihex", self.path + "/test.elf",
            self.path + "/test.hex"])
        subprocess.call(cmd, shell=True)

    def upload(self):
        print("Uploading...")
        cmd = " ".join(["avrdude -qq",
            "-p", self.mcu,
            "-c", self.prog,
            "-P", self.harness.port,
            "-U flash:w:" + self.path + "/test.hex"])
        subprocess.call(cmd, shell=True)

    def capture_eeprom(self):
        cmd = " ".join(["avrdude -qq",
            "-p", self.mcu,
            "-c", self.prog,
            "-P", self.harness.port,
            "-U eeprom:r:" + self.path + "/eeprom.bin:r"]);
        subprocess.call(cmd, shell=True)

    def update_test_count(self, count):
        self.test_count = count

    def on_complete(self):
        s = time.time()
        def cb():
            e = time.time()
            print("Time elapsed: %.3f s" % (e - s))
            print("Passed: %d" % self.passed)
            print("Failed: %d" % self.failed)
            self.harness.total_passed += self.passed
            self.harness.total_failed += self.failed
        return cb

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test harness")
    parser.add_argument('-p', '--port', required=True)
    parser.add_argument('-d', '--test-dir', required=True)

    args = parser.parse_args()

    test_path = args.test_dir
    port = args.port

    harness = TestHarness(port)
    for path, _, _ in os.walk(test_path):
        if path == test_path:
            continue
        suite = TestSuite(path, harness)
        harness.add_suite(suite)

    harness.run()
    harness.print_summary()
