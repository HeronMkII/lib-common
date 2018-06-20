from __future__ import print_function
import subprocess
import argparse
import select
import time
import sys
import os
import re

try:
    import serial
except ImportError:
    print("Error: This program requires the pyserial module. To install " +
        "pyserial,\nvisit https://pypi.org/project/pyserial/ or run\n" +
        "    $ pip install pyserial\n" +
        "in the command line.")
    sys.exit(1)

sep = 80*"-"

class TestHarness:
    baud_rate = 9600

    def __init__(self, port):
        self.port = port
        self.serial_port = []
        for p in port:
            (head, tail) = os.path.split(p)
            tail = tail[:-1] + str(int(tail[-1]) + 2)
            self.serial_port.append(os.path.join(head, tail))
        self.serial = []
        self.suites = []
        self.total_passed = 0
        self.total_failed = 0

    def add_suite(self, suite):
        self.suites.append(suite)

    def has_permission(self, suite):
        print("WARNING: The UART TX pin(s) on SCK must be disconnected " +
            "before upload.\nEnsure the UART TX pin on each board " +
            "is disconnected before proceeding.")
        ans = raw_input("Run test suite '%s'? (y/n) " % suite.name)
        if ans == "y":
            return True
        elif ans == "n":
            return False
        else:
            return self.has_permission(suite)

    def recv_count(self, suite):
        # TODO: seperate this into two functions?
        ok = raw_input("Connect the UART TX pin(s) to SCK. (ok) ")
        if ok == "ok":
            print(sep)
            self.serial = [ serial.Serial(self.serial_port[i], self.baud_rate)
                for i in range(suite.boards) ]
            for ser in self.serial:
                ser.write("COUNT\r\n")
            count = int(self.serial[0].readline().strip())
            return count
        else:
            return self.recv_count(suite)

    def send_start(self):
        self.serial[0].write("START\r\n")

    def send_end(self):
        for ser in self.serial[1:]:
            ser.write("KILL\r\n")
        for ser in self.serial:
            ser.close()
        self.serial = []

    def run(self):
        for suite in self.suites:
            suite.run()

    def update_stats(self, passed, failed):
        self.total_passed += passed
        self.total_failed += failed

    def print_summary(self):
        total = self.total_passed + self.total_failed
        if total > 0:
            print("Testing summary:")
            print("    Total passed: %d / %d" % (self.total_passed, total))
            print("    Total failed: %d / %d" % (self.total_failed, total))

class TestSuite:
    # constants
    cc = "avr-gcc"
    cflags = "-std=gnu99 -Wall -Wl,-u,vfprintf -mmcu=atmega32m1 -Os -mcall-prologues"
    mcu = "m32m1"
    prog = "stk500"
    includes = "-I./include/"
    lib = "-L./lib/ -ltest -lspi -luart -lcan -lprintf_flt -lm"

    def __init__(self, path, boards, harness):
        self.path = path
        self.boards = boards
        self.name = os.path.basename(path)
        self.harness = harness
        self.tests = []
        self.passed = 0
        self.failed = 0

    def compile(self):
        print("    Compiling...")
        for i in range(1, self.boards + 1):
            cmd = " ".join([self.cc, self.cflags,
                "-o " + self.path + "/main" + str(i) + ".o",
                "-c " + self.path + "/main" + str(i) + ".c",
                self.includes])
            subprocess.call(cmd, shell=True)

    def link(self):
        print("    Linking...")
        for i in range(1, self.boards + 1):
            cmd = " ".join([self.cc, self.cflags,
                "-o " + self.path + "/test" + str(i) + ".elf",
                self.path + "/main" + str(i) + ".o",
                self.lib])
            subprocess.call(cmd, shell=True)

    def obj_copy(self):
        for i in range(1, self.boards + 1):
            cmd = " ".join(["avr-objcopy",
                "-j .text", "-j .data",
                "-O ihex", self.path + "/test" + str(i) + ".elf",
                self.path + "/test" + str(i) + ".hex"])
            subprocess.call(cmd, shell=True)

    def upload(self):
        print("    Uploading...")
        for i in range(1, self.boards + 1):
            cmd = " ".join(["avrdude -qq",
                "-p", self.mcu,
                "-c", self.prog,
                "-P", self.harness.port[i - 1],
                "-U flash:w:" + self.path + "/test" + str(i) + ".hex"])
            subprocess.call(cmd, shell=True)

    def capture_eeprom(self):
        pass
        #cmd = " ".join(["avrdude -qq",
        #    "-p", self.mcu,
        #    "-c", self.prog,
        #    "-P", self.harness.port,
        #    "-U eeprom:r:" + self.path + "/eeprom.bin:r"])
        #subprocess.call(cmd, shell=True)

    def run(self):
        if self.harness.has_permission(self):
            self.compile()
            self.link()
            self.obj_copy()
            self.upload()

            count = self.harness.recv_count(self)
            for _ in range(count):
                self.tests.append(Test())

            s = time.time()

            for test in self.tests:
                self.harness.send_start()
                while not test.is_done():
                    readable, _, _ = select.select(self.harness.serial,
                        [], [])
                    for i in range(self.boards):
                        if self.harness.serial[i] in readable:
                            line = self.harness.serial[i].readline()
                            test.handle_line(line)

            e = time.time()
            passed = len(filter(lambda x: x.passed(), self.tests))
            failed = len(self.tests) - passed
            total = passed + failed

            print("Test suite '%s' complete" % self.name)
            print("    Time elapsed: %.3f s" % (e - s))
            print("    Passed: %d / %d" % (passed, total))
            print("    Failed: %d / %d" % (failed, total))
            print(sep)

            self.harness.update_stats(passed, failed)
            self.harness.send_end()

