#!/usr/bin/env python3
import os
os.system("sudo chmod 666 /dev/ttyUSB1")
os.system("litex_term /dev/ttyUSB1 --kernel firmware/firmware.bin")
