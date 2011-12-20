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

bool verboseflag = false;
irecv_device_t device = NULL;
irecv_client_t client = NULL;
Dictionary *firmwarePatches, *patchDict, *info;
int Img3DecryptLast = TRUE;

#define usage(x) \
	printf("Usage: %s [OPTION]\n" \
			"Jailbreak an iOS device (iPhones/iPod touches/iPads)\n" \
			"Supported devices are: %s\n" \
			"\n" \
			"Options:\n" \
			"\t-v                           Verbose mode. Useful for debugging.\n" \
			"\t-w url                       Get necessary files from a remote IPSW.\n" \
			"\t-p plist                     Use firmware plist\n" \
			"\t-h                           Help.\n" \
			"\t-k kernelcache               Boot using specified kernel.\n" \
			"\t-i ipsw                      Use specified ipsw to retrieve files from\n" \
			"\t-b bootlogo.img3             Use specified bootlogo img3 file during startup.\n" \
			"\n", \
			argv[0], \
			"A4 devices"); \
			exit(-1);

bool file_exists(const char* fileName) {
	struct stat buf;
	return !stat(fileName, &buf);
}

int poll_device_for_dfu() {
	irecv_error_t err;
	static int try;

	err = irecv_open(&client);
	if (err != IRECV_E_SUCCESS) {
		printf("\rConnect the device in DFU mode. [%d]", try);
		try++;
		return 1;
	}

	if (client->mode != kDfuMode) {
		printf("\rConnect the device in DFU mode. [%d]", try);
		irecv_close(client);
		try++;
		return 1;
	}

	return 0;
}

int fetch_image(const char* path, const char* output) {
	printf("Fetching %s...\n", path);
	if (download_file_from_zip(device->url, path, output, NULL)
			!= 0) {
		printf("Unable to fetch %s\n", path);
		return -1;
	}

	return 0;
}

