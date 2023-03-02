# create a makefile to compile the program and build a shared library
# the flags are -Wall -lxl

# the compiler
CC = gcc

LIB_DIR = lib/
INCLUDE_DIR = include/libxl

# the compiler flags
CFLAGS = -Wall -I$(INCLUDE_DIR) -lxl

SOURCES = lib/inspector.c lib/util.c

OBJECTS = $(SOURCES:.c=.o)

# the build target executable
TARGET = libinspector.dylib

all: $(SOURCES) $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -shared -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

example: example.c
	$(CC) $(SOURCES) $(CFLAGS) example.c -o example

clean:
	rm -f $(OBJECTS) $(TARGET)