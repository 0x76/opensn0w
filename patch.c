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

#include "sn0w.h"

#undef HAVE_LIBBZ2
/* preprocessor macros */

#ifdef _GUI_ENABLE_
#ifdef _WIN32
extern HWND window, hStatus2;
#define PATCH_FILE(candidate, original_data, name) \
	if(!memcmp(candidate, original_data.original, original_data.length)) { \
		char buffer[256]; \
		sprintf(buffer, "Patching check at 0x%x...\n", i); \
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) buffer); \
		InvalidateRect(window, NULL, TRUE); \
		DPRINT("Patching " name " check at 0x%08x\n", i); \
		memcpy(candidate, original_data.patched, original_data.length); \
		continue; \
	}

#endif
#else 
#define PATCH_FILE(candidate, original_data, name) \
	if(!memcmp(candidate, original_data.original, original_data.length)) { \
		DPRINT("Patching " name " check at 0x%08x\n", i); \
		memcpy(candidate, original_data.patched, original_data.length); \
		continue; \
	}
#endif



/* globals */
extern Dictionary *firmwarePatches, *patchDict, *info;
extern unsigned char iBEC_bootargs_ramdisk[];
extern int UsingRamdisk;
extern char* version;

/*!
 * \fn Dictionary *get_key_dictionary_from_bundle(char *member)
 * \brief Get \a member from firmware property list
 * 
 * \param member Plist Member
 */
 
Dictionary *get_key_dictionary_from_bundle(char *member)
{
	firmwarePatches = (Dictionary *) getValueByKey(info, "FirmwareKeys");
	patchDict = (Dictionary *) firmwarePatches->values;

	if (!strcasecmp(patchDict->dValue.key, member))
		return patchDict;

	while (patchDict != NULL) {
		if (!strcasecmp(patchDict->dValue.key, member))
			return patchDict;

		patchDict = (Dictionary *) patchDict->dValue.next;
	}

	return NULL;
}

/*!
 * \fn int patch_devtree(char *buffer, size_t length)
 * \brief Patch firmware DeviceTree.
 * 
 * \param buffer Decrypted image.
 * \param length Image length.
 */
 
int patch_devtree(char *buffer, size_t length)
{
	int i;
	for (i = 0; i < length; i++) {
		char *candidate = &buffer[i];
		PATCH_FILE(candidate, devicetree_root_name, "DeviceTree secure-root");
	}
	return 0;
}

/*!
 * \fn int patch_bootloaders(char *buffer, size_t length)
 * \brief Patch firmware iBSS/iBEC.
 * 
 * \param buffer Decrypted image.
 * \param length Image length.
 */

int patch_bootloaders(char *buffer, size_t length)
{
	int i;

	/* use md0 bootargs if ramdisk */
	if (UsingRamdisk == TRUE)
		iBEC_bootargs.patched = iBEC_bootargs_ramdisk;

	for (i = 0; i < length; i++) {
		char *candidate = &buffer[i];
		PATCH_FILE(candidate, iBSS_SDOM, "SDOM tag");
		PATCH_FILE(candidate, iBSS_PROD, "PROD tag");
		PATCH_FILE(candidate, iBSS_CHIP, "CHIP tag");
		PATCH_FILE(candidate, iBSS_TYPE, "TYPE tag");
		PATCH_FILE(candidate, iBSS_SEPO, "SEPO tag");
		PATCH_FILE(candidate, iBSS_CEPO, "CEPO tag");
		PATCH_FILE(candidate, iBSS_BORD, "BORD tag");
		PATCH_FILE(candidate, iBSS_ECID, "ECID tag");
		PATCH_FILE(candidate, iBSS_SHSH, "SHSH tag");
		PATCH_FILE(candidate, iBEC_bootargs, "iBoot boot-args");
		PATCH_FILE(candidate, iBEC_bootargs_jmp, "iBoot boot-args conditional");
	}
	return 0;
}

/*!
 * \fn int patch_kernel(char *buffer, size_t length)
 * \brief Patch firmware kernelcache.
 * 
 * \param buffer Decrypted image.
 * \param length Image length.
 */
 
int patch_kernel(char *buffer, size_t length)
{
	int i;
	for (i = 0; i < length; i++) {
		char *candidate = &buffer[i];
		PATCH_FILE(candidate, kernel_CSED, "Code signature enforcement");
		PATCH_FILE(candidate, kernel_AMFI, "AMFIv2 binary");
		PATCH_FILE(candidate, kernel__PE_i_can_has_debugger, "_PE_i_can_has_debugger");
		PATCH_FILE(candidate, kernel_IOAESAccelerator, "IOAESAccelerator usage");
		PATCH_FILE(candidate, kernel_sigcheck, "Code signature enforcement 2");
		PATCH_FILE(candidate, kernel_vm, "vm");
		PATCH_FILE(candidate, kernel_spec_close, 
"spec_close");
		PATCH_FILE(candidate, kernel_sandbox, 
"sandbox");
		PATCH_FILE(candidate, kernel_nor0, "NOR patch 0");
		PATCH_FILE(candidate, kernel_nor1, "NOR patch 1");
		PATCH_FILE(candidate, kernel_nor2, "NOR patch 2");
		PATCH_FILE(candidate, kernel_nor3, "NOR patch 3");
		PATCH_FILE(candidate, kernel_nor4, "NOR patch 4");
	}
	return 0;
}

/*!
 * \fn int patch_file(char *filename)
 * \brief Patch firmware binary
 * 
 * \param filename Filename to decrypt and patch.
 */
 
