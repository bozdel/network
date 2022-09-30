all: lab1.o misc.o
	gcc lab1.o misc.o -o lab1 -lpthread
lab1.o: lab1.c
	gcc -c lab1.c
misc.o: misc.c
	gcc -c misc.c
clean:
	rm *.o
