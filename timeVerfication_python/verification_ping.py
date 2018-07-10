import serial
ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=9600,
    timeout= 2,
    bytesize=serial.SEVENBITS
)
print(ser.name)         # check which port was really used
ser.write(b'0x50bFF2')     # write a string
print('ping 1 sent')
rcvd_data = ser.read(8)
print(rcvd_data)
if rcvd_data  != '51BFF4\r\n':
    ser.write(b'0x50bFF3')        
    print('ping 2 sent')
else:
    print('verified')
    exit(0)
rcvd_data = ser.read(8)
if rcvd_data != '51BFF4\r\n':
    ser.write(b'0x50bff3')        
    print('ping 3 sent')
else:
    print('verified')
    exit(0)
rcvd_data = ser.read(8)
if rcvd_data != '51BFF4\r\n':
    print("Error: 3 ping messages ignored!")
else:
    print('verified')
    exit(0)
ser.close()             # close port
