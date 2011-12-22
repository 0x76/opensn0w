CC=gcc
CFLAGS=-Wall -I. -g -Ixpwntool -fPIC
OS_TARGET=opensn0w
LIBS=-lusb-1.0 -lcurl -lz -lreadline -lpng -lssl -lcrypto
OBJS=main.o limera1n.o libpartial.o crtsup.o libirecovery.o patch.o patches.o img3.o nor_files.o \
	img2.o 8900.o ibootim.o abstractfile.o lzss.o lzssfile.o plist.o base64.o
all: opensn0w

UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS = -c -Wall -I. -g -I./include -I/usr/local/include -I/opt/local/include -Ilibusb-1.0  -Ixpwntool
	LDFLAGS = -L/usr/lib -L/opt/local/lib 
	LIBS =-lusb-1.0 -lcurl -lz -lreadline -framework CoreFoundation -framework IOKit -lpng -lssl -lcrypto
endif

opensn0w: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OS_TARGET) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o opensn0w
