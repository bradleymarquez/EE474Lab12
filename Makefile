# Lab 1 Makefile

# default target
music: music.c music.h
	gcc -o music music.c

# false target used to delete build files
clean:
	rm -rf *.o music
