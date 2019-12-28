import serial
import datetime

packetLength = 120 + 2
first = True
start = datetime.datetime.now()
end = datetime.datetime.now()

ser = serial.Serial('COM5', baudrate=500000)                         # open serial port
print(ser.baudrate)
print("New connection on: " + ser.name)         # check which port was really used
ser.write(0x10)
lastFrag = -1
fileArray = []

fileEnd = False
while not fileEnd:
    lineEnd = False
    index = 0
    line = None
    line = ser.read(packetLength + 1)
    if (len(line) != packetLength + 1):
        if (lastFrag < 640):
            missed = [0xFF] * ((packetLength - 2) * (640 - lastFrag - 1))
            fileArray.append(bytes(missed))
        fileEnd = True
        break
    
    if (first):
        ser.timeout = 3
        start = datetime.datetime.now()
        first = False
    
    fragment = line[0] * 256 + line[1]
    if (fragment - lastFrag < 640):
        missed = [0xFF] * ((packetLength - 2) * (fragment - lastFrag - 1))
        fileArray.append(bytes(missed)) 
        fileArray.append(line[2:122])
        lastFrag = fragment
    rssi = 255 - line[packetLength]
    
    print(str(fragment) + " @ " + str(rssi))
    if (fragment == 639):
        fileEnd = True
        end = datetime.datetime.now()
  
file = open("recvbytes.bin", "wb")  
for element in fileArray:
    file.write(element)

print("Time: " + str((end-start).microseconds))
#input("Press ENTER to exit")
file.close()
ser.close()             # close port