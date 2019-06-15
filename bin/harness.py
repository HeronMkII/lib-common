# Use the following command to run the harness from the lib-common local directory:
# python ./bin/harness.py -p <Programming port> -u <UART port> -d tests

# When the uart port (-u option) is not specified, the program guesses using uart_offset()
# On Mac, port numbers can be found by using the command 'ls /dev/tty.*' in terminal

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

harness_description = ("This test harness runs tests built using the libtest " +
        "framework.")

sep = 80*"-"

class TestHarness:
    # Baud Rate specified by 32m1 data sheet
    baud_rate = 9600

    # port and serial_port are passed in as port, uart (see code at bottom)
    def __init__(self, port, serial_port):
        self.port = port
        self.serial_port = serial_port
        self.serial = []
        self.suites = []
        self.total_passed = 0
        self.total_failed = 0
        self.timeout = 10

    # Adds suites to suite class variable in TestHarness object
    def add_suite(self, suite):
        self.suites.append(suite)

    # Asks user for permission to run test suite (wait for correct shell input to continue)
    def has_permission(self, suite):
        print("WARNING: Disconnect programmer TX pin from board RX/SCK pin " +
            "or set board switch to PROG.")
        ans = input("Run test suite '%s'? (y/n) " % suite.name)
        if ans == "y":
            return True
        elif ans == "n":
            return False
        else:
            return self.has_permission(suite)

    # returns count from harness
    # checks for appropriate input (ok)
    def recv_count(self, suite):
        ok = input("Connect programmer TX pin to board RX/SCK pin or set board switch to RUN. (press enter) ")

        # prints '-' 80 times
        print(sep)
        # Assigns serial ports to self.serial and specifies baud rate
        # Added timeout member, see if this resolves blocking and freezing issues
        # Figure out how to prevent this from failing and crashing program (if it does)
        self.serial = [serial.Serial(self.serial_port[i], self.baud_rate, timeout = self.timeout)
            for i in range(suite.boards)]
        # Write data to each port
        for ser in self.serial:
            ser.write(b"COUNT\r\n")
        # strip method removes white space when called with no parameters
        # readline reads an entire line from self.serial[0] (self.serial_port[i])
        # and a trailing '\n' (new line) is kept in the string (but removed by strip method)
        count = int(self.serial[0].readline().strip().decode('utf-8',errors='ignore'))

        return count

    # Writes 'START' to serial port
    def send_start(self):
        self.serial[0].write(b"START\r\n")

    # Closes serial ports when tests are complete
    def send_end(self):
        for ser in self.serial[1:]:
            ser.write(b"KILL\r\n")
        for ser in self.serial:
            ser.close()
        self.serial = []

    # Runs tests for each suite member
    def run_harness(self):
        # Call "make" in the lib-common root folder
        # Calls cmd using shell
        print("    Compiling libraries...")
        subprocess.call("make", shell=True)
        print()
        for suite in self.suites:
            suite.run_suite()

    # Update total tests passed and failed
    def update_stats(self, passed, failed):
        self.total_passed += passed
        self.total_failed += failed

    # Prints results
    def print_summary(self):
        total = self.total_passed + self.total_failed
        if total > 0:
            print("Testing summary:")
            print("    Total passed: %d / %d" % (self.total_passed, total))
            print("    Total failed: %d / %d" % (self.total_failed, total))

