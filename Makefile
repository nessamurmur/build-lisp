P=
OBJECTS=
CFLAGS= -g -Wall -O3
LDFLAGS=
CC=c99

$(P):$(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/$@ src/$(P).c
