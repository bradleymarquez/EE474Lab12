# Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
# Makefile for Lab 5
# 6/7/16

CC = arm-linux-gnueabihf-gcc
# default target
all: hBridgeDrive

hBridgeDrive: hBridgeDrive.o  arduino-serial-lib.h
	$(CC) -Wall -o hBridgeDrive hBridgeDrive.o

hBridgeDrive.o: hBridgeDrive.c
	$(CC) -Wall -c hBridgeDrive.c

# false target for easy transfer to beaglebone
transfer:
	scp  hBridgeDrive arduino-serial-lib.c arduino-serial-lib.h root@192.168.7.2:~

# false target for cleanup
clean:
	rm -rf hBridgeDrive *.o *~

