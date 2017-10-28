.PHONY: all clean

HIDAPI=hidapi-hidraw
#HIDAPI=hidapi-libusb

CFLAGS+=-Wall -pedantic-errors $(shell pkg-config --cflags $(HIDAPI))
LIBS=$(shell pkg-config --libs $(HIDAPI))

COMMANDS=$(patsubst %.c, %.o, $(wildcard commands/*.c))

all: mx3000control

mx3000control: main.o encoding.o util.o $(COMMANDS)
	$(CC) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o commands/*.o mx3000control
