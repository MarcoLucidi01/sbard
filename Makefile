# see LICENSE file for copyright and license details

.POSIX:

CC     := cc
CFLAGS := -std=c99 -pedantic -Wall -Wextra -Werror -D_POSIX_C_SOURCE=199309L
LDLIBS := -lX11 -lasound
PREFIX := /usr/local

sbard: sbard.o memory.o volume.o battery.o datetime.o sh.o wireless.o temperature.o
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ $^

battery.o: battery.c sbard.h
datetime.o: datetime.c
memory.o: memory.c
sbard.o: sbard.c config.h sbard.h
sh.o: sh.c
temperature.o: temperature.c
volume.o: volume.c
wireless.o: wireless.c

install: sbard
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -m 755 sbard $(DESTDIR)$(PREFIX)/bin

uninstall:
	rm $(DESTDIR)$(PREFIX)/bin/sbard

clean:
	rm -f sbard *.o
