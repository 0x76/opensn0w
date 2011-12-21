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

#include "sn0w.h"

/* iBSS patches */
unsigned char iBSS_SDOM_original[] = {0x03, 0x93, 0xFF, 0xF7, 0x42, 0xFE, 0x00, 0x28};
unsigned char iBSS_SDOM_patch[] = {0x03, 0x93, 0x00, 0x20, 0x00, 0x20, 0x00, 0x28};

unsigned char iBSS_PROD_original[] = {0x03, 0x93, 0xFF, 0xF7, 0x2F, 0xFE, 0x00, 0x28};
unsigned char iBSS_PROD_patch[] = {0x03, 0x93, 0x00, 0x20, 0x00, 0x20, 0x00, 0x28};

unsigned char iBSS_CHIP_original[] = {0x03, 0x93, 0xFF, 0xF7, 0x10, 0xFE, 0x00, 0x28};
unsigned char iBSS_CHIP_patch[] = {0x03, 0x93, 0x00, 0x20, 0x00, 0x20, 0x00, 0x28};

unsigned char iBSS_TYPE_original[] = {0x03, 0x93, 0xFF, 0xF7, 0xFE, 0xFD, 0x00, 0x28};
unsigned char iBSS_TYPE_patch[] = {0x03, 0x93, 0x00, 0x20, 0x00, 0x20, 0x00, 0x28};

unsigned char iBSS_SEPO_original[] = {0x03, 0x95, 0xFF, 0xF7, 0xE8, 0xFD, 0x00, 0x28};
unsigned char iBSS_SEPO_patch[] = {0x03, 0x95, 0x00, 0x20, 0x00, 0x20, 0x00, 0x28};

unsigned char iBSS_CEPO_original[] = {0x03, 0x95, 0xFF, 0xF7, 0xD6, 0xFD, 0x00, 0x28};
unsigned char iBSS_CEPO_patch[] = {0x03, 0x95, 0x00, 0x20, 0x00, 0x20, 0x00, 0x28};

unsigned char iBSS_BORD_original[] = {0x03, 0x94, 0xFF, 0xF7, 0xC4, 0xFD, 0x00, 0x28};
unsigned char iBSS_BORD_patch[] = {0x03, 0x94, 0x00, 0x20, 0x00, 0x20, 0x00, 0x28};

unsigned char iBSS_ECID_original[] = {0x03, 0x94, 0xFF, 0xF7, 0xAF, 0xFD, 0x00, 0x28};
unsigned char iBSS_ECID_patch[] = {0x03, 0x94, 0x00, 0x20, 0x00, 0x20, 0x00, 0x28};

unsigned char iBSS_SHSH_original[] = {0x4F, 0xF0, 0xFF, 0x30, 0x01, 0xE0, 0x4F, 0xF0, 0xFF, 0x30};
unsigned char iBSS_SHSH_patch[] = {0x00, 0x20, 0x00, 0x20, 0x01, 0xE0, 0x00, 0x20, 0x00, 0x20};

unsigned char iBEC_bootargs_original[] = {"rd=md0 nand-enable-reformat=1 -progress"};
unsigned char iBEC_bootargs_patch[] =    {"rd=disk0s1s1 debug=0x016E -v keepsyms=1"};

unsigned char iBEC_bootargs_ramdisk[] =  {"rd=md0 -v debug=0x16E keepsyms=1       "};

unsigned char iBEC_bootargs_jmp_original[] = {0x01, 0xF0, 0xE1, 0xF9, 0x96, 0x49, 0x00, 0x2C};
unsigned char iBEC_bootargs_jmp_patch[] =    {0x01, 0xF0, 0xE1, 0xF9, 0x96, 0x49, 0x01, 0x2C};

