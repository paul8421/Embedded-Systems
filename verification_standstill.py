
import serial
ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=9600,
    timeout=2,
    bytesize=serial.SEVENBITS
)


print(ser.name)         # check which port was really used
ser.write(b'0x61024505')     # write a string
for i in range(0, 15):
    rcvd_data = ser.read(48)
    if rcvd_data != "left velocity 0 right velocity 0\r\n":
        print("Error: not staying still")
        exit(0)
print("verified OOB")

