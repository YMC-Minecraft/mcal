.POSIX:
CFLAGS += -I.
CFLAGS += -std=c99
CFLAGS += -Wall
LDLIBS += -lm
BIN = expvc

all: $(BIN)

expvc: expvc.c

clean:
	rm -rf $(BIN)
