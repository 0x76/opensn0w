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

int UsingRamdisk = FALSE;

#define usage(x) \
	printf("Usage: %s [OPTIONS]\n" \
			"Jailbreak an iOS device, this includes the iPhone, iPad, iPod touch and\n" \
			"Apple TV 2G.\n\n" \
			"Currently supported devices are: %s\n" \
			"\n" \
			"Options:\n" \
			"   -v                 Verbose mode. Useful for debugging.\n" \
			"   -w url             Get necessary files from a remote IPSW.\n" \
			"   -p plist           Use firmware plist\n" \
			"   -h                 Help.\n" \
			"   -k kernelcache     Boot using specified kernel.\n" \
			"   -i ipsw            Use specified ipsw to retrieve files from\n" \
			"   -b bootlogo.img3   Use specified bootlogo img3 file during startup.\n" \
			"   -r ramdisk.dmg     Boot specified ramdisk.\n" \
			"   -d                 Just pwn dfu mode.\n" \
			"   -a [boot-args]     Set device boot-args for boot.\n" \
			"\n" \
			"Exit status:\n" \
			"  0  if OK,\n" \
			" -1  if severe issues.\n" \
			"\n" \
			"Report %s bugs to rms@velocitylimitless.org\n" \
			"%s homepage: <http://www.opensn0w.com>\n" \
			"For complete documentation, see the UNIX manual. \n<man page needs to be written>\n", \
			argv[0], \
			"s5l8930x, s5l8920x, s5l8922x, s5l8720x", argv[0], argv[0]); \
			exit(-1);

char* image_names[] = {
    "iBSS",
    "DeviceTree",
    "BatteryCharging1",
    "GlyphCharging",
    "BatteryCharging0",
    "iBoot",
    "BatteryLow0",
    "LLB",
    "iBEC",
    "KernelCache"
    "FileSystem",
    "AppleLogo",
    "UpdateRamdisk",
    "RestoreRamdisk",
    "GlyphPlugin",
    "Recovery",
    "BatteryLow1"
};

char* image_paths[] = {"Firmware/all_flash/all_flash.%s.production/%s", "Firmware/dfu/%s", "%s"};

Dictionary *get_key_dictionary_from_bundle(char *member);

void boot_args_process(char *args) {
	char buffer[39];

	if (strlen(args) > 39) {
		printf
		    ("Boot-args is too long. Please shorten to 39 characters or less.\n");
		exit(-1);
	}

	/* pad */
	snprintf(buffer, 39, "%-39s", args);

	printf("Booting with boot-args \"%s\"\n", buffer);

	memcpy(iBEC_bootargs.patched, buffer, 39);
}

