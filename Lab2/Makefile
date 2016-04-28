# <Names>
# Makefile foe Lab 2
# 4/28/16

# default target
Hangman: Hangman.o ledinit.o
	gcc -Wall -o Hangman Hangman.o ledinit.o

# individual source files
Hangman.o: Hangman.c ledinit.c ledinit.h
	gcc -Wall -c Hangman.c

ledinit.o: ledinit.c ledinit.h
	gcc -Wall -c ledinit.c

# false target for easy transfer to beaglebone
transfer:
	scp Hangman root@192.168.7.2:~

# false target for cleanup
clean:
	rm -rf Hangman *.o *~
