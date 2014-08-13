P=repl
OBJECTS=
CFLAGS= -g -Wall -O3
LDFLAGS=-ledit -lm
CC=c99

$(P):$(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/$@ src/$(P).c src/mpc.c