class TestSuite:
    def __init__(self, path, boards, harness):
        self.path = path
        self.boards = boards
        self.name = os.path.basename(path)
        self.harness = harness
        self.tests = []
        self.passed = 0
        self.failed = 0

    # Compiles code for each board
    def compile_upload(self):
        print("    Compiling and uploading program...")

        for i in range(1, self.boards + 1):
            # Call "make upload" in the specific test suite's directory
            cmd = " ".join(["make", "upload", "-C", self.path,
                "PROG=main" + str(i), "PORT=" + self.harness.port[i - 1]])
            # Calls cmd using shell
            subprocess.call(cmd, shell=True)

    def run_suite(self):
        # Upon getting permission from user, compile, link, copy and upload code to 32m1
        if self.harness.has_permission(self):
            self.compile_upload()

            # Gets count of tests to be run, then appends it
            count = self.harness.recv_count(self)
            for _ in range(count):
                self.tests.append(Test())

            # Returns current time (i.e. start time)
            s = time.time()

            # Non-master input is not supported on Windows. Thus Windows does
            # not support multi-board testing.
            for test in self.tests:
                self.harness.send_start()
                while not test.is_done():
                    if os.name == 'posix':
                        # wait until serial port is ready for reading, or until timeout occurs
                        readable, _, _ = select.select(self.harness.serial,
                            [], [])
                        for i in range(self.boards):
                            if self.harness.serial[i] in readable:
                                # reads line from port
                                # ignores errors, or else unexpected results can occur
                                # In the majority of cases, encoding errors do not affect the test running,
                                # so we can (mostly) safely ignore them
                                line = self.harness.serial[i].readline().decode("utf-8", errors='ignore')
                                test.handle_line(line)
                    elif os.name == 'nt':
                        line = self.harness.serial[0].readline().decode("utf-8", errors='ignore')
                        test.handle_line(line)

            e = time.time()

            passed = len(list(filter(lambda x: x.passed(), self.tests)))
            failed = len(self.tests) - passed
            total = passed + failed

            print("Test suite '%s' complete" % self.name)
            print("    Time elapsed: %.3f s" % (e - s))
            print("    Passed: %d / %d" % (passed, total))
            print("    Failed: %d / %d" % (failed, total))
            print(sep)

            # Update global test passes and failures
            self.harness.update_stats(passed, failed)
            # Closes serial port
            self.harness.send_end()

