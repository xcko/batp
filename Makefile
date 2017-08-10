CC?=gcc
INSTALL?=install
RM?=rm -f

PREFIX?=/usr/local

BINDIR?=$(PREFIX)/bin
INCDIR?=$(PREFIX)/include
LIBDIR?=$(PREFIX)/lib
MANDIR?=$(PREFIX)/man

CFLAGS?=-Os
CFLAGS+=-ansi -pedantic -Wall -Wextra
CFLAGS+=-Isrc -I/usr/include -I$(INCDIR) -I/usr/X11R6/include

LDFLAGS+=-L/usr/lib -L$(LIBDIR) -L/usr/X11R6/lib

LIBS+=-lX11

OBJECTS=batp.o

all: batp

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

batp: $(OBJECTS)
	$(CC) $(LDFLAGS) -o batp $(OBJECTS) $(LIBS)

clean:
	$(RM) $(OBJECTS) batp batp.core

install: batp
	$(INSTALL) -m0755 batp $(BINDIR)
#	$(INSTALL) -m0444 batp.1 $(MANDIR)/man1

uninstall:
	$(RM) $(BINDIR)/batp
#	$(RM) $(MANDIR)/man1/batp.1

.PHONY: all clean install uninstall
