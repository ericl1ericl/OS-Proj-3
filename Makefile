
all: mandel mandelmovie

mandelmovie: mandelmovie.o
	gcc mandelmovie.o -o mandelmovie -lpthread

mandel: mandel.o bitmap.o
	gcc mandel.o bitmap.o -o mandel -lpthread

mandelmovie.o: mandelmovie.c
	gcc -Wall -std="c99" -g -c mandelmovie.c -o mandelmovie.o

mandel.o: mandel.c
	gcc -Wall -g -c mandel.c -o mandel.o

bitmap.o: bitmap.c
	gcc -Wall -g -c bitmap.c -o bitmap.o

clean:
	rm -f *.o mandel mandelmovie
