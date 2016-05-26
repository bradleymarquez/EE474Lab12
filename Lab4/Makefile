# Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
# Makefile for Lab 4
# 5/25/16

# default target
tank: hBridgeDriver.o sensorDriver.o
	arm-linux-gnueabihf-gcc -Wall -o tank hBridgeDriver.o sensorDriver.o

# individual source files
hBridgeDriver.o: hBridgeDriver.c
	arm-linux-gnueabihf-gcc -Wall -c hBridgeDriver.c

sensorDriver.o: sensorDriver.c
	arm-linux-gnueabihf-gcc -Wall -c sensorDriver.c

# false target for easy transfer to beaglebone
transfer:
	scp hBridgeDriver sensorDriver root@192.168.7.2:~

# false target for cleanup
clean:
	rm -rf hBridgeDriver sensorDriver *.o *~
