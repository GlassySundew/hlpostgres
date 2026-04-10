# name of produced module
TARGET = postgres.hdll

# source files
SRC = postgres.c

# compiler
CC = gcc

# IMPORTANT: change this to your hashlink root
HL_PATH ?= ../../src

LIBPQ_CFLAGS := $(shell pkg-config --cflags libpq)
LIBPQ_LIBS   := $(shell pkg-config --libs libpq)

CFLAGS = -O2 -fPIC -shared \
	-I$(HL_PATH) \
	$(LIBPQ_CFLAGS)

LDFLAGS = $(LIBPQ_LIBS)

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: all clean

