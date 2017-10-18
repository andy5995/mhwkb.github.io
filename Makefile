TARGET = mhwkb
LIBS = -lm
CC = gcc
CFLAGS = -Wall

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst src/%.c, src/%.o, $(wildcard src/*.c))
HEADERS = $(wildcard src/*.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -std=gnu99 -Wall $(LIBS) -o $@

clean:
	-rm -f *.html
	-rm -f src/*.o
	-rm -f $(TARGET)