CC=gcc
CFLAGS=-O3 -c -Wall
SOURCES=bmp.c hifs.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=../bin/hifs

all: $(SOURCES) $(EXECUTABLE)
	rm *.o

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o