class Test:
    def __init__(self):
        self.name = "Unknown"
        self.assert_passed = 0
        self.assert_failed = 0
        self.time_cb = lambda *x: None
        self.done = False

    def is_done(self):
        return self.done

    def passed(self):
        if self.assert_failed == 0:
            return True
        else:
            return False

    def handle_line(self, line):
        if line == "DONE\r\n":
            (self.time_cb)()
            self.done = True
            if self.assert_failed > 0:
                print("Test complete - Failed")
            else:
                print("Test complete - Passed")
            print(sep)
        elif line[:9] == "TEST NAME":
            self.handle_name(line)
        elif line[:4] == "TIME":
            self.handle_time(line)
        elif line[:9] == "ASSERT EQ":
            self.handle_assert_eq(line)
        elif line[:11] == "ASSERT TRUE":
            self.handle_assert_true(line)
        else:
            sys.stdout.write(line)

    def handle_name(self, line):
        regex = r"TEST NAME (.+)\r\n"
        match = re.search(regex, line)
        name = str(match.group(1))
        self.name = name
        print("Test: %s" % name)

    def handle_time(self, line):
        regex = r"TIME ([-+]?\d*\.\d+|\d+)\r\n"
        match = re.search(regex, line)
        expected = float(match.group(1))
        if expected == 0:
            return
        else:
            s = time.time()
            def fn():
                e = time.time()
                elapsed = e - s
                if abs(elapsed - expected) >= 10e-2:
                    self.assert_failed += 1
                    print("    Error: " +
                        "expected test to complete in %.3f s, took %.3f s"
                        % (expected, elapsed))
                else:
                    self.assert_passed += 1
            self.time_cb = fn

    def handle_assert_eq(self, line):
        regex = r"ASSERT EQ (\d+) (\d+) \((.+)\) \((.+)\)\r\n"
        match = re.search(regex, line)
        a, b = int(match.group(1)), int(match.group(2))
        if a == b:
            self.assert_passed += 1
        else:
            self.assert_failed += 1
            fn, line = str(match.group(3)), int(match.group(4))
            print("In function '%s', line %d" % (fn, line))
            print("    Error: ASSERT_EQ failed")

    def handle_assert_true(self, line):
        regex = r"ASSERT TRUE (\d+) \((.+)\) \((.+)\)\r\n"
        match = re.search(regex, line)
        v = int(match.group(1))
        if v != 0:
            self.assert_passed += 1
        else:
            self.assert_failed += 1
            fn, line = str(match.group(2)), int(match.group(3))
            print("In function '%s', line %d" % (fn, line))
            print("    Error: ASSERT_TRUE failed")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Test harness")
    parser.add_argument('-p', '--port', nargs='+', required=True)
    parser.add_argument('-d', '--test-dir', required=True)

    args = parser.parse_args()

    test_path = args.test_dir
    port = args.port

    harness = TestHarness(port)
    for path, _, files in os.walk(test_path):
        if path == test_path:
            continue
        boards = 0
        regex = r"main\d.c"
        for f in files:
            if re.search(regex, f):
                boards += 1

        if boards > len(port):
            print("Skipping test suite '%s', requires %d more board(s)."
                % (os.path.basename(path), boards - len(port)))
        else:
            suite = TestSuite(path, boards, harness)
            harness.add_suite(suite)

    harness.run()
    harness.print_summary()

    if harness.total_failed > 0:
        sys.exit(1)
    else:
        sys.exit(0)