class Test:
    def __init__(self):
        self.name = "Unknown"
        self.assert_passed = 0
        self.assert_failed = 0
        self.time_cb = lambda *x: None
        self.done = False
        self.s = time.time()
        self.expected_max = None
        self.expected_min = None 

    # Checks to see if test is done (would be set by handle_line)
    def is_done(self):
        return self.done

    def passed(self):
        if self.assert_failed == 0:
            return True
        else:
            return False

    def handle_line(self, line):
        # Print the raw characters received
        # print("UART RX (%d characters)" % len(line))
        print("UART RX:", line.strip())

        if line == "DONE\r\n":
            
            if (enable_time == True):
                e = time.time()
                elapsed = e - self.s
                if abs(elapsed - self.expected_max) >= 10e-2:
                    self.assert_failed += 1
                    print("    Error: " +
                          "expected test to complete in %.3f s, took %.3f s"
                          % (self.expected_max, elapsed))
                else if abs(self.expected_min - elapsed) >= 10e-2:
                    self.assert_failed += 1
                    print("    Error: " +
                          "expected test to take at least %.3f s, took %.3f s"
                          % (self.expected_min, elapsed))
                else:
                    self.assert_passed += 1
                    
            self.done = True
            if self.assert_failed > 0:
                print("Test complete - Failed")
            else:
                print("Test complete - Passed")
            print(sep)
        # Handle different cases based on input read
        elif line[:9] == "TEST NAME":
            self.handle_name(line)
        elif line[:4] == "TIME":
            if (enable_time == True) self.handle_time(line)
        elif line.startswith("AS STR EQ"):
            self.handle_assert_two_strings(line)
        elif line.startswith("AS EQ"):
            self.handle_assert_two_nums(line)
        elif line.startswith("AS NEQ"):
            self.handle_assert_two_nums(line)
        elif line.startswith("AS GT"):
            self.handle_assert_two_nums(line)
        elif line.startswith("AS LT"):
            self.handle_assert_two_nums(line)
        elif line.startswith("AS TRUE"):
            self.handle_assert_true(line)
        elif line.startswith("AS FALSE"):
            self.handle_assert_false(line)
        elif line.startswith("AS FP EQ"):
            self.handle_assert_two_float_nums(line)
        elif line.startswith("AS FP NEQ"):
            self.handle_assert_two_float_nums(line)
        elif line.startswith("AS FP GT"):
            self.handle_assert_two_float_nums(line)
        elif line.startswith("AS FP LT"):
            self.handle_assert_two_float_nums(line)
        else:
            # Execute line in code if no other conditions are true
            sys.stdout.write(line)

    # Searches for match anywhere in string and returns first subgroup
    def handle_name(self, line):
        regex = r"TEST NAME (.+)\r\n"
        match = re.search(regex, line)
        name = str(match.group(1))
        self.name = name
        print("Test: %s" % name)

    # Calculate elapsed time, if necessary
    def handle_time(self, line):
        regex = r"TIME MIN ([-+]?\d*\.\d+|\d+) MAX ([-+]?\d*\.\d+|\d+)\r\n"
        match = re.search(regex, line)
        # In some cases, random data is output here, use try/except
        # to ensure that the test harness does not error out
        try:
            expected_min = float(match.group(1))
            expected_max = float(match.group(2))
            if (expected_min == 0 || expected_max == 0):
                print("    Error: " +
                          "either expected_min or expected_max is 0 which it shouldn't be")
                return
        except:
            return
        else:
            self.expected_min = expected_min
            self.expected_max = expected_max

    # Extracts line for assertion with two string inputs
    # Prints out error message if it fails
    def handle_assert_two_strings(self, line):
        regex = r"AS STR EQ (\w+) (\w+) \((.+)\) \((.+)\)\r\n"
        match = re.search(regex, line)
        a, b = str(match.group(1)), str(match.group(2))

        if a == b:
            self.assert_passed += 1
        else:
            self.assert_failed += 1
            fn, line = str(match.group(3)), int(match.group(4))
            print("In function '%s', line %d" % (fn, line))
            print("    Error: ASSERT_FALSE failed")

    # Extracts line for assertion with two integer inputs
    # Prints out error message if it fails
    def handle_assert_two_nums(self, line):
        regex = r"AS (\w+) (\d+) (\d+) \((.+)\) \((.+)\)\r\n"
        match = re.search(regex, line)
        operation = str(match.group(1)) # Type of assertion
        a, b = int(match.group(2)), int(match.group(3))
        failure = False # Flag for if assertion failed

        # Passes if both sides are equivalent
        if operation == "EQ":
            if a == b:
                self.assert_passed += 1
            else:
                failure = True
        # Passes if both sides are not equivalent
        elif operation == "NEQ":
            if a != b:
                self.assert_passed += 1
            else:
                failure = True
        # Passes if first greater than second number
        elif operation == "GT":
            if a > b:
                self.assert_passed += 1
            else:
                failure = True
        # Passes if first less than second number
        elif operation == "LT":
            if a < b:
                self.assert_passed += 1
            else:
                failure = True

        # Printing failure message
        if failure == True:
            self.assert_failed += 1
            fn, line = str(match.group(4)), int(match.group(5))
            print("In function '%s', line %d" % (fn, line))
            print("    Error: ASSERT_%s failed" % (operation))

    # Extracts line for assertion with two float inputs
    # Prints out error message if it fails
    # Accurate to 3 decimal places. Code will round for the 6th decimal place
    # Note: If program fails here, check to make sure that function name is not long
    def handle_assert_two_float_nums(self, line):
        regex = r"AS FP (\w+) (-?\d+\.\d+) (-?\d+\.\d+) \((.+)\) \((.+)\)\r\n"

        match = re.search(regex, line)
        operation = str(match.group(1)) # Type of assertion
        a, b = float(match.group(2)), float(match.group(3))
        a = "%.3f" % a # Truncating to three decimal places
        b = "%.3f" % b
        failure = False # Flag for if assertion failed
        # Passes if both sides are equivalent
        if operation == "EQ":
            if a == b:
                self.assert_passed += 1
            else:
                failure = True
        # Passes if both sides are not equivalent
        elif operation == "NEQ":
            if a != b:
                self.assert_passed += 1
            else:
                failure = True
        # Passes if first greater than second number
        elif operation == "GT":
            if a > b:
                self.assert_passed += 1
            else:
                failure = True
        # Passes if first less than second number
        elif operation == "LT":
            if a < b:
                self.assert_passed += 1
            else:
                failure = True

        # Printing failure message
        if failure == True:
            self.assert_failed += 1
            fn, line = str(match.group(4)), int(match.group(5))
            print("In function '%s', line %d" % (fn, line))
            print("    Error: ASSERT_FP_%s failed" % (operation))

    # Extracts line and passes if it evaluates to true
    # Prints out error message if it fails
    def handle_assert_true(self, line):
        regex = r"AS TRUE (\d+) \((.+)\) \((.+)\)\r\n"
        match = re.search(regex, line)
        v = int(match.group(1))
        if v != 0:
            self.assert_passed += 1
        else:
            self.assert_failed += 1
            fn, line = str(match.group(2)), int(match.group(3))
            print("In function '%s', line %d" % (fn, line))
            print("    Error: ASSERT_TRUE failed")

    # Extracts line and passes if it evaluates to false, else fails and prints error
    def handle_assert_false(self, line):
        regex = r"AS FALSE (\d+) \((.+)\) \((.+)\)\r\n"
        match = re.search(regex, line)
        v = int(match.group(1))
        if v == 0:
            self.assert_passed += 1
        else:
            self.assert_failed += 1
            fn, line = str(match.group(2)), int(match.group(3))
            print("In function '%s', line %d" % (fn, line))
            print("    Error: ASSERT_FALSE failed")

