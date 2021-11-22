.POSIX:
CFLAGS += -I.
CFLAGS += -std=c99
CFLAGS += -Wall
LDLIBS += -lm
BIN = expvc expexc

all: $(BIN)

expvc: expvc.c

expexc: expexc.c

clean:
	rm -rf $(BIN)
