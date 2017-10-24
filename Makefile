.PHONY: all clean

HIDAPI=hidapi-hidraw
#HIDAPI=hidapi-libusb

CFLAGS+=-Wall -pedantic-errors $(shell pkg-config --cflags $(HIDAPI))
LIBS=$(shell pkg-config --libs $(HIDAPI))

COMMANDS=command_raw.o command_setledcolor.o command_setledpulsation.o

all: mx3000control

mx3000control: main.o encoding.o $(COMMANDS)
	$(CC) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o mx3000control
