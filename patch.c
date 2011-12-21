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

extern Dictionary *firmwarePatches, *patchDict, *info;
extern unsigned char iBEC_bootargs_ramdisk[];
extern int UsingRamdisk;

Dictionary* get_key_dictionary_from_bundle(char* member) {
	firmwarePatches = (Dictionary*)getValueByKey(info, "FirmwarePatches");
	patchDict = (Dictionary*) firmwarePatches->values;
	StringValue* fileValue = (StringValue*) getValueByKey(patchDict, "File");

	if(!strcasecmp(patchDict->dValue.key, member))
		return patchDict;

	while(patchDict != NULL) {
		fileValue = (StringValue*) getValueByKey(patchDict, "File");

		if(!strcasecmp(patchDict->dValue.key, member))
			return patchDict;

		patchDict = (Dictionary*) patchDict->dValue.next;
	}

	return NULL;
}

int patch_bootloaders(char* buffer, size_t length) {
	int i;
	
	/* use md0 bootargs if ramdisk */
	if(UsingRamdisk == TRUE) 
		iBEC_bootargs.patched = iBEC_bootargs_ramdisk;

	for(i = 0; i < length; i++) {
		char* candidate = &buffer[i];
		if(!memcmp(candidate, iBSS_SDOM.original, iBSS_SDOM.length)) {
			printf("Patching bootloader SDOM check... at 0x%08x\n", i);
			memcpy(candidate, iBSS_SDOM.patched, iBSS_SDOM.length);
			continue;
		}
		if(!memcmp(candidate, iBSS_PROD.original, iBSS_PROD.length)) {
			printf("Patching bootloader PROD check... at 0x%08x\n", i);
			memcpy(candidate, iBSS_PROD.patched, iBSS_PROD.length);
			continue;
		}
		if(!memcmp(candidate, iBSS_CHIP.original, iBSS_CHIP.length)) {
			printf("Patching bootloader CHIP check... at 0x%08x\n", i);
			memcpy(candidate, iBSS_CHIP.patched, iBSS_CHIP.length);
			continue;
		}
		if(!memcmp(candidate, iBSS_TYPE.original, iBSS_TYPE.length)) {
			printf("Patching bootloader TYPE check... at 0x%08x\n", i);
			memcpy(candidate, iBSS_TYPE.patched, iBSS_TYPE.length);
			continue;
		}
		if(!memcmp(candidate, iBSS_SEPO.original, iBSS_SEPO.length)) {
			printf("Patching bootloader SEPO check... at 0x%08x\n", i);
			memcpy(candidate, iBSS_SEPO.patched, iBSS_SEPO.length);
			continue;
		}
		if(!memcmp(candidate, iBSS_CEPO.original, iBSS_CEPO.length)) {
			printf("Patching bootloader CEPO check... at 0x%08x\n", i);
			memcpy(candidate, iBSS_CEPO.patched, iBSS_CEPO.length);
			continue;
		}
		if(!memcmp(candidate, iBSS_BORD.original, iBSS_BORD.length)) {
			printf("Patching bootloader BORD check... at 0x%08x\n", i);
			memcpy(candidate, iBSS_BORD.patched, iBSS_BORD.length);
			continue;
		}
		if(!memcmp(candidate, iBSS_ECID.original, iBSS_ECID.length)) {
			printf("Patching bootloader ECID check... at 0x%08x\n", i);
			memcpy(candidate, iBSS_ECID.patched, iBSS_ECID.length);
			continue;
		}
		if(!memcmp(candidate, iBSS_SHSH.original, iBSS_SHSH.length)) {
			printf("Patching bootloader SHSH check... at 0x%08x\n", i);
			memcpy(candidate, iBSS_SHSH.patched, iBSS_SHSH.length);
			continue;
		}
		if(!memcmp(candidate, iBEC_bootargs.original, iBEC_bootargs.length)) {
			printf("Patching bootargs... at 0x%08x\n", i);
			memcpy(candidate, iBEC_bootargs.patched, iBEC_bootargs.length);
			continue;
		}
		if(!memcmp(candidate, iBEC_bootargs_jmp.original, iBEC_bootargs_jmp.length)) {
			printf("Patching bootargs check... at 0x%08x\n", i);
			memcpy(candidate, iBEC_bootargs_jmp.patched, iBEC_bootargs_jmp.length);
			continue;
		}
	}
	return 0;
}

