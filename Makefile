# xad_exe - XAD client for Windows PE/EXE files
#
# One Makefile, two kinds of build:
#
#   make            native test harness (Linux/Unix)   -> test_xad_pe
#   make amiga      AmigaOS 3.x client object           -> xad_exe.amiga.o
#   make morphos    MorphOS client object               -> xad_exe.morphos.o
#   make clean      remove all build artifacts
#
# minilibxad.c is a libc shim of xadmaster.library. It is linked ONLY into
# the native test harness so the client can be exercised without AmigaOS.
# The Amiga/MorphOS builds never use it: there the xad* functions are
# provided by the real xadmaster.library.

CC       ?= cc
CFLAGS   ?= -Wall -Wextra -pedantic -std=c99 -g -O2
LDFLAGS  ?=

# --- native test harness -------------------------------------------------
TEST_SRC = test_xad_pe.c minilibxad.c
TEST_BIN = test_xad_pe
TEST_DEP = test_xad_pe.c minilibxad.c xad_exe.c xadClient.h xadmaster.h ConvertE.c

# --- XAD client (AmigaOS / MorphOS) --------------------------------------
CLIENT_SRC = xad_exe.c
CLIENT_DEP = xad_exe.c xadClient.h xadmaster.h ConvertE.c

AMIGA_CC       ?= m68k-amigaos-gcc
AMIGA_CFLAGS   ?= -Wall -Wextra -std=c99 -O2 -fbaserel
AMIGA_OBJ       = xad_exe.amiga.o

MORPHOS_CC     ?= ppc-morphos-gcc
MORPHOS_CFLAGS ?= -Wall -Wextra -std=c99 -O2
MORPHOS_OBJ     = xad_exe.morphos.o

.PHONY: all test amiga morphos clean

all: test

test: $(TEST_BIN)

$(TEST_BIN): $(TEST_DEP)
	$(CC) $(CFLAGS) -o $@ $(TEST_SRC) $(LDFLAGS)

# Compile the client for inclusion in the XAD build. Linking against
# xadmaster.library is done by the XAD SDK/library link step, not here.
amiga: $(AMIGA_OBJ)

$(AMIGA_OBJ): $(CLIENT_DEP)
	$(AMIGA_CC) $(AMIGA_CFLAGS) -c -o $@ $(CLIENT_SRC)

morphos: $(MORPHOS_OBJ)

$(MORPHOS_OBJ): $(CLIENT_DEP)
	$(MORPHOS_CC) $(MORPHOS_CFLAGS) -c -o $@ $(CLIENT_SRC)

clean:
	rm -f $(TEST_BIN) $(AMIGA_OBJ) $(MORPHOS_OBJ)
