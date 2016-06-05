# Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
# Makefile for Lab 4
# 5/25/16

CC = arm-linux-gnueabihf-gcc
# default target
all: sensor hBridge

hBridge: hBridgeDriver.o arduino-serial-lib.h
	$(CC) -Wall -o hBridge hBridgeDriver.o

hBridgeDriver.o: hBridgeDriver.c
	$(CC) -Wall -c hBridgeDriver.c

sensor: sensorDriver.o
	$(CC) -Wall -o sensor sensorDriver.o

sensorDriver.o: sensorDriver.c
	$(CC) -Wall -c sensorDriver.c

# false target for easy transfer to beaglebone
transfer:
	scp sensor hBridge arduino-serial-lib.c arduino-serial-lib.h root@192.168.7.2:~

# false target for cleanup
clean:
	rm -rf hBridgeDriver sensorDriver *.o *~