bool file_exists(const char *fileName) {
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

int fetch_image(const char *path, const char *output) {
	printf("Fetching %s...\n", path);
	if (download_file_from_zip(device->url, path, output, NULL) != 0) {
		printf("Unable to fetch %s\n", path);
		return -1;
	}

	return 0;
}

size_t writeData(void *ptr, size_t size, size_t mem, FILE *stream) {
	size_t written;
	written = fwrite(ptr, size, mem, stream);
	return written;
}

int upload_image(firmware_item item, int mode, int patch) {
	char path[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;
	char *buffer;
    char* filename = item.name;

	printf("Checking if %s already exists\n", filename);

	memset(path, 0, 255);

	switch(item.flags) {
		case ALL_FLASH:
            sprintf(path, image_paths[ALL_FLASH], device->model, item.name);
			if (stat(filename, &buf) != 0) {
				if (fetch_image(path, filename) < 0) {
					printf("Unable to upload DFU image\n");
					return -1;
					}
			}
		case DFU:
            sprintf(path, image_paths[DFU], item.name);
			if (stat(filename, &buf) != 0) {
				if (fetch_image(path, filename) < 0) {
					printf("Unable to upload DFU image\n");
					return -1;
                }
			}
		case ROOT:
            sprintf(path, image_paths[ROOT], item.name);
			if (stat(filename, &buf) != 0) {
				if (fetch_image(path, filename) < 0) {
					printf("Unable to upload DFU image\n");
					return -1;
                }
			}
    }
    
	if (patch)
		patch_file(filename);

	buffer = malloc(strlen(filename) + 5);
	if (!buffer) {
		printf("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 5);

	if (patch)
		snprintf(buffer, strlen(filename) + 5, "%s.pwn", filename);
	else
		snprintf(buffer, strlen(filename) + 5, "%s", filename);

	printf("Uploading %s to device\n", buffer);

	if (client->mode != kDfuMode)
		error = irecv_send_file(client, buffer, 0);
	else
		error = irecv_send_file(client, buffer, 1);

	if (error != IRECV_E_SUCCESS) {
		printf("%s\n", irecv_strerror(error));
		return -1;
	}
	return 0;
}

int downloadFile(char *path) {
	CURL *curl;
	CURLcode ret;
	FILE *fp;
	char save[FILENAME_MAX] = "/tmp/ipsw.zip";
	curl = curl_easy_init();
	if (curl) {
		fp = fopen(save, "wb");
		curl_easy_setopt(curl, CURLOPT_URL, path);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeData);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		ret = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		fclose(fp);
	}
	return 0;
}

int main(int argc, char **argv) {
	int c, i;
	char *ipsw = NULL, *kernelcache = NULL, *bootlogo = NULL, *url = NULL, *plist = NULL, *ramdisk = NULL;
	int pwndfu = false;
	irecv_error_t err = IRECV_E_SUCCESS;
	AbstractFile *plistFile;
  	Dictionary *bundle;
    firmware Firmware; 
    

	printf("opensn0w, an open source jailbreaking program.\n"
	       "Compiled on: " __DATE__ " " __TIME__ "\n\n");

	opterr = 0;

	while ((c = getopt(argc, argv, "vdhp:b:w:k:i:r:a:")) != -1) {
		switch (c) {
		case 'v':
			verboseflag = true;
			break;
		case 'a':
			boot_args_process(optarg);
			break;
		case 'd':
			pwndfu = true;
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
				printf("Cannot open kernelcache file '%s'\n",
				       optarg);
				return -1;
			}
			kernelcache = optarg;
			break;
		case 'w':
			url = optarg;
			break;
		case 'b':
			if (!file_exists(optarg)) {
				printf("Cannot open bootlogo file '%s'\n",
				       optarg);
				return -1;
			}
			bootlogo = optarg;
			break;
		case 'r':
			if (!file_exists(optarg)) {
				printf("Cannot open ramdisk file '%s'\n",
				       optarg);
				return -1;
			}
			ramdisk = optarg;
			break;
		default:
			usage();
		}
	}

	if (!plist && pwndfu == false) {
		printf("The plist is sort of required now.\n");
		return -1;
	}

	if ((plistFile = createAbstractFileFromFile(fopen(plist, "rb"))) != NULL) {
		plist = (char *)malloc(plistFile->getLength(plistFile));
		plistFile->read(plistFile, plist, plistFile->getLength(plistFile));
		plistFile->close(plistFile);
		info = createRoot(plist);
	}
    
    bundle = (Dictionary *) getValueByKey(info, "FirmwareKeys");
    if(bundle != NULL)
        bundle = (Dictionary *) bundle->values;
    
	while (bundle != NULL) {
        i = 0;
        while(i != 17) {
            if(!strcmp(bundle->dValue.key, image_names[i])) {
                StringValue *key = NULL, *iv = NULL, *name = NULL, *vfkey = NULL;

                key = (StringValue *) getValueByKey(bundle, "Key");
                iv = (StringValue *) getValueByKey(bundle, "IV");
                name = (StringValue *) getValueByKey(bundle, "FileName");
                vfkey = (StringValue *) getValueByKey(bundle, "VFDecryptKey");
        
                if(key)
                    Firmware.item[i].key = key->value;
                
                if(iv)
                    Firmware.item[i].iv = iv->value;
                
                if(name)
                    Firmware.item[i].name = name->value;
                
                if(vfkey)
                    Firmware.item[i].vfkey = vfkey->value;
                
                if(strstr(Firmware.item[i].name, ".dfu"))
                    Firmware.item[i].flags = DFU;
                else if(strstr(Firmware.item[i].name, ".dmg"))
                    Firmware.item[i].flags = ROOT;
                else if(strstr(Firmware.item[i].name, ".img3"))
                    Firmware.item[i].flags = ALL_FLASH;
                else
                    Firmware.item[i].flags = ALL_FLASH;
                
                printf("[plist] (%s %s %s %s) [%s %d]\n", 
                       Firmware.item[i].key,
                       Firmware.item[i].iv,
                       Firmware.item[i].name,
                       Firmware.item[i].vfkey,
                       image_names[i],
                       i);
                
                break;
            }
            i++;
        }
		bundle = (Dictionary *) bundle->dValue.next;
	}

	/* to be done */
	printf("Initializing libirecovery\n");
	irecv_init();

#ifndef __APPLE__
	irecv_set_debug_level(3);
#endif

	/* Poll for DFU mode */
	while (poll_device_for_dfu()) {
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

    Dictionary* temporaryDict = (Dictionary*)getValueByKey(info, "FirmwareInfo");
    StringValue *urlKey;
    if(temporaryDict != NULL)
        urlKey = (StringValue*)getValueByKey(temporaryDict, "URL");
    if(urlKey)
        device->url = urlKey->value;
    
	printf("Device found: name: %s, processor s5l%dxsi\n", device->product,
	       device->chip_id);
	printf("iBoot information: %s\n", client->serial);

	/* What jailbreak exploit is this thing capable of? */
	if (device->chip_id == 8930 || device->chip_id == 8922
	    || device->chip_id == 8920) {
		printf
		    ("This device is compatible with the limera1n exploit. Sending.\n");
		err = limera1n();
		if (err) {
			printf("Error during limera1ning.\n");
			exit(-1);
		}
		if (pwndfu == true) {
			printf
			    ("bootrom is owned. feel free to restore custom ipsws.\n");
			exit(0);
		}
	} else if(device->chip_id == 8720) {
		printf
        ("This device is compatible with the steaks4uce exploit. Sending.\n");
		err = steaks4uce();
		if (err) {
			printf("Error during steaks4uceing.\n");
			exit(-1);
		}
		if (pwndfu == true) {
			printf
            ("bootrom is owned. feel free to restore custom ipsws.\n");
			exit(0);
		}
	} else if(device->chip_id == 8900) {
		printf
        ("This device is compatible with the pwnage2 exploit. Sending.\n");
		err = pwnage2();
		if (err) {
			printf("Error during pwnage2ing.\n");
			exit(-1);
		}
		if (pwndfu == true) {
			printf
            ("bootrom is owned. feel free to restore custom ipsws.\n");
			exit(0);
		}
    }
	else {
		printf("Support for the S5L%dX isn't done yet.\n",
		       device->chip_id);
	}

	/* We are owned now! */
	printf("Bootrom is pwned now! :D\n");

	/* upload iBSS */
	if (ramdisk)
		UsingRamdisk = TRUE;

	upload_image(Firmware.item[IBSS], 0, 1);
	client = irecv_reconnect(client, 10);

	upload_image(Firmware.item[IBEC], 0, 1);
	client = irecv_reconnect(client, 10);

	irecv_reset(client);
	client = irecv_reconnect(client, 10);
	irecv_set_interface(client, 0, 0);
	irecv_set_interface(client, 1, 1);

	/* upload logo */
	upload_image(Firmware.item[APPLELOGO], 2, 0);

	irecv_send_command(client, "setpicture 0");
	irecv_send_command(client, "bgcolor 0 0 0");
	client = irecv_reconnect(client, 10);

	/* upload devicetree */
	upload_image(Firmware.item[DEVICETREE], 1, 0);
	client = irecv_reconnect(client, 10);
	irecv_send_command(client, "devicetree");
	client = irecv_reconnect(client, 10);

	/* upload ramdisk */
	if (ramdisk) {
		upload_image(Firmware.item[RESTORERAMDISK], 4, 0);

		sleep(5);

		printf("sending ramdisk command\n");
		irecv_send_command(client, "ramdisk");

		irecv_reset_counters(client);
	}

	/* upload kernel */
	upload_image(Firmware.item[KERNELCACHE], 3, 1);
	client = irecv_reconnect(client, 10);

	/* BootX */
	printf("booting\n");
	irecv_send_command(client, "bootx");

	/* device is ready to run */

	printf("to be completed\n");

	return 0;
}
