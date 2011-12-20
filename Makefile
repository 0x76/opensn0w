CC=gcc
CFLAGS=-c -Wall -I. -g
OS_TARGET=opensn0w
LIBS=-lusb-1.0 -lcurl -lz -lreadline
OBJS=main.o limera1n.o libpartial.o crtsup.o libirecovery.o patch.o patches.o
all: opensn0w

UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS = -c -Wall -I. -g -I./include -I/usr/local/include -I/opt/local/include -Ilibusb-1.0
	LDFLAGS = -L/usr/lib -L/opt/local/lib 
	LIBS =-lusb-1.0 -lcurl -lz -lreadline -framework CoreFoundation -framework IOKit
endif

opensn0w: $(OBJS)
	$(CC) $(OBJS) -o $(OS_TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o opensn0w
