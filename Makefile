CC=gcc
CFLAGS=-Wall -I. -g -Ixpwntool -fPIC
OS_TARGET=opensn0w
LDFLAGS=
MANDIR=/usr/local/man/man1/
BINDIR=/usr/local/bin/
PLIST_DIR=/usr/local/share/opensn0w/
LIBS=-lusb-1.0 -lcurl -lz -lreadline -lpng -lssl -lcrypto 
OBJS=main.o limera1n.o libpartial.o crtsup.o libirecovery.o patch.o patches.o img3.o nor_files.o \
	img2.o 8900.o ibootim.o abstractfile.o lzss.o lzssfile.o plist.o base64.o keys.o steaks4uce.o pwnage2.o
all: opensn0w

.SILENT:
.SUFFIXES: .c .o

UNAME := $(shell uname -s)

ifeq ($(UNAME),Darwin)
	CFLAGS = -Wall -I. -g -I./include -I/usr/local/include -I/opt/local/include -Ilibusb-1.0  -Ixpwntool
	LDFLAGS = -L/usr/lib -L/opt/local/lib -L/usr/X11/lib
	LIBS =-lusb-1.0 -lcurl -lz -lreadline -framework CoreFoundation -framework IOKit -lpng -lssl -lcrypto
endif

opensn0w: $(OBJS)
	${LINK}
	if $(CC) $(CFLAGS) $(OBJS) -o $(OS_TARGET) $(LDFLAGS) $(LIBS); then \
		${LINK_OK}; \
	else \
		${LINK_FAILED}; \
	fi
	

%.o: %.c
	${COMPILE_STATUS}
	if ${CC} ${CFLAGS} ${CFLAGS} -c -o $@ $<; then \
		${COMPILE_OK}; \
	else \
		${COMPILE_FAILED}; \
	fi

clean:
	rm -rf *.o opensn0w

install:
	install -d $(PLIST_DIR) $(BINDIR) $(MANDIR)
	install -m644 Keys/*.plist $(PLIST_DIR)
	install -m755 opensn0w $(BINDIR)
	install -m775 opensn0w.1 $(MANDIR)
	@echo "Firmware bundles are in $(PLIST_DIR)."

DIR_ENTER = printf "\033[K\033[0;36mEntering directory \033[1;36m$$i\033[0;36m.\033[0m\n"; cd $$i || exit 1
DIR_LEAVE = printf "\033[K\033[0;36mLeaving directory \033[1;36m$$i\033[0;36m.\033[0m\n"; cd .. || exit 1
DEPEND_STATUS = printf "\033[K\033[0;33mGenerating dependencies...\033[0m\r"
DEPEND_OK = printf "\033[K\033[0;32mSuccessfully generated dependencies.\033[0m\n"
DEPEND_FAILED = printf "\033[K\033[0;31mFailed to generate dependencies!\033[0m\n"; exit 1
COMPILE_STATUS = printf "\033[K\033[0;33mCompiling \033[1;33m$<\033[0;33m...\033[0m\r"
COMPILE_OK = printf "\033[K\033[0;32mSuccessfully compiled \033[1;32m$<\033[0;32m.\033[0m\n"
COMPILE_FAILED = printf "\033[K\033[0;31mFailed to compile \033[1;31m$<\033[0;31m!\033[0m\n"; exit 1
LINK_STATUS = printf "\033[K\033[0;33mLinking \033[1;33m$@\033[0;33m...\033[0m\r"
LINK_OK = printf "\033[K\033[0;32mSuccessfully linked \033[1;32m$@\033[0;32m.\033[0m\n"
LINK_FAILED = printf "\033[K\033[0;31mFailed to link \033[1;31m$@\033[0;31m!\033[0m\n"; exit 1
INSTALL_STATUS = printf "\033[K\033[0;33mInstalling \033[1;33m$$i\033[0;33m...\033[0m\r"
INSTALL_OK = printf "\033[K\033[0;32mSuccessfully installed \033[1;32m$$i\033[0;32m.\033[0m\n"
INSTALL_FAILED = printf "\033[K\033[0;31mFailed to install \033[1;31m$$i\033[0;31m!\033[0m\n"; exit 1
DELETE_OK = printf "\033[K\033[0;34mDeleted \033[1;34m$$i\033[0;34m.\033[0m\n"
DELETE_FAILED = printf "\033[K\033[0;31mFailed to delete \033[1;31m$$i\033[0;31m!\033[0m\n"; exit 1

