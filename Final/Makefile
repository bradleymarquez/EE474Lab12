# Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
# Makefile for Lab 4
# 5/25/16

# default target
all: sensor hBridge

hBridge: hBridgeDriver.o 
	arm-linux-gnueabihf-gcc -Wall -o hBridge hBridgeDriver.o

hBridgeDriver.o: hBridgeDriver.c
	arm-linux-gnueabihf-gcc -Wall -c hBridgeDriver.c

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

