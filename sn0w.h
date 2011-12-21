/* OpenSn0w 
 * Open source equivalent of redsn0w
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

#include "libirecovery.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdbool.h>
#include "libpartial.h"
#include "common.h"
#include "xpwn/libxpwn.h"
#include "xpwn/nor_files.h"
#include <xpwn/plist.h>
#include <ctype.h>

extern irecv_device_t device;
extern irecv_client_t client;

typedef struct _patch {
	unsigned char* original;
	unsigned char* patched;
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


int limera1n();
void* our_memmem(const void *l, size_t l_len, const void *s, size_t s_len);
void hex_dump(void *data, int size);

#endif
