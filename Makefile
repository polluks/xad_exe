CC = cc
CFLAGS = -Wall -Wextra -pedantic -std=c99 -g -O2

all: test_xad_pe

test_xad_pe: test_xad_pe.c minilibxad.c
	$(CC) $(CFLAGS) -o $@ test_xad_pe.c minilibxad.c

clean:
	rm -f test_xad_pe

.PHONY: all clean
