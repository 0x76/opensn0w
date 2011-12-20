CC=gcc
CFLAGS=-c -Wall -I. -g
OS_TARGET=opensn0w
LIBS=-lusb-1.0 -lcurl -lz
OBJS=main.o limera1n.o libpartial.o crtsup.o libirecovery.o patch.o
all: opensn0w

opensn0w: $(OBJS)
	$(CC) $(OBJS) -o $(OS_TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o opensn0w
