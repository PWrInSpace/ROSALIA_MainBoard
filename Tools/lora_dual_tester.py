import serial
import pandas as pd
import serial.tools.list_ports

ROSALIA_BAUD = 115200
GS_BAUD = 0

def show_serial_ports() -> list:
    ports = serial.tools.list_ports.comports()
    prt = []
    for i, (port, desc, hwid) in enumerate(sorted(ports)):
        print("Number {}: {} {} [{}]".format(i ,port, desc, hwid))
        prt.append(port)
    return prt

available_ports = show_serial_ports()
# print(available_ports)

rosalia_num = int(input('Insert the ROSALIA serial port: '))
gs_num = int(input('Insert the ground station serial port: '))

rosalia_serial = serial.Serial(available_ports[rosalia_num], ROSALIA_BAUD)
gs_serial = serial.Serial(available_ports[gs_num], GS_BAUD)