int upload_image(char* filename, int mode) {
	char path[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;
	char *buffer;

	printf("Checking if %s already exists\n", filename);

	memset(path, 0, 255);

	switch(mode) {
		case 0:	/* dfu */
			snprintf(path, 255, "Firmware/dfu/%s.%s.RELEASE.dfu", filename, device->model);
			printf("dfu binary: IPSW path is %s\n", path);
			if (stat(filename, &buf) != 0) {
				if (fetch_image(path, filename) < 0) {
					printf("Unable to upload DFU image\n");
					return -1;
				}
			}
			break;
		case 1: /* all_flash */
			snprintf(path, 255, "Firmware/all_flash/all_flash.%s.production/%s.%s.img3", 
       	          device->model, filename, device->model);
			printf("all_flash binary: IPSW path is %s\n", path);
			if (stat(filename, &buf) != 0) {
				if (fetch_image(path, filename) < 0) {
					printf("Unable to upload DFU image\n");
					return -1;
				}
			}
			break;
		case 2: /* logos */
			snprintf(path, 255, "Firmware/all_flash/all_flash.%s.production/%s.s5l%dx.img3", 
       	          device->model, filename, device->chip_id);
			printf("logo binary: IPSW path is %s\n", path);
			if (stat(filename, &buf) != 0) {
				if (fetch_image(path, filename) < 0) {
					printf("Unable to upload DFU image\n");
					return -1;
				}
			}
			break;
		case 3: /* kernelcache */
			snprintf(path, 255, "%s.release.%c%c%c", 
       	          filename, device->model[0], device->model[1], device->model[2]);
			printf("kernel binary: IPSW path is %s\n", path);
			if (stat(filename, &buf) != 0) {
				if (fetch_image(path, filename) < 0) {
					printf("Unable to upload DFU image\n");
					return -1;
				}
			}
			break;
	}

	if(client->mode != kDfuMode) {
		printf("Resetting device counters\n");
		error = irecv_reset_counters(client);
		if (error != IRECV_E_SUCCESS) {
			printf("Unable to upload firmware image\n");
			printf("%s\n", irecv_strerror(error));	
			return -1;
		}
	}

	patch_file(filename);

	buffer = malloc(strlen(filename) + 5);
	if(!buffer) {
		printf("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 5);

	snprintf(buffer, strlen(filename) + 5, "%s.pwn", filename);

	printf("Uploading %s to device\n", buffer);
	error = irecv_send_file(client, buffer, 1);
	if (error != IRECV_E_SUCCESS) {
		printf("%s\n", irecv_strerror(error));
		return -1;
	}
	return 0;
}

int main(int argc, char **argv) {
	int c;
	char *ipsw = NULL, *kernelcache = NULL, *bootlogo = NULL, *url = NULL, *plist = NULL;
	irecv_error_t err = IRECV_E_SUCCESS;
	AbstractFile* plistFile;

	printf("opensn0w, an open source jailbreaking program.\n"
		   "Compiled on: " __DATE__ " " __TIME__ "\n\n");

	opterr = 0;

	while ((c = getopt (argc, argv, "vhp:b:w:k:i:")) != -1) {
		switch (c) {
		case 'v':
			verboseflag = true;
			break;
		case 'h':
			usage();
			break;
		case 'p':
			plist = optarg;
		case 'i':
			if (!file_exists(optarg)) {
				printf("Cannot open IPSW file '%s'\n", ipsw);
				return -1;
			}
			ipsw = optarg;
			break;
		case 'k':
			if (!file_exists(optarg)) {
				printf("Cannot open kernelcache file '%s'\n", optarg);
				return -1;
			}
			kernelcache = optarg;
			break;
		case 'w':
			url = optarg;
			break;
		case 'b':
			if (!file_exists(optarg)) {
				printf("Cannot open bootlogo file '%s'\n", optarg);
				return -1;
			}
			bootlogo = optarg;
			break;
		default:
			usage();
		}
	}

	if(!plist) {
		printf("The plist is sort of required now.\n");
		return -1;
	}

	if((plistFile = createAbstractFileFromFile(fopen(plist, "rb"))) != NULL) {
		plist = (char*) malloc(plistFile->getLength(plistFile));
		plistFile->read(plistFile, plist, plistFile->getLength(plistFile));
		plistFile->close(plistFile);
		info = createRoot(plist);
	}

	/* to be done */
	printf("Initializing libirecovery\n");
	irecv_init();

//#ifdef DEBUG
	irecv_set_debug_level(3);
//#endif

	/* Poll for DFU mode */
	while(poll_device_for_dfu()) {
		sleep(1);
	}
	puts("");

	/* Got the handle */
	printf("So we have a handle! :-)\n");

	/* Check the device */
	err = irecv_get_device(client, &device);
	if (device == NULL || device->index == DEVICE_UNKNOWN) {
		printf("Bad device. errno %d\n", err);
		return -1;
	}

	printf("Device found: name: %s, processor s5l%dxsi\n", device->product, device->chip_id);
	printf("iBoot information: %s\n", client->serial);

	/* What jailbreak exploit is this thing capable of? */
	if(device->chip_id == 8930 || device->chip_id == 8922 || device->chip_id == 8920) {
		printf("This device is compatible with the limera1n exploit. Sending.\n");
		err = limera1n();
		if(err) {
			printf("Error during limera1ning.\n");
			exit(-1);
		}
	} else {
		printf("Support for the S5L%dX isn't done yet.\n", device->chip_id);
	}

	/* We are owned now! */
	printf("Bootrom is pwned now! :D\n");

	/* upload iBSS */

	/* FOR SOME REASON: THIS BREAKS ON 5.0. iBEC is not being uploaded properly. */

	upload_image("iBSS", 0);
	client = irecv_reconnect(client, 10);

	upload_image("iBEC", 0);
	client = irecv_reconnect(client, 10);

	irecv_reset(client);
	sleep(10);
	client = irecv_reconnect(client, 10);
	irecv_set_interface(client, 0, 0);

	/* upload kernel */
	irecv_send_command(client, "go kernel bootargs -v rd=disk0s1s1 keepsyms=1");
	irecv_send_command(client, "kernel bootargs -v rd=disk0s1s1 keepsyms=1");

	irecv_set_interface(client, 1, 1);

	upload_image("kernelcache", 3);
	client = irecv_reconnect(client, 10);

	printf("booting\n");
	irecv_send_command(client, "bootx");

	/* device is ready to run */

	printf("to be completed\n");

	return 0;
}
