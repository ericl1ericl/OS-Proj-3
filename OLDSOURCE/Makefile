# DO NOT EDIT -------------------------------
C=/afs/nd.edu/user14/csesoft/new/bin/gcc
CFLAGS=-Wall
CPP=/afs/nd.edu/user14/csesoft/new/bin/g++
CPPFLAGS=-Wall
LD=/afs/nd.edu/user14/csesoft/new/bin/g++
LDFLAGS=-lpthread
# # ---------------------------- END DO NOT EDIT

CPPFLAGS += -std=c++11 -g   # Add your own flags here, or leave blank
LDFLAGS +=                  # Add your own flags here, or leave blank

mandelmovie: mandelmovie.o mandel.o bitmap.o
	$(LD) $^ $(LDFLAGS) -o $@

mandel: mandel.o bitmap.o
	$(LD) $^ $(LDFLAGS) -o $@

mandelmovie.o: mandelmovie.cpp
	$(CPP) $(CPPFLAGS) -c $<

bitmap.o: bitmap.c
	$(C) $(CFLAGS) -c $<

mandel.o: mandel.c
	$(C) $(CFLAGS) -c $<

 #Uncoment to use the C compiler
%.o: %.c
	$(C) $(CFLAGS) -c $<

.PHONY: clean
clean:
	rm -f mandel mandelmovie *.o *.bmp

