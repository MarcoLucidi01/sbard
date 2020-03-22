.POSIX:

CC     := cc
CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -D_POSIX_C_SOURCE=199309L
LDLIBS := -lX11 -lasound
PREFIX := /usr/local

sbard: sbard.o memory.o volume.o battery.o datetime.o
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ $^

sbard.o: sbard.c config.h sbard.h
memory.o: memory.c
volume.o: volume.c
battery.o: battery.c sbard.h
datetime.o: datetime.c

install: sbard
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 755 sbard $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/sbard

clean:
	rm -f sbard *.o
