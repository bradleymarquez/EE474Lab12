# Brad Marquez, Joseph Rothlin, Aigerim Shintemirova
# April 13, 2016
# Lab 1 Makefile

# default target
music: music.c music.h
	gcc -o music music.c

#false target used to cross-compile
cross:
	arm-linux-gnueabihf-gcc -o music music.c

# false target used to delete build files
clean:
	rm -rf *.o music

# false target used to transfer executable file
transfer:
	scp music root@192.168.7.2:~
