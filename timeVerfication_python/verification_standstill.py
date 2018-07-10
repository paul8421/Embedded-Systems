
import serial
ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=9600,
    timeout=1.1,
    bytesize=serial.SEVENBITS
)


print(ser.name)         # check which port was really used
ser.write(b'0x61024505')     # write a string
rcvd_data = ser.read(41)
print(rcvd_data)
for i in range(0, 29):
    rcvd_data = ser.read(41)
    print(rcvd_data)
    if 'speed 0' not in str(rcvd_data) : #!= "left motor speed 0 \nright motor speed 0\r\n":
        print("Error: not staying still")
        exit(0)
print("verified OOB")

print(rcvd_data)
ser.close()
