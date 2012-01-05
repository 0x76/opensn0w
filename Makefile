CC=gcc
CFLAGS=-Wall -I. -g -Ixpwntool -fPIC
OS_TARGET=opensn0w
LDFLAGS=
MANDIR=/usr/local/man/man1/
BINDIR=/usr/local/bin/
PLIST_DIR=/usr/local/share/opensn0w/
LIBS=-lusb-1.0 -lcurl -lz -lreadline -lpng -lssl -lcrypto 
OBJS=main.o limera1n.o libpartial.o crtsup.o libirecovery.o patch.o patches.o img3.o nor_files.o \
	img2.o 8900.o ibootim.o abstractfile.o lzss.o lzssfile.o plist.o base64.o keys.o
all: opensn0w

UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS = -Wall -I. -g -I./include -I/usr/local/include -I/opt/local/include -Ilibusb-1.0  -Ixpwntool
	LDFLAGS = -L/usr/lib -L/opt/local/lib -L/usr/X11/lib
	LIBS =-lusb-1.0 -lcurl -lz -lreadline -framework CoreFoundation -framework IOKit -lpng -lssl -lcrypto
endif

opensn0w: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(OS_TARGET) $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o opensn0w

install:
	install -d $(PLIST_DIR) $(BINDIR) $(MANDIR)
	install -m644 Keys/*.plist $(PLIST_DIR)
	install -m755 opensn0w $(BINDIR)
	install -m775 opensn0w.1 $(MANDIR)
	@echo "Firmware bundles are in $(PLIST_DIR)/opensn0w."
