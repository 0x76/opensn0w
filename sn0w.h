/* opensn0w 
 * An open-source jailbreaking utility.
 * Brought to you by rms, acfrazier & Maximus
 * Special thanks to iH8sn0w & MuscleNerd
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#ifndef _SN0W_H_
#define _SN0W_H_
#define _GNU_SOURCE		/* for strcasestr */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MSVC_VER
/*
 * evil hacks 
 */
#pragma warning(disable:4131)	/* k&r c style used */
#pragma warning(disable:4003)	/* not enough actual parameters for macro */
#pragma warning(disable:4127)	/* conditional expression is always constant */
#pragma warning(disable:4214)	/* unknown nonstandard extension */
#pragma warning(disable:4200)	/* unknown nonstandard extension */
#pragma warning(disable:4201)	/* unknown nonstandard extension */
#pragma warning(disable:4204)	/* unknown nonstandard extension */
#pragma warning(disable:4018)	/* signed/unsigned mismatch */
#pragma warning(disable:4245)	/* signed/unsigned mismatch */
#pragma warning(disable:4242)	/* integer conversion */
#pragma warning(disable:4244)	/* integer conversion */
#pragma warning(disable:4005)	/* macro redefinition */
#pragma warning(disable:4706)	/* assignment within cond expression */
#pragma warning(disable:4702)	/* unreachable code */
#pragma warning(disable:4701)	/* *potentially* unused variable used? */
#endif

#include <curl/curl.h>
#include "libirecovery.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <stdarg.h>
#include "libpartial.h"
#include "common.h"
#include "xpwn/libxpwn.h"
#include "xpwn/nor_files.h"
#include <xpwn/plist.h>
#include <ctype.h>
#include "debug.h"

#ifdef MSVC_VER
#include "config.win32.msvc.h"
#elif defined(__APPLE__) && defined(__XCODE_BUILD__)
#include "config.apple.llvm.h"
#else
#include "config.h"
#endif

#define __SN0W_VERSION__ "opensn0w-" PACKAGE_VERSION

#ifndef RELEASE
#define __SN0W_CONFIG__ "DEVELOPMENT"
#else
#define __SN0W_CONFIG__ "RELEASE"
#endif


/* fun here */
#ifdef MSVC_VER
#ifdef _M_X64
#elif defined _M_IX86
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_I386(msvc)"
#elif defined _M_IA64
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_IA64(msvc)"
#elif defined _M_MPPC
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_PPC(msvc)"
#elif defined _M_MRX000
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_MIPS(msvc)"
#else
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_GENERIC(msvc)"
#endif	/* end msvc */
#elif defined __clang__
#ifdef __alpha__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_ALPHA(clang)"
#elif defined __x86_64__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_X86_64(clang)"
#elif defined __arm__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_ARM(clang)"
#elif defined __thumb__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_THUMB(clang)"
#elif defined __convex__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_CONVEX(clang)"
#elif defined __hppa__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_HPPA(clang)"
#elif defined __i386__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_I386(clang)"
#elif defined __ia64__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_IA64(clang)"
#elif defined __m68k__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_M68K(clang)"
#elif defined __mips__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_MIPS(clang)"
#elif defined __powerpc__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_POWERPC(clang)"
#elif defined __sparc__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_SPARC(clang)"
#elif defined __sh__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_SUPERH(clang)"
#elif defined __s390__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_S390(clang)"
#endif
#elif defined __GNUC__
#ifdef __alpha__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_ALPHA(gcc)"
#elif defined __x86_64__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_X86_64(gcc)"
#elif defined __arm__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_ARM(gcc)"
#elif defined __thumb__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_THUMB(gcc)"
#elif defined __convex__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_CONVEX(gcc)"
#elif defined __hppa__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_HPPA(gcc)"
#elif defined __i386__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_I386(gcc)"
#elif defined __ia64__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_IA64(gcc)"
#elif defined __m68k__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_M68K(gcc)"
#elif defined __mips__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_MIPS(gcc)"
#elif defined __powerpc__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_POWERPC(gcc)"
#elif defined __sparc__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_SPARC(gcc)"
#elif defined __sh__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_SUPERH(gcc)"
#elif defined __s390__
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_S390(gcc)"
#endif
#else	/* end supported compilers */
#define __SN0W_VERSION_FULL__ __SN0W_VERSION__ "/" __SN0W_CONFIG__ "_GENERIC(unknown)"
#endif




#ifdef __APPLE__
#define _SVID_SOURCE
#define _BSD_SOURCE
#define _XOPEN_SOURCE 700
#endif

#ifndef _WIN32
#include <execinfo.h>
#include <signal.h>
#include <ucontext.h>
#include <unistd.h>

void critical_error_handler(int sig_num, siginfo_t * info, void *ucontext);
#endif

typedef enum ____endian_t {
	ENDIAN_BIG = 0,
	ENDIAN_LITTLE
} __endian_t;

extern irecv_device_t device;
extern irecv_client_t client;