patch iBSS_SHSH = {iBSS_SHSH_original, iBSS_SHSH_patch, 10};
patch iBSS_ECID = {iBSS_ECID_original, iBSS_ECID_patch, 8};
patch iBSS_BORD = {iBSS_BORD_original, iBSS_BORD_patch, 8};
patch iBSS_CEPO = {iBSS_CEPO_original, iBSS_CEPO_patch, 8};
patch iBSS_SEPO = {iBSS_SEPO_original, iBSS_SEPO_patch, 8};
patch iBSS_TYPE = {iBSS_TYPE_original, iBSS_TYPE_patch, 8};
patch iBSS_CHIP = {iBSS_CHIP_original, iBSS_CHIP_patch, 8};
patch iBSS_SDOM = {iBSS_SDOM_original, iBSS_SDOM_patch, 8};
patch iBSS_PROD = {iBSS_PROD_original, iBSS_PROD_patch, 8};

patch iBEC_bootargs = {iBEC_bootargs_original, iBEC_bootargs_patch, sizeof(iBEC_bootargs_original)};
patch iBEC_bootargs_jmp = {iBEC_bootargs_jmp_original, iBEC_bootargs_jmp_patch, sizeof(iBEC_bootargs_jmp_original)};

/* kernel patches */
unsigned char kernel_CSED_original[] = {0xdf, 0xf8, 0x88, 0x33, 0x1d, 0xee, 0x90, 0x0f, 0xa2, 0x6a, 0x1b, 0x68};
unsigned char kernel_CSED_patch[] = {0xdf, 0xf8, 0x88, 0x33, 0x1d, 0xee, 0x90, 0x0f, 0xa2, 0x6a, 0x01, 0x23};

unsigned char kernel_AMFI_original[] = {0xD0, 0x47, 0x01, 0x21, 0x40, 0xB1, 0x13, 0x35};
unsigned char kernel_AMFI_patch[] = {0x00, 0x20, 0x01, 0x21, 0x40, 0xB1, 0x13, 0x35};

unsigned char kernel__PE_i_can_has_debugger_original[] = {0x38, 0xB1, 0x05, 0x49, 0x09, 0x68, 0x00, 0x29};
unsigned char kernel__PE_i_can_has_debugger_patch[] = {0x01, 0x20, 0x70, 0x47, 0x09, 0x68, 0x00, 0x29};

unsigned char kernel_IOAESAccelerator_original[] = {0x67, 0xD0, 0x40, 0xF6};
unsigned char kernel_IOAESAccelerator_patch[] = {0x00, 0x20, 0x40, 0xF6};

unsigned char kernel_sigcheck_original[] = {0xFF, 0x31, 0xA7, 0xF1, 0x18, 0x04, 0x08, 0x46, 0xA5, 0x46, 0xBD, 0xE8, 0x00, 0x0D, 0xF0, 0xBD};
unsigned char kernel_sigcheck_patch[] = {0xFF, 0x31, 0xA7, 0xF1, 0x18, 0x04, 0x00, 0x20, 0xA5, 0x46, 0xBD, 0xE8, 0x00, 0x0D, 0xF0, 0xBD};

unsigned char kernel_redsn0w_unknown0_original[] = {0x01, 0xf0, 0x06, 0x00, 0x06, 0x28, 0x04, 0xbf, 0x19, 0x98, 0x00, 0x28, 0x08, 0xd1, 0xdf, 0xf8};
unsigned char kernel_redsn0w_unknown0_patch[] = {0x01, 0xf0, 0x06, 0x00, 0xff, 0x28, 0x04, 0xbf, 0x19, 0x98, 0x00, 0x28, 0x08, 0xd1, 0xdf, 0xf8};

