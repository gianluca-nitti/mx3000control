.PHONY: all clean install uninstall

PROGRAMNAME=mx3000control
PREFIX?=/usr

HIDAPI=hidapi-hidraw
#HIDAPI=hidapi-libusb

CFLAGS+=-Wall -pedantic-errors $(shell pkg-config --cflags $(HIDAPI))
LIBS=$(shell pkg-config --libs $(HIDAPI))

COMMANDS=$(patsubst %.c, %.o, $(wildcard commands/*.c))

all: $(PROGRAMNAME)

$(PROGRAMNAME): main.o encoding.o util.o keymap.o macro.o default_macros.o $(COMMANDS)
	$(CC) $^ $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o commands/*.o $(PROGRAMNAME)

install: $(PROGRAMNAME)
	install -Dm 755 $< $(PREFIX)/bin/$<

uninstall:
	rm -f $(PREFIX)/bin/$(PROGRAMNAME)