typedef struct _patch {
	unsigned char *original;
	unsigned char *patched;
	unsigned int length;
} patch, *patch_t;

extern patch iBSS_SHSH;
extern patch iBSS_ECID;
extern patch iBSS_BORD;
extern patch iBSS_CEPO;
extern patch iBSS_SEPO;
extern patch iBSS_TYPE;
extern patch iBSS_CHIP;
extern patch iBSS_SDOM;
extern patch iBSS_PROD;

extern patch iBEC_bootargs;
extern patch iBEC_bootargs_jmp;
extern patch iBEC_51_conditional_ba;

extern patch kernel_CSED;
extern patch kernel_AMFI;
extern patch kernel__PE_i_can_has_debugger;
extern patch kernel_IOAESAccelerator;
extern patch kernel_sigcheck;
extern patch kernel_xattr;
extern patch kernel_redsn0w_unknown0;
extern patch kernel_redsn0w_unknown1;
extern patch kernel_redsn0w_unknown2;
extern patch kernel_redsn0w_unknown3;
extern patch kernel_nor0;
extern patch kernel_nor1;
extern patch kernel_nor2;
extern patch kernel_nor3;
extern patch kernel_nor4;

extern patch devicetree_root_name;

#ifdef MSVC_VER
#ifndef __cplusplus
typedef BOOL bool;
#endif
#define __builtin_bswap32 _byteswap_ulong	/* msvc bswap variant */

#define snprintf _snprintf	/* ugh */
#define stat _stat
#define vsnprintf _vsnprintf
#define strdup _strdup
#define unlink _unlink



#endif



int limera1n();
int steaks4uce();
int pwnage2();
int endian();
int shatter();
void dos_cursor();
int hfs_main(int argc, const char *argv[]);
int fetch_image(const char *path, const char *output);
char* endian_to_string(int endian);
void *our_memmem(const void *l, size_t l_len, const void *s, size_t s_len);
void hex_dump(void *data, int size);
int patch_file(char *filename);
int irecovery_shell_initialize();
bool file_exists(const char *fileName);
int poll_device(int mode);
Dictionary *get_key_dictionary_from_bundle(char *member);
int bsdiff(char *oldfile, char *newfile, char *patchfile);
void jailbreak();
void greenpois0n_inject(void);

/* crt supplement */

#ifndef HAVE_STRCASESTR
char *strcasestr (char *haystack, char *needle);
#endif

#ifndef HAVE_STRNDUP
char *strndup (const char *s, size_t n);
#endif

#ifndef HAVE_WCSCASECMP
int wcscasecmp(const wchar_t *s1, const wchar_t *s2);
#endif

#ifndef HAVE_STRCASECMP
int strcasecmp(const char *s1, const char *s2);
#endif

#ifdef _WIN32
bool is_process_running(WCHAR* process_name);
BOOL GuiUpdateJailbreakStatus(VOID);
void GuiGdiPlusConstructor(void);
void GuiGdiPlusDeconstructor(void);
HICON GuiGetIconForPng(const WCHAR *filename);
HICON GuiGetIconForResource(const WCHAR *bitmapName);

#ifndef MSVC_VER
BOOL WINAPI GradientFill(
    HDC hdc,
    PTRIVERTEX pVertex,
    ULONG nVertex,
    PVOID pMesh,
    ULONG nMesh,
    ULONG ulMode
);		
/* mingw headers suck */
#endif

#endif

typedef enum _image_magic {
	IMG3 = 0,
	IMG2,
	S5L8900,
	DMG
} image_magic_t;

typedef struct _firmware_item {
	char *key;
	char *iv;
	char *vfkey;
	char *name;
	image_magic_t magic;
} firmware_item, *firmware_item_t;

// The enum _firmware_images and the image_names array in main.c MUST match //

typedef enum _firmware_images {
	IBSS = 0,
	DEVICETREE,
	BATTERYCHARGING1,
	GLYPHCHARGING,
	BATTERYCHARGING0,
	IBOOT,
	BATTERYLOW0,
	LLB,
	IBEC,
	KERNELCACHE,
	FILESYSTEM,
	APPLELOGO,
	UPDATERAMDISK,
	RESTORERAMDISK,
	GLYPHPLUGIN,
	RECOVERY,
	BATTERYLOW1,
	BATTERYFULL,
	NEEDSERVICE
} firmware_images_t;

typedef enum _modes_t {
	DFU = 0,
	RECOVERYMODE
} modes_t;

typedef struct _firmware {
	firmware_item *item;
	uint8_t items;
} firmware, *firmware_t;

int make_ramdisk(firmware_item item);

int upload_image(firmware_item item, int mode, int patch, int userprovided);

#if defined(_GUI_ENABLE_) && !defined(_WIN32)
#error GUI is only supported on Windows for now! Sorry!
#endif

#ifdef _GUI_ENABLE_
VOID GuiToggleDFUTimers(BOOL show);
#endif

#ifdef __cplusplus
}
#endif

#endif
