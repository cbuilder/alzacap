all:
	gcc capture.c -Wall -lasound

clean:
	@rm -f a.out *.o
