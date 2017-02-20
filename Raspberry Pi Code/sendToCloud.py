#!/usr/bin/python

#Reading the information of the arduino device from the serial port and
#uploading it to the thingspeak cloud platform
import httplib
import serial
import urllib
import json
import time

# Serial port connection USB
#try:
#        ser = serial.Serial("/dev/ttyACM0", 9600, timeout=1)
#except:
#        ser = serial.Serial("/dev/ttyACM1", 9600, timeout=1)

#Serial port connection Bluetooth
ser = serial.Serial("/dev/rfcomm0", baudrate=9600, timeout=1)

pulses = 0;
liters = 0;
numberOfMeasures = 0;

key = "EE9FJQGXOHZXF2K2"
pulsesPerLiter = 400;

while True:
    # Read line received
	try:
        	line = ser.readline().strip()
	except:
		print "Serial port error"
		

	if line:
                print line
    # Remove possible garbage lines
        if line.startswith("pulses"):
            piece = line.split(":")
            if 2==len(piece):
                pulses = float(piece[1].strip())
                print pulses
                liters = float(pulses)/pulsesPerLiter
                print liters

		#Preparing to send the information to the cloud
                params=urllib.urlencode({'field1':liters,'key':key})
                headers={"Content-typZZe": "application/x-www-form-urlencoded","Accept":"text/plain"}
                conn=httplib.HTTPConnection("api.thingspeak.com:80")

		#Sending the data to the thingspeak platform
                try:
                        conn.request("POST", "/update", params, headers)
                        response=conn.getresponse()
                        print response.status, response.reason
                        data=response.read()
                        conn.close()
                except:
                        print "connection failed"
		
		#Writing the data to a file for backup
		file = open("littersSaved.csv","a")
		numberOfMeasures +=1
		cTime = time.strftime("%H:%M:%S")
		cDate = time.strftime("%Y-%m-%d")
		record = cDate + " " + cTime + " " + "+0300," + str(numberOfMeasures) + "," + str(liters) + "\n"
		file.write(record)
		file.close()











