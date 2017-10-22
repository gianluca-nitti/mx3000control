.PHONY: all clean

HIDAPI=hidapi-hidraw
#HIDAPI=hidapi-libusb

CFLAGS+=-Wall -pedantic-errors $(shell pkg-config --cflags $(HIDAPI))
LIBS=$(shell pkg-config --libs $(HIDAPI))

all: mx3000control

mx3000control: main.o encoding.o command_raw.o command_setrgbled.o
	$(CC) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o mx3000control