# 'if __name__ == "__main__"' means that these statements will only be executed when run as the main module
# i.e. only runs when called via shell (i.e. terminal, command prompt), not when imported separately
# It has nothing to do with the test file mainx.c naming convention
if __name__ == "__main__":
    # Detects if correct python version is being run
    if sys.version_info[0] == 2:
        print("You are using Python 2. Please update to Python 3 and try again.")
        sys.exit(1)
    elif sys.version_info[0] == 3:
        print("You are running Python 3.")
    else:
        print("Unknown error. Exiting....")
        sys.exit(1)

    print("WARNING: Disconnect CoolTerm so the port is available for testing.")

    # It is necessary for the user to specify the programming port and appropriate directory
    # In most cases, this should be the tests directory
    # The uart port only needs to be specified when it is not able to be inferred from the
    # uart_offset() method
    parser = argparse.ArgumentParser(description=harness_description)
    # Method arguments include (in order), expected shell text, name of that argument (used below),
    # nargs specifies the number of arguments, with '+' inserting arguments of that type into a list
    # required is self-explanatory, metavar assigns a displayed name to each argument when using the help argument
    parser.add_argument('-p', '--prog', nargs='+', required=True,
            metavar=('port1', 'port2'),
            help='list of programming ports')
    parser.add_argument('-u', '--uart', nargs='+', required=True,
            metavar=('uart1', 'uart2'),
            help='list of UART ports')
    parser.add_argument('-d', '--test-dir', required=True,
            metavar='test_dir',
            help='directory in which to search for tests')

    # Converts strings to objects, which are then assigned to variables below
    args = parser.parse_args()
    root_path = "harness_tests"
    test_path = args.test_dir
    port = args.prog
    uart = args.uart

    # Creates TestHarness object
    harness = TestHarness(port, uart)
    # Generates file names in directory specified by test_path (above)
    # Number of boards is initialized at 0, then incremented when os.walk finds
    # mainx.c file
    for path, _, files in os.walk(test_path):
        if path == root_path:
            continue
        boards = 0
        regex = r"main\d.c"
        for f in files:
            if re.search(regex, f):
                boards += 1
        # Does not add more boards if on windows system or if there are not
        # enough programming ports
        if (boards > 1) and (os.name == "nt"):
            print(("Skipping test suite '%s', Windows does not support " +
                "multi-board testing.") % os.path.basename(path))
        elif boards > len(port):
            print("Skipping test suite '%s', requires %d more board(s)."
                % (os.path.basename(path), boards - len(port)))
        else:
            # Instantiates TestSuite object with appropriate boards, file path, and harness defined above
            # Adds newly-created suite to harness
            suite = TestSuite(path, boards, harness)
            harness.add_suite(suite)

    # Runs harness, then prints summary after conclusion of tests
    harness.run_harness()
    harness.print_summary()

    if harness.total_failed > 0:
        sys.exit(1)
    else:
        sys.exit(0)