unsigned char kernel_redsn0w_unknown1_original[] = {0x2a, 0xf1, 0xf2, 0xfe, 0xe8, 0x6c, 0x01, 0x69, 0x01, 0x39, 0x01, 0x61, 0xe8, 0x6c, 0x00, 0x69, 0xb0, 0xf1, 0xff, 0x3f, 0xdc, 0xbf, 0x43, 0x48, 0x5e, 0xf7, 0x0e, 0xfb, 0x28, 0x46, 0xe4, 0xf7};
unsigned char kernel_redsn0w_unknown1_patch[] = {0x2a, 0xf1, 0xf2, 0xfe, 0xe8, 0x6c, 0x01, 0x69, 0x01, 0x39, 0x01, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x46, 0xe4, 0xf7};

unsigned char kernel_redsn0w_unknown2_original[] = {0x08, 0x46, 0xbd, 0xe8, 0x00, 0x05, 0xf0, 0xbd, 0x50, 0x61, 0x5e, 0x80, 0x50, 0x65, 0x5e, 0x80};
unsigned char kernel_redsn0w_unknown2_patch[] = {0x01, 0x20, 0xbd, 0xe8, 0x00, 0x05, 0xf0, 0xbd, 0x50, 0x61, 0x5e, 0x80, 0x50, 0x65, 0x5e, 0x80};

unsigned char kernel_redsn0w_unknown3_original[] = {0x7c, 0x4c, 0x05, 0x96, 0xbc, 0xe0, 0x48, 0x98, 0x48, 0xb1, 0x7b, 0x48, 0x00, 0x78, 0x10, 0xf0, 0x04, 0x0f, 0x04, 0xd0, 0x79, 0x48, 0x29, 0x46, 0x79, 0x4e, 0xb0, 0x47, 0xd6, 0xe7, 0x79, 0x4c};
unsigned char kernel_redsn0w_unknown3_patch[] = {0x7c, 0x4c, 0x05, 0x96, 0xbc, 0xe0, 0x48, 0x98, 0x48, 0xb1, 0x7b, 0x48, 0x00, 0x78, 0x01, 0x23, 0x01, 0x23, 0x04, 0xd0, 0x79, 0x48, 0x29, 0x46, 0x79, 0x4e, 0xb0, 0x47, 0xd6, 0xe7, 0x79, 0x4c};

unsigned char kernel_xattr_original[] = {"com.apple.system."};
unsigned char kernel_xattr_patch[] = {"com.apple.aaaaaa."};

patch kernel_CSED = {kernel_CSED_original, kernel_CSED_patch, sizeof(kernel_CSED_original)};
patch kernel_AMFI = {kernel_AMFI_original, kernel_AMFI_patch, sizeof(kernel_AMFI_original)};
patch kernel__PE_i_can_has_debugger = {kernel__PE_i_can_has_debugger_original, kernel__PE_i_can_has_debugger_patch, sizeof(kernel__PE_i_can_has_debugger)};
patch kernel_IOAESAccelerator = {kernel_IOAESAccelerator_original, kernel_IOAESAccelerator_patch, sizeof(kernel_IOAESAccelerator)};
patch kernel_sigcheck = {kernel_sigcheck_original, kernel_sigcheck_patch, sizeof(kernel_sigcheck_patch)};
patch kernel_xattr = {kernel_xattr_original, kernel_xattr_patch, sizeof(kernel_xattr)};

// unknown redsn0w patches
patch kernel_redsn0w_unknown0 = {kernel_redsn0w_unknown0_original, kernel_redsn0w_unknown0_patch, sizeof(kernel_redsn0w_unknown0_original)};
patch kernel_redsn0w_unknown1 = {kernel_redsn0w_unknown1_original, kernel_redsn0w_unknown1_patch, sizeof(kernel_redsn0w_unknown1_original)};
patch kernel_redsn0w_unknown2 = {kernel_redsn0w_unknown2_original, kernel_redsn0w_unknown2_patch, sizeof(kernel_redsn0w_unknown2_original)};
patch kernel_redsn0w_unknown3 = {kernel_redsn0w_unknown3_original, kernel_redsn0w_unknown3_patch, sizeof(kernel_redsn0w_unknown3_original)};