int patch_file(char *filename)
{
	AbstractFile *template = NULL, *inFile, *certificate =
	    NULL, *outFile, *newFile, *outFile2, *newFile2;
	unsigned int *key = NULL;
	unsigned int *iv = NULL;
	char *inData;
	size_t inDataSize;
	char *buffer;
	Dictionary *data;
	char *buf;
	char *tokenizedname;
#ifdef HAVE_LIBBZ2
	char *truename = NULL, *name2 = NULL;
	char *dup2 = strndup(filename, 255);
	char *build_train = strndup(version, 255);
#endif
	char *dup = strndup(filename, 255);
	StringValue *keyValue;
	StringValue *ivValue;

	template = createAbstractFileFromFile(fopen(filename, "rb"));

	/* open base template */
	if (!template) {
		ERR("Cannot open template.\n");
		return -1;
	}

	DPRINT("getting keys\n");

	tokenizedname = strtok(dup, ".,");
#ifdef HAVE_LIBBZ2
	truename = strtok(dup2, "_");
	name2 = strtok(build_train, "_"); /* device */
	name2 = strtok(NULL, "_");        /* version */
	name2 = strtok(NULL, "_");        /* build */
#endif

	data = get_key_dictionary_from_bundle(tokenizedname);
	keyValue = (StringValue *) getValueByKey(data, "Key");
	ivValue = (StringValue *) getValueByKey(data, "IV");

	if (keyValue) {
		size_t bytes;
		DPRINT("Key for %s: %s\n", filename, keyValue->value);
		hexToInts(keyValue->value, &key, &bytes);
	}

	if (ivValue) {
		size_t bytes;
		DPRINT("IV for %s: %s\n", filename, ivValue->value);
		hexToInts(ivValue->value, &iv, &bytes);
	}

	/* open file */
	inFile =
	    openAbstractFile2(createAbstractFileFromFile(fopen(filename, "rb")),
			      key, iv);
	if (!inFile) {
		DPRINT("Cannot open %s.\n", filename);
		return -1;
	}

	/* read it */
	DPRINT("reading data from initial abstract\n");
	inDataSize = (size_t) inFile->getLength(inFile);
	inData = (char *)malloc(inDataSize);
	inFile->read(inFile, inData, inDataSize);
	inFile->close(inFile);

	/* debug 8) */
	//hex_dump(inData, 0x240);

	/* zero buffer */
	buffer = malloc(strlen(filename) + 5 + strlen(version));
	if (!buffer) {
		ERR("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 5 + strlen(version));

	/* zero buffer */
	buf = malloc(strlen(filename) + 8 + strlen(version));
	if (!buf) {
		ERR("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 8 + strlen(version));

	snprintf(buffer, strlen(filename) + 8 + strlen(version), "%s.pwn", filename);
	snprintf(buf, strlen(filename) + 8 + strlen(version), "%s.dec", filename);
	unlink(buf);
	unlink(buffer);

	/* open output */
	DPRINT("opening %s (output) as an abstract file\n", filename);

	outFile = createAbstractFileFromFile(fopen(buffer, "wb"));
	if (!outFile) {
		DPRINT("Cannot open outfile\n");
		return -1;
	}

	outFile2 = createAbstractFileFromFile(fopen(buf, "wb"));
	if (!outFile2) {
		DPRINT("Cannot open outfile\n");
		return -1;
	}

	DPRINT("pwned file is %s, will upload later\n", buffer);

	newFile =
	    duplicateAbstractFile2(template, outFile, key, iv, certificate);
	if (!newFile) {
		DPRINT("Cannot open newfile\n");
		return -1;
	}

	newFile2 = outFile2;
	if (!newFile2) {
		DPRINT("Cannot open newfile\n");
		return -1;
	}

	/* pwn it 8) */
	DPRINT("pwning %s\n", filename);

	if (strcasestr(filename, "iBEC") || strcasestr(filename, "iBSS") || strcasestr(filename, "iBoot"))
		patch_bootloaders(inData, inDataSize);
	else if (strcasestr(filename, "kernelcache"))
		patch_kernel(inData, inDataSize);
	else if (strcasestr(filename, "DeviceTree"))
		patch_devtree(inData, inDataSize);

	/* write patched contents */
	DPRINT("writing pwned file\n");

	newFile2->write(newFile2, inData, inDataSize);
	newFile2->close(newFile2);

#ifdef HAVE_LIBBZ2
	/* bsdiff */
	if(name2[0] != '9') {
		char patchbuf[255], newname[255];

		snprintf(patchbuf, 255, "patches/%s.%s/%s.patch", device->model, name2, truename);
		snprintf(newname, 255, "%s.pwn_bsdiff", filename);
		if(file_exists(patchbuf) == TRUE) {
			DPRINT("Running bspatch(%s, %s, %s);\n", buf, newname, patchbuf);
			bsdiff(buf, newname, patchbuf);
		}

		newFile2 = createAbstractFileFromFile(fopen(newname, "rb"));
		if (!newFile2) {
			DPRINT("Cannot open outfile\n");
			return -1;
		}
		newFile2->read(newFile2, inData, inDataSize);
		newFile2->close(newFile2);
		unlink(newname);
	}
#endif
	newFile->write(newFile, inData, inDataSize);

	newFile->close(newFile);

#ifndef MSVC_VER
	free(buffer);	/* I know this is strange, and malpractice, not freeing the buffer...
			   but on MSVC, and my Windows 7 installation, the heap handler tends to 
			   destroy the program state here. I have no clue why. -- acfrazier */
			/* addendum: At most, like 1kB will be lost. Does it really matter? */
#endif

	return 0;
}