int patch_kernel(char* buffer, size_t length) {
	int i;
	for(i = 0; i < length; i++) {
		char* candidate = &buffer[i];
		if(!memcmp(candidate, kernel_CSED.original, kernel_CSED.length)) {
			printf("Patching kernel CSED check... at 0x%08x\n", i);
			memcpy(candidate, kernel_CSED.patched, kernel_CSED.length);
			continue;
		}
		if(!memcmp(candidate, kernel_AMFI.original, kernel_AMFI.length)) {
			printf("Patching kernel AMFI check... at 0x%08x\n", i);
			memcpy(candidate, kernel_AMFI.patched, kernel_AMFI.length);
			continue;
		}
		if(!memcmp(candidate, kernel__PE_i_can_has_debugger.original, kernel__PE_i_can_has_debugger.length)) {
			printf("Patching kernel _i_can_has_debugger check... at 0x%08x\n", i);
			memcpy(candidate, kernel__PE_i_can_has_debugger.patched, kernel__PE_i_can_has_debugger.length);
			continue;
		}
		if(!memcmp(candidate, kernel_IOAESAccelerator.original, kernel_IOAESAccelerator.length)) {
			printf("Patching kernel IOAESAccelerator check... at 0x%08x\n", i);
			memcpy(candidate, kernel_IOAESAccelerator.patched, kernel_IOAESAccelerator.length);
			continue;
		}
		if(!memcmp(candidate, kernel_sigcheck.original, kernel_sigcheck.length)) {
			printf("Patching kernel signature enforcement check... at 0x%08x\n", i);
			memcpy(candidate, kernel_sigcheck.patched, kernel_sigcheck.length);
			continue;
		}
		if(!memcmp(candidate, kernel_xattr.original, kernel_xattr.length)) {
			printf("Patching kernel xattr check... at 0x%08x\n", i);
			memcpy(candidate, kernel_xattr.patched, kernel_xattr.length);
			continue;
		}
		if(!memcmp(candidate, kernel_redsn0w_unknown0.original, kernel_redsn0w_unknown0.length)) {
			printf("Patching unknown kernel check (redsn0w 0)... at 0x%08x\n", i);
			memcpy(candidate, kernel_redsn0w_unknown0.patched, kernel_redsn0w_unknown0.length);
			continue;
		}
		if(!memcmp(candidate, kernel_redsn0w_unknown1.original, kernel_redsn0w_unknown1.length)) {
			printf("Patching unknown kernel check (redsn0w 1)... at 0x%08x\n", i);
			memcpy(candidate, kernel_redsn0w_unknown1.patched, kernel_redsn0w_unknown1.length);
			continue;
		}
		if(!memcmp(candidate, kernel_redsn0w_unknown2.original, kernel_redsn0w_unknown2.length)) {
			printf("Patching unknown kernel check (redsn0w 2)... at 0x%08x\n", i);
			memcpy(candidate, kernel_redsn0w_unknown2.patched, kernel_redsn0w_unknown2.length);
			continue;
		}
		if(!memcmp(candidate, kernel_redsn0w_unknown3.original, kernel_redsn0w_unknown3.length)) {
			printf("Patching unknown kernel check (redsn0w 3)... at 0x%08x\n", i);
			memcpy(candidate, kernel_redsn0w_unknown3.patched, kernel_redsn0w_unknown3.length);
			continue;
		}
	}
	return 0;
}

int patch_file(char* filename) {
	AbstractFile *template = NULL, *inFile, *certificate = NULL, *outFile, *newFile;
	unsigned int* key = NULL;
	unsigned int* iv = NULL;
	char* inData;
	size_t inDataSize;
	char *buffer;
	Dictionary* data;

	template = createAbstractFileFromFile(fopen(filename, "rb"));

	/* open base template */
	if(!template) {
		printf("Cannot open template.\n");
		return -1;
	}

	printf("getting keys\n");
	data = get_key_dictionary_from_bundle(filename);
	StringValue* keyValue = (StringValue*) getValueByKey(data, "Key");
	StringValue* ivValue = (StringValue*) getValueByKey(data, "IV");

	if(keyValue) {
		size_t bytes;
		printf("Key for %s: %s\n", filename, keyValue->value);
		hexToInts(keyValue->value, &key, &bytes);
	}

	if(ivValue) {
		size_t bytes;
		printf("IV for %s: %s\n", filename, ivValue->value);
		hexToInts(ivValue->value, &iv, &bytes);
	}

	/* open file */
	inFile = openAbstractFile2(createAbstractFileFromFile(fopen(filename, "rb")), key, iv);
	if(!inFile) {
		printf("Cannot open %s.\n", filename);
		return -1;
	}

	/* read it */
	printf("reading data from initial abstract\n");
	inDataSize = (size_t) inFile->getLength(inFile);
	inData = (char*) malloc(inDataSize);
	inFile->read(inFile, inData, inDataSize);
	inFile->close(inFile);

	/* debug 8) */
	hex_dump(inData, 0x240);

	/* zero buffer */
	buffer = malloc(strlen(filename) + 5);
	if(!buffer) {
		printf("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 5);

	snprintf(buffer, strlen(filename) + 5, "%s.pwn", filename);

	/* open output */
	printf("opening %s (output) as an abstract file\n", filename);

	outFile = createAbstractFileFromFile(fopen(buffer, "wb"));
	if(!outFile) {
		printf("Cannot open outfile\n");
		return -1;
	}

	printf("pwned file is %s, will upload later\n", buffer);

	newFile = duplicateAbstractFile2(template, outFile, key, iv, certificate);
	if(!newFile) {
		printf("Cannot open newfile\n");
		return -1;
	}

	/* pwn it 8) */
	printf("pwning %s\n", filename);

	if(!strcasecmp(filename, "iBEC") || !strcasecmp(filename, "iBSS"))
		patch_bootloaders(inData, inDataSize);
	else if(!strcasecmp(filename, "kernelcache"))
		patch_kernel(inData, inDataSize);

	/* write patched contents */
	printf("writing pwned file\n");
	newFile->write(newFile, inData, inDataSize);
	newFile->close(newFile);

	free(buffer);

	return 0;
}
