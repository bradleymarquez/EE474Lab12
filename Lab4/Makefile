# Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
# Makefile for Lab 4
# 5/25/16

# default target
all: sensor hBridge

hBridge: hBridgeTest.o 
	arm-linux-gnueabihf-gcc -Wall -o hBridge hBridgeTest.o

hBridgeTest.o: hBridgeTest.c
	arm-linux-gnueabihf-gcc -Wall -c hBridgeTest.c

sensor: sensorDriver.o
	arm-linux-gnueabihf-gcc -Wall -o sensor sensorDriver.o

sensorDriver.o: sensorDriver.c
	arm-linux-gnueabihf-gcc -Wall -c sensorDriver.c

# false target for easy transfer to beaglebone
transfer:
	scp sensor hBridge root@192.168.7.2:~

# false target for cleanup
clean:
	rm -rf hBridgeDriver sensorDriver *.o *~

