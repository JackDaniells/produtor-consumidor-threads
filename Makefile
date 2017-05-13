CC=gcc
CFLAGS=-I.
LIBS=-lpthread
DEPS = module.h pedrolio.h
OBJ = main.o pedrolio.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

grupo9: $(OBJ) mod_grupo9/mod_grupo9.o
	gcc -o $@ $^ $(CFLAGS) $(LIBS)

all: grupo9

PHONY: clean

clean:
	rm -f *.o */*.o grupo9
