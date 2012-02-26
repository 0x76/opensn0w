#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([opensn0w], [0.1], [rms@velocitylimitless.org])

AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AC_GNU_SOURCE

AC_CONFIG_SRCDIR([8900.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([-Wall])
# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcrypto':
AC_CHECK_LIB([crypto], [BN_init])
AC_CHECK_LIB([ssl], [SSL_accept])
AC_CHECK_LIB([z], [inflate])

AC_CHECK_LIB([usb-1.0], [libusb_init])
AC_CHECK_LIB([readline], [readline])
AC_CHECK_LIB([curl], [curl_free])

dnl use pretty build output with automake >= 1.11
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],
  [AM_DEFAULT_VERBOSITY=1
   AC_SUBST(AM_DEFAULT_VERBOSITY)])

# Checks for header files.
AC_HEADER_DIRENT
AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h inttypes.h limits.h netinet/in.h stddef.h stdint.h stdlib.h string.h sys/ioctl.h sys/mount.h sys/socket.h sys/time.h unistd.h])

case "$target" in
*-*-mingw32)
    CFLAGS="-I/mingw/include $CFLAGS"
    LIBS="$LIBS -lsetupapi"
        ;;
esac

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_TYPE_UID_T
AC_C_INLINE
AC_C_BIGENDIAN
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_HEADER_TIME
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_C_VOLATILE
AC_PROG_CC_C99


# Checks for library functions.
AC_FUNC_CHOWN
AC_FUNC_CLOSEDIR_VOID
AC_FUNC_FORK
AC_FUNC_FSEEKO
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MALLOC
AC_FUNC_MEMCMP
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STAT
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero clock_gettime dup2 memchr memmove memset mkdir rmdir socket strcasecmp strchr strdup strndup strrchr strstr strtol uname strcasestr strndup])

AC_PATH_PROG(RM, rm)
AC_PATH_PROG(MV, mv)
AC_PATH_PROG(CP, cp)
AC_PATH_PROG(LN, ln)
AC_PATH_PROG(TAR, tar)
AC_PATH_PROG(AR, ar)
AC_PATH_PROG(INSTALL, install)


AC_CONFIG_FILES([Makefile])
AC_OUTPUT

echo "
Configuration:
	opensn0w version     : ${PACKAGE_VERSION}
	LDFLAGS              : ${LDFLAGS}
	CFLAGS               : ${CFLAGS}

Type make to build opensn0w, and make install to install it."
