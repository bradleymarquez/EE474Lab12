# Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
# Makefile for Lab 2
# 4/28/16

# default target
Hangman: Hangman.o ledinit.o
	arm-linux-gnueabihf-gcc -Wall -o Hangman Hangman.o ledinit.o

# individual source files
Hangman.o: Hangman.c ledinit.c ledinit.h
	arm-linux-gnueabihf-gcc -Wall -c Hangman.c

ledinit.o: ledinit.c ledinit.h
	arm-linux-gnueabihf-gcc -Wall -c ledinit.c

# false target for easy transfer to beaglebone
transfer:
	scp Hangman root@192.168.7.2:~

# false target for cleanup
clean:
	rm -rf Hangman *.o *~
