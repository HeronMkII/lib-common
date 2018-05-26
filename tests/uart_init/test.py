import serial

with serial.Serial('/dev/tty.usbmodem00208214', 9600, timeout=1) as ser:
    while True:
        print(ser.readline());
