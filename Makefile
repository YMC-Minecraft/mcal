.POSIX:
CFLAGS += -I.
CFLAGS += -std=c99
CFLAGS += -Wall
LDLIBS += -lm
BIN = expvc expexc tprojvc

all: $(BIN)

expvc: expvc.c

expexc: expexc.c

tprojvc: tprojvc.c

clean:
	rm -rf $(BIN)
