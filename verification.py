import serial
__init__(port = '/dev/ttyUSB1', baudrate=9600, bytesize=EIGHTBITS, parity=PARITY_NONE, stopbits=STOPBITS_ONE, timeout=100)//read timeout set to 100ms

ser = serial.Serial('/dev/ttyUSB1')  # open serial port
print(ser.name)         # check which port was really used
ser.write(b'0x50bff3')     # write a string
rcvd_data = ser.read(48)
if rcvd_data is not '0x51bff4':
    ser.write(b'0x50bff3')        
else:
    print('verified')
rcvd_data = ser.read(48)
if rcvd_data is not '0x51bff4':
    ser.write(b'0x50bff3')        
else:
    print('verified')
rcvd_data = ser.read(48)
if rcvd_data is not '0x51bff4':
   print("Error: 3 ping messages ignored!")
else:
    print('verified')

ser.close()             # close port
