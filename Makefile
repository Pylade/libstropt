CC=gcc
AR=ar
MAIN_CFLAGS=-std=c89 -pedantic -Wall -Wextra -Winit-self -Wstrict-prototypes -Wwrite-strings -Wunreachable-code
DEBUG_CFLAGS=-g -O0
RELEASE_CFLAGS=-O2
MAIN_LDFLAGS=-shared -Wl,-soname,libstropt.so.1
DEBUG_LDFLAGS=
RELEASE_LDFLAGS=-s
LIB=libstropt.a libstropt.so.1.0-a2
EXEC=debug

ifeq ($(DEBUG),true)
CFLAGS=$(DEBUG_CFLAGS) $(MAIN_CFLAGS)
LDFLAGS=$(DEBUG_LDFLAGS) $(MAIN_LDFLAGS)
else
CFLAGS=$(RELEASE_CFLAGS) $(MAIN_CFLAGS)
LDFLAGS=$(RELEASE_LDFLAGS) $(MAIN_LDFLAGS)
endif


lib: $(LIB)

all: $(LIB) $(EXEC)

libstropt.a: atropt.o user.o
	$(AR) rcs $@ $^

libstropt.so.0.1-a2: atropt.pic.o user.pic.o
	$(CC) $(LDFLAGS) $^ -o $@

atropt.o: atropt.c atropt.h stropt.h
	$(CC) $(CFLAGS) $< -c -o $@

atropt.pic.o: atropt.c atropt.h stropt.h
	$(CC) $(CFLAGS) -fpic $< -c -o $@

user.o: user.c stropt.h
	$(CC) $(CFLAGS) $< -c -o $@

user.pic.o: user.c stropt.h
	$(CC) $(CFLAGS) -fpic $< -c -o $@

.PHONY: clean mrproper

clean:
	@rm -f *.o

mrproper: clean
	@rm -f $(LIB) $(EXEC)

debug: debug.c libstropt.a stropt.h
	$(CC) $(CFLAGS) $< -L. -lstropt -o $@
