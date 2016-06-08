# Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
# Makefile for Lab 4
# 5/25/16

CC = arm-linux-gnueabihf-gcc
# default target
all: sensor hBridge

hBridge: hBridgeDriver.o ScannerControl.o SensorControl.o ServoControl.o arduino-serial-lib.h
	$(CC) -Wall -o hBridge hBridgeDriver.o ScannerControl.o SensorControl.o ServoControl.o

hBridgeDriver.o: hBridgeDriver.c
	$(CC) -Wall -c hBridgeDriver.c

sensor: sensorDriver.o
	$(CC) -Wall -o sensor sensorDriver.o

sensorDriver.o: sensorDriver.c
	$(CC) -Wall -c sensorDriver.c

ScannerControlTest: ScannerControlTest.o ScannerControl.o SensorControl.o ServoControl.o
	arm-linux-gnueabihf-gcc -Wall -o ScannerControlTest ScannerControlTest.o ScannerControl.o SensorControl.o ServoControl.o

ScannerControlTest.o: ScannerControlTest.c ScannerControl.h
	arm-linux-gnueabihf-gcc -Wall -c ScannerControlTest.c

ScannerControl.o: ScannerControl.c ScannerControl.h SensorControl.h ServoControl.h
	arm-linux-gnueabihf-gcc -Wall -c ScannerControl.c

SensorControl.o: SensorControl.c SensorControl.h
	arm-linux-gnueabihf-gcc -Wall -c SensorControl.c

ServoControl.o: ServoControl.c ServoControl.h
	arm-linux-gnueabihf-gcc -Wall -c ServoControl.c

# false target for easy transfer to beaglebone
transfer:
	scp sensor hBridge arduino-serial-lib.c arduino-serial-lib.h root@192.168.7.2:~

# false target for cleanup
clean:
	rm -rf hBridgeDriver sensorDriver *.o *~

