CC := gcc
CFLAGS := -Wall -g -I/usr/include/SDL2
LIBS := -lSDL2 -lGLESv2

# Define the target executable
TARGET := main
SRC := main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET)

.PHONY: all clean
