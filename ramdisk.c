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

#ifdef _GUI_ENABLE_
#ifdef _WIN32
#include <commctrl.h>
#include <windowsx.h>
extern HWND hStatus0, hStatus1, hStatus2, progress, window;
#endif
#endif

extern volatile int dry_run;
extern irecv_device_t device;

/*!
 * \fn void inject_files(char* buffer, int bufsize, char* filename) g
 * \brief Inject files into RAM disk.
 *
 * \param buffer Raw ramdisk buffer
 * \param bufsize Size of ramdisk
 * \param filename Filename prefix.
 */
int inject_files(char* buffer, int bufsize, char* filename) {
	FILE *fp;
	int len;
	char strbuf[255];
	const char *rmall_0[] = {NULL, NULL, "rm", "/usr/local/standalone/firmware/ICE2.Release.bbfw"};
	const char *addall_0[] = {NULL, NULL, "addall", "./mythos/binaries/bin/", "/bin"};
	const char *restored_rm[] = {NULL, NULL, "rm", "/usr/local/bin/restored_external"};
	const char *restored_add[] = {NULL, NULL, "add", "./mythos/binaries/usr/local/bin/restored_external", "/usr/local/bin/"};
	const char *add_libs[] = {NULL, NULL, "addall", "./mythos/binaries/usr/lib", "/usr/lib/"};

	snprintf(strbuf, 255, "%s.tmp", filename);

	rmall_0[1] = strbuf;
	addall_0[1] = strbuf;
	restored_rm[1] = strbuf;
	restored_add[1] = strbuf;
	add_libs[1] = strbuf;


	fp = fopen(strbuf, "wb");
	fwrite(buffer, bufsize, 1, fp);
	fclose(fp);

	hfs_main(4, rmall_0);
	hfs_main(5, addall_0);
	hfs_main(4, restored_rm);
	hfs_main(5, restored_add);
	hfs_main(5, add_libs);

	fp = fopen(strbuf, "rb");
	fseek(fp, 0, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	buffer = realloc(buffer, len);
	fread(buffer, len, 1, fp);
	fclose(fp);
	unlink(buffer);
    
	return len;
}

/*!
 * \fn int prepare_ramdisk(char* filename)
 * \brief Prepare ramdisk.
 *
 * \param filename Ramdisk filename.
 */
int prepare_ramdisk(char* filename) {
	AbstractFile *template = NULL, *inFile, *certificate =
	    NULL, *outFile, *newFile;
	unsigned int *key = NULL;
	unsigned int *iv = NULL;
	char *inData;
	size_t inDataSize;
	char *buffer;
	Dictionary *data;
	StringValue *keyValue;
	StringValue *ivValue;

	template = createAbstractFileFromFile(fopen(filename, "rb"));

	/* open base template */
	if (!template) {
		ERR("Cannot open template.\n");
		return -1;
	}

	DPRINT("getting keys\n");

	data = get_key_dictionary_from_bundle("RestoreRamdisk");
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

	/* zero buffer */
	buffer = malloc(strlen(filename) + 50);
	if (!buffer) {
		ERR("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 50);

	snprintf(buffer, strlen(filename) + 50, "%s.jb_ramdisk", filename);
	unlink(buffer);

	/* open output */
	DPRINT("opening %s (output) as an abstract file\n", filename);

	outFile = createAbstractFileFromFile(fopen(buffer, "wb"));
	if (!outFile) {
		DPRINT("Cannot open outfile\n");
		return -1;
	}

	DPRINT("jb ramdisk file is %s, will upload later\n", buffer);

	newFile =
	    duplicateAbstractFile2(template, outFile, key, iv, certificate);
	if (!newFile) {
		DPRINT("Cannot open newfile\n");
		return -1;
	}

	DPRINT("injecting files...\n");
	inDataSize = inject_files(inData, inDataSize, buffer);

	/* write patched contents */
	DPRINT("writing pwned file\n");

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


/*!
 * \fn int make_ramdisk(firmware_item item)
 * \brief Create jailbreak RAM disk.
 * 
 * \param item Restore ramdisk item
 */
int make_ramdisk(firmware_item item) {
	char path[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;
	char buffer[255];

	snprintf(buffer, 255, "jailbreak_ramdisk.s5l%dx", device->chip_id);

	DPRINT("Checking if %s already exists\n", buffer);

	memset(path, 0, 255);

	if (stat(buffer, &buf) != 0) {
#ifdef _WIN32
#ifdef _GUI_ENABLE_
		SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Downloading from Apple..."));
		SendMessage(progress, PBM_SETPOS, 0, 0);
		SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) "Base RAM disk");
		SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) " ");
		InvalidateRect(window, NULL, TRUE);
#endif
#endif
		if (fetch_image(item.name, buffer) < 0) {
			ERR("Unable to upload DFU image\n");
			return -1;
		}
	}

#ifdef _WIN32
#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Adding core jailbreak files..."));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) " ");
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) " ");
	InvalidateRect(window, NULL, TRUE);
#endif
#endif

	DPRINT("Preparing ramdisk...\n");
	prepare_ramdisk(buffer);
    
#ifdef _WIN32
#ifdef _GUI_ENABLE_
	SendMessage(hStatus0, WM_SETTEXT, 0, (LPARAM) TEXT("Uploading..."));
	SendMessage(progress, PBM_SETPOS, 0, 0);
	SendMessage(hStatus1, WM_SETTEXT, 0, (LPARAM) "Jailbreak RAM disk");
	SendMessage(hStatus2, WM_SETTEXT, 0, (LPARAM) " ");
	InvalidateRect(window, NULL, TRUE);
#endif
#endif

	snprintf(buffer, 255, "jailbreak_ramdisk.s5l%dx.jb_ramdisk", device->chip_id);

	if(!dry_run) {
		if (client->mode != kDfuMode)
			error = irecv_send_file(client, buffer, 0);
		else
 			error = irecv_send_file(client, buffer, 1);
	}

	if (error != IRECV_E_SUCCESS) {
		ERR("%s\n", irecv_strerror(error));
		return -1;
 	}

#ifdef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif

	irecv_send_command(client, "ramdisk");
#ifdef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif
	return 0;
}
