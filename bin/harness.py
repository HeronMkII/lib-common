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
                "before upload.\nEnsure the UART TX pin is disconnected " +
                "before proceeding.")
            ans = raw_input("Execute test suite '%s'? (y/n) " % suite.name)
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
                on_end = suite.on_end()

                for _ in range(test_count):
                    self.serial.write("START\r\n")
                    while True:
                        line = self.serial.readline()
                        if line == "DONE\r\n":
                            print("Test complete")
                            print(self.sep)
                            break
                        elif line[:9] == "TEST NAME":
                            self.handle_test_name(line, suite)
                        elif line[:9] == "ASSERT EQ":
                            self.handle_assert_eq(line, suite)
                        elif line[:11] == "ASSERT TRUE":
                            self.handle_assert_true(line, suite)
                        else:
                            sys.stdout.write(line)

                on_end()
                print(self.sep)
                self.serial.close()

    def handle_assert_eq(self, line, suite):
        regex = r"ASSERT EQ (\d+) (\d+) \((.+)\) \((.+)\)\r\n"
        match = re.search(regex, line)
        a, b = int(match.group(1)), int(match.group(2))
        if a == b:
            suite.passed += 1
        else:
            suite.failed += 1
            fn, line = str(match.group(3)), int(match.group(4))
            print("In function '%s', line %d" % (fn, line))
            print("    Error: ASSERT_EQ failed")

    def handle_assert_true(self, line, suite):
        regex = r"ASSERT TRUE (\d+) \((.+)\) \((.+)\)\r\n"
        match = re.search(regex, line)
        v = int(match.group(1))
        if v != 0:
            suite.passed += 1
        else:
            suite.failed += 1
            fn, line = str(match.group(2)), int(match.group(3))
            print("In function '%s', line %d" % (fn, line))
            print("    Error: ASSERT_TRUE failed")

    def handle_test_name(self, line, suite):
        regex = r"TEST NAME (.+)\r\n"
        match = re.search(regex, line)
        name = str(match.group(1))
        print("Test: %s" % name)

    def print_summary(self):
        total = self.total_passed + self.total_failed
        print("Summary:")
        print("    Total passed %d / %d" % (self.total_passed, total))
        print("    Total failed %d / %d" % (self.total_failed, total))

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
        print("    Compiling...")
        cmd = " ".join([self.cc, self.cflags,
            "-o " + self.path + "/main.o",
            "-c " + self.path + "/main.c", self.includes])
        subprocess.call(cmd, shell=True)

    def link(self):
        print("    Linking...")
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
        print("    Uploading...")
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

    def on_end(self):
        s = time.time()
        def fn():
            total = self.passed + self.failed
            e = time.time()
            print("Test suite '%s' complete" % self.name)
            print("    Time elapsed: %.3f s" % (e - s))
            print("    Passed: %d / %d" % (self.passed, total))
            print("    Failed: %d / %d" % (self.failed, total))
            self.harness.total_passed += self.passed
            self.harness.total_failed += self.failed
        return fn

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
