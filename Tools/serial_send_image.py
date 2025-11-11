# Tools/python3 serial_send_image.py

import serial
import time
import os

PORT = "/dev/ttyUSB0"  # Correct the port name; remove extra descriptor
BAUD = 115200

# Get the path to this script
script_dir = os.path.dirname(os.path.abspath(__file__))

# Build the path to the binary
binary_path = os.path.join(script_dir, "..", "build", "bin", "ha-ctrl_combined_update_image.bin")
binary_path = os.path.normpath(binary_path)

# Open the serial port
ser = serial.Serial(PORT, BAUD, timeout=1)
time.sleep(2)  # allow MCU to reset if needed

# 1. Send the command
command = "fw_update\n"  # often adding newline helps MCU recognize command
ser.write(command.encode())  # send as bytes
print(f"Sent command: {command.strip()}")

# 2. Wait 1 second
time.sleep(1)

# 3. Send the binary file
with open(binary_path, "rb") as f:
    data = f.read()
    print(f"Sending {len(data)} bytes...")
    ser.write(data)

ser.close()
print("Done.")
