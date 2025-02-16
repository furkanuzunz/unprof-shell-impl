CC=gcc
CFLAGS=-Wall -Wextra -I../include

SRCDIR=../src
LIBDIR=../lib
SOURCES=$(SRCDIR)/shell.c $(SRCDIR)/utils.c
OBJECTS=$(SOURCES:.c=.o)
TARGET=shell

# Object dosyaları lib/ dizinine
$(TARGET): $(OBJECTS)
	mv $(SRCDIR)/*.o $(LIBDIR)/
	$(CC) $(LIBDIR)/*.o -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(LIBDIR)/*.o $(TARGET)