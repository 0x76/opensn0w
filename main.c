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

#define __SN0W_VERSION__ "0.0.0.1-pre2"

/* globals */

int opensn0w_debug_level = DBGFLTR_RELEASE;
bool verboseflag = false, dump_bootrom = false, raw_load = false, raw_load_exit = false;
int Img3DecryptLast = TRUE;
int UsingRamdisk = FALSE;
char *version = "UNKNOWN";
irecv_device_t device = NULL;
irecv_client_t client = NULL;
Dictionary *firmwarePatches, *patchDict, *info;

/* usage */

#define usage(x) \
	printf("Usage: %s [OPTIONS]\n" \
			"Jailbreak an iOS device, this includes the iPhone, iPad, iPod touch and\n" \
			"Apple TV 2G.\n\n" \
			"Currently supported devices are: %s\n" \
			"\n" \
			"Options:\n" \
			"   -v                 Verbose mode. Useful for debugging.\n" \
			"   -i ipsw            Get necessary files from a remote IPSW.\n" \
			"   -p plist           Use firmware plist\n" \
			"   -h                 Help.\n" \
			"   -k kernelcache     Boot using specified kernel.\n" \
			"   -b bootlogo.img3   Use specified bootlogo img3 file during startup.\n" \
			"   -r ramdisk.dmg     Boot specified ramdisk.\n" \
			"   -R                 Just boot into pwned recovery mode.\n" \
			"   -Z                 Use raw image load payload only.\n" \
			"   -z                 Use raw image load payload and boot device. (Use on devices with corrupted Chip ID)\n" \
			"   -B                 Dump SecureROM to bootrom.bin (works on limera1n devices only.)\n" \
			"   -s                 Start iRecovery recovery mode shell.\n" \
			"   -d                 Just pwn dfu mode.\n" \
			"   -X                 Download all files from plist.\n" \
			"   -S [file]          Send file to device.\n" \
			"   -C [command]       Send command to device.\n" \
			"   -A                 Set auto-boot. (Kick out of recovery.)\n" \
			"   -a [boot-args]     Set device boot-args for boot.\n" \
			"\n" \
			"Exit status:\n" \
			"  0  if OK,\n" \
			" -1  if severe issues.\n" \
			"\n" \
			"Report %s bugs to rms@velocitylimitless.org\n" \
			"%s homepage: <http://www.opensn0w.com>\n" \
			"For complete documentation, see the UNIX manual.\n", \
			"opensn0w", \
			"s5l8930x, s5l8920x, s5l8922x, s5l8720x, s5l8900x", "opensn0w", "opensn0w"); \
			exit(-1);

/* image names */

static const char *image_names[] = {
	"iBSS",
	"DeviceTree",
	"BatteryCharging1",
	"GlyphCharging",
	"BatteryCharging0",
	"iBoot",
	"BatteryLow0",
	"LLB",
	"iBEC",
	"KernelCache",
	"FileSystem",
	"AppleLogo",
	"UpdateRamdisk",
	"RestoreRamdisk",
	"GlyphPlugin",
	"Recovery",
	"BatteryLow1",
	"BatteryFull",
	"NeedService"
};

/*!
 * \fn void boot_args_process(char *args)
 * \brief Process iPhone boot arguments for iBoot injection.
 *
 * \param args Arguments for iBoot
 */
 
void boot_args_process(char *args)
{
	char buffer[39];

	if (strlen(args) > 39) {
		printf
		    ("Boot-args is too long. Please shorten to 39 characters or less.\n");
		exit(-1);
	}

	/* pad */
	snprintf(buffer, 39, "%-39s", args);

	DPRINT("Booting with boot-args \"%s\"\n", buffer);

	memcpy(iBEC_bootargs.patched, buffer, 39);
}

/*!
 * \fn bool file_exists(const char *fileName)
 * \brief Check if file exists.
 *
 * \param fileName filename.
 */
 
bool file_exists(const char *fileName)
{
	struct stat buf;
	return !stat(fileName, &buf);
}

/*!
 * \fn char *mode_to_string(int mode)
 * \brief Required \a mode to string.
 *
 * \param mode Required mode (DFU/iBoot)
 */

char *mode_to_string(int mode)
{
	switch (mode) {
		case DFU:
			return "DFU";
		case RECOVERYMODE:
			return "Recovery/iBoot";
		default:
			return "UNKNOWN";
	}
}

/*!
 * \fn int poll_device(int mode)
 * \brief Poll device for specified \a mode.
 *
 * \param mode Required mode (DFU/iBoot).
 */

int poll_device(int mode)
{
	irecv_error_t err;
	static int try;

	err = irecv_open(&client);
	if (err != IRECV_E_SUCCESS) {
		STATUS("Connect the device in %s mode. [%u]\r", mode_to_string(mode), try);
		fflush(stdout);
		try++;
		return 1;
	}

	switch (client->mode) {
		case kDfuMode2:
		case kDfuMode:
			if (mode == DFU) {
				return 0;
			}
		case kRecoveryMode2:
			if (mode == RECOVERYMODE) {
				return 0;
			}
		default:
			STATUS("Connect the device in %s mode. [%u]\r",
			       mode_to_string(mode), try);
			irecv_close(client);
			try++;
			return 1;
	}

	return 0;
}

/*!
 * \fn int send_command(char *name)
 * \brief Send \a name as an iBoot command to device.
 *
 * \param name iBoot command.
 */

int send_command(char *name)
{
	irecv_error_t err;

	DPRINT("Initializing libirecovery...\n");
	irecv_init();

#ifndef __APPLE__
	irecv_set_debug_level(3);
#endif

	while (1) {
		err = irecv_open(&client);
		if (err != IRECV_E_SUCCESS) {
			STATUS("Connect the device. err %d\n", err);
			sleep(1);
		} else if (err == IRECV_E_SUCCESS) {
			break;
		}
	};

	if (!client) {
		FATAL("Null client!\n");
	}

	STATUS("[*] Sending command \"%s\" to iBoot...\n", name);
	irecv_send_command(client, name);

	exit(err);
	return err;
}

/*!
 * \fn int send_filename(char *name)
 * \brief Send \a name as file to device.
 *
 * \param name iBoot file.
 */


int send_file(char *name)
{
	irecv_error_t err;

	DPRINT("Initializing libirecovery...\n");
	irecv_init();

#ifndef __APPLE__
	irecv_set_debug_level(3);
#endif

	while (1) {
		err = irecv_open(&client);
		if (err != IRECV_E_SUCCESS) {
			STATUS("Connect the device. err %d\n", err);
			sleep(1);
		} else if (err == IRECV_E_SUCCESS) {
			break;
		}
	};

	if (!client) {
		FATAL("Null client!\n");
	}

	if (client->mode != kDfuMode) {
		err = irecv_send_file(client, name, 0);
	} else {
		err = irecv_send_file(client, name, 1);
	}

	if (err != IRECV_E_SUCCESS) {
		FATAL("%s\n", irecv_strerror(err));
	}

	exit(err);
	return err;
}

/*!
 * \fn int fetch_image(const char *path, const char *output)
 * \brief Fetch a specified image from the Internet.
 *
 * \param path Filename in zip.
 * \param output Output filename.
 */

int fetch_image(const char *path, const char *output)
{
	DPRINT("Fetching %s...\n", path);
	STATUS("[*] Fetching %s...\n", path);
	if (download_file_from_zip(device->url, path, output, NULL) != 0) {
		ERR("Unable to fetch %s\n", path);
		return -1;
	}

	return 0;
}

/*!
 * \fn size_t writeData(void *ptr, size_t size, size_t mem, FILE * stream)
 * \brief Wrapper for fwrite.
 *
 * \param ptr Buffer for data.
 * \param size Size of written data.
 * \param mem Number of members of written data.
 * \param stream File descriptor structure.
 */

size_t writeData(void *ptr, size_t size, size_t mem, FILE * stream)
{
	size_t written;
	written = fwrite(ptr, size, mem, stream);
	return written;
}


/*!
 * \fn int download_all_the_files()
 * \brief Download ALL the files
 */
int download_all_the_files(firmware Firmware) {
	char path[255];
	struct stat buf;
	char *buffer;
	char *filename;
	firmware_item item;
	int i;

	for(i = 0; i < NEEDSERVICE; i++) {
begin:
		item = Firmware.item[i];
		if(item.name != NULL) {

			filename = strrchr(item.name, '/');

			if (filename == NULL)
				filename = item.name;
			else
				filename++;

			if(strstr(filename, "dmg")) {
				i++;
				goto begin;
			}
	
			buffer = malloc(strlen(filename) + 10 + strlen(version));
			if (!buffer) {
				ERR("Cannot allocate memory\n");
				return -1;
			}
			memset(buffer, 0, strlen(filename) + 10 + strlen(version));

			snprintf(buffer, strlen(filename) + 10 + strlen(version), "%s_%s", filename, version);

			DPRINT("Checking if %s already exists\n", buffer);

			memset(path, 0, 255);

			if (stat(buffer, &buf) != 0) {
				if (fetch_image(item.name, buffer) < 0) {
					ERR("Unable to download image\n");
					goto begin;
				}
			}

			free(buffer);
		}
	}
	return 0;
}

/*!
 * \fn int upload_image(firmware_item item, int mode, int patch, int userprovided)
 * \brief Upload image to device based on \a item and \a patch it if necessary.
 *
 * \param item Firmware item to be uploaded.
 * \param mode Notify DFU mode if upload should be finished or not.
 * \param patch Patch file.
 * \param userprovided User provided.
 */

int upload_image(firmware_item item, int mode, int patch, int userprovided)
{
	char path[255];
	struct stat buf;
	irecv_error_t error = IRECV_E_SUCCESS;
	char *buffer;
	char *filename = strrchr(item.name, '/');
	
	if (filename == NULL)
		filename = item.name;
	else
		filename++;

	buffer = malloc(strlen(filename) + 10 + strlen(version));
	if (!buffer) {
		ERR("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 10 + strlen(version));

	snprintf(buffer, strlen(filename) + 10 + strlen(version), "%s_%s", filename, version);

	if(userprovided)
		snprintf(buffer, strlen(filename) + 10 + strlen(version), "%s", filename);

	DPRINT("Checking if %s already exists\n", buffer);

	memset(path, 0, 255);

	if (stat(buffer, &buf) != 0) {
		if (fetch_image(item.name, buffer) < 0) {
			ERR("Unable to upload DFU image\n");
			return -1;
		}
	}

	if (patch && !userprovided)
		patch_file(buffer);

	if (patch && !userprovided) {
		snprintf(buffer, strlen(filename) + 10 + strlen(version), "%s_%s.pwn", filename, version);
	}

	if(raw_load == true && strcasestr(item.name, "iBSS")) {
		snprintf(buffer, strlen(filename) + 10 + strlen(version), "%s_%s.dec", filename, version);
	}

	DPRINT("Uploading %s to device\n", buffer);

	if (client->mode != kDfuMode)
		error = irecv_send_file(client, buffer, 0);
	else
		error = irecv_send_file(client, buffer, 1);

	if (error != IRECV_E_SUCCESS) {
		ERR("%s\n", irecv_strerror(error));
		return -1;
	}
	free(buffer);
	return 0;
}

/*!
 * \fn void print_configuration(void)
 * \brief Print system configuration.
 */

void print_configuration(void)
{
	printf("Running on %s.\n\n", endian_to_string(endian()));
}

/*!
 * \fn int main(int argc, char **argv)
 * \brief Main opensn0w routine!
 *
 * \param argc Argument count.
 * \param argv Argument variables.      
 */

int main(int argc, char **argv)
{
	int c, i;
	char *kernelcache = NULL, *bootlogo = NULL, *url =
	    NULL, *plist = NULL, *ramdisk = NULL;
	char *processedname;
	int pwndfu = false, pwnrecovery = false, autoboot = false, download = false;
	int userprovided = 0;
	irecv_error_t err = IRECV_E_SUCCESS;
	AbstractFile *plistFile;
	Dictionary *bundle;
	firmware Firmware;

	/* configure xpwn endian */
	switch(endian()) {
		case ENDIAN_BIG:
			endianness = IS_BIG_ENDIAN;
			break;
		case ENDIAN_LITTLE:
			endianness = IS_LITTLE_ENDIAN;
			break;
		default:
			break;
	}

	/* set up signals */
#ifndef _WIN32
	struct sigaction sigact;

	sigact.sa_sigaction = critical_error_handler;
	sigact.sa_flags = SA_RESTART | SA_SIGINFO;
	if (sigaction(SIGSEGV, &sigact, (struct sigaction *)NULL) != 0) {
		FATAL("Error setting signal handler for %d (%s)\n", SIGSEGV, strsignal(SIGSEGV));
	}
#endif

	printf("opensn0w, an open source jailbreaking program.\n"
	       "version: " __SN0W_VERSION__ "\n\n"
	       "Compiled on: " __DATE__ " " __TIME__ "\n\n");

	print_configuration();

	opterr = 0;

	while ((c = getopt(argc, argv, "vZdAhBzsXp:Rb:i:k:S:C:r:a:")) != -1) {
		switch (c) {
		case 'Z':
			raw_load_exit = true;
			break;
		case 'B':
			dump_bootrom = true;
			break;
		case 'A':
			autoboot = true;
			break;
		case 'v':
			opensn0w_debug_level = DBGFLTR_MISC;
			break;
		case 'R':
			pwnrecovery = true;
			break;
		case 'z':
			raw_load = true;
			break;
		case 'a':
			boot_args_process(optarg);
			break;
		case 'X':
			download = true;
			break;
		case 'd':
			pwndfu = true;
			break;
		case 'h':
			usage();
			break;
		case 'p':
			if (!file_exists(optarg)) {
				printf("Cannot open plist file '%s'\n", optarg);
				return -1;
			}
			plist = optarg;
			break;
		case 'k':
			if (!file_exists(optarg)) {
				printf("Cannot open kernelcache file '%s'\n",
				       optarg);
				return -1;
			}
			kernelcache = optarg;
			break;
		case 'i':
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
		case 'S':
			if (!file_exists(optarg)) {
				printf("Cannot open file '%s'\n", optarg);
				return -1;
			}
			send_file(optarg);
			break;
		case 'C':
			send_command(optarg);
			break;
		case 's':
			irecovery_shell_initialize();
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
			break;
		}
	}
	
	/* Do stuff */
	if (autoboot) {
		DPRINT("Initializing libirecovery\n");
		irecv_init();

#ifndef _NDEBUG_
		irecv_set_debug_level(3);
#endif

		/* Poll for DFU mode */
		while (poll_device(RECOVERYMODE)) {
			sleep(1);
		}

		/* Check the device */
		err = irecv_get_device(client, &device);
		if (device == NULL || device->index == DEVICE_UNKNOWN) {
#ifdef _WIN32
			STATUS("[!] Make sure your device is in the appropriate mode, and that iTunes is installed and closed!\n");
#endif
			FATAL("Bad device. errno %d\n", err);
		}
		STATUS("[*] Device found.\n");

		DPRINT("Device found: name: %s, processor s5l%dxsi\n",
		       device->product, device->chip_id);
		DPRINT("iBoot information: %s\n", client->serial);

		STATUS("[*] Fixing recovery loop...\n");
		irecv_send_command(client, "setenv auto-boot true");
		irecv_send_command(client, "saveenv");
#ifndef _WIN32
		client = irecv_reconnect(client, 2);
#else
		client = irecv_reconnect(client, 5);
#endif
		irecv_send_command(client, "reboot");

		STATUS("[*] Operation completed.\n");
		exit(0);
	}

	if (dump_bootrom || raw_load_exit) {
		/* i know, hacky */
		goto actually_do_stuff;
	}

	if ((plistFile =
	     createAbstractFileFromFile(fopen(plist, "rb"))) != NULL) {
		plist = (char *)malloc(plistFile->getLength(plistFile));
		plistFile->read(plistFile, plist,
				plistFile->getLength(plistFile));
		plistFile->close(plistFile);
		info = createRoot(plist);
	} else if ((pwndfu == false) &&
		   (plistFile =
		    createAbstractFileFromFile(fopen(plist, "rb"))) == NULL) {
		usage();
		FATAL("plist must be specified in this mode!\n\n");
	}
	
	/* Initialize Firmware structure */
	memset(&Firmware, 0, sizeof(firmware));
	Firmware.items = sizeof(image_names) / sizeof(char *);
	Firmware.item = malloc(Firmware.items * sizeof(firmware_item));

	if (Firmware.item == NULL) {
		FATAL("Unable to allocate memory for decryption keys!\n");
	}

	memset(Firmware.item, 0, Firmware.items * sizeof(firmware_item));

	bundle = (Dictionary *) getValueByKey(info, "FirmwareKeys");
	if (bundle != NULL) {
		for (i = 0; i < Firmware.items; i++) {
			Dictionary *entry =
			    (Dictionary *) getValueByKey(bundle,
							 image_names[i]);
			if (entry != NULL) {
				StringValue *key = NULL, *iv = NULL, *name =
				    NULL, *vfkey = NULL;

				key =
				    (StringValue *) getValueByKey(entry, "Key");
				iv = (StringValue *) getValueByKey(entry, "IV");
				name =
				    (StringValue *) getValueByKey(entry,
								  "FileName");
				vfkey =
				    (StringValue *) getValueByKey(entry,
								  "VFDecryptKey");

				if (key)
					Firmware.item[i].key = key->value;

				if (iv)
					Firmware.item[i].iv = iv->value;

				if (name)
					Firmware.item[i].name = name->value;

				if (vfkey)
					Firmware.item[i].vfkey = vfkey->value;

				DPRINT("[plist] (%s %s %s %s) [%s %d]\n",
				       Firmware.item[i].key,
				       Firmware.item[i].iv,
				       Firmware.item[i].name,
				       Firmware.item[i].vfkey,
				       image_names[i], i);

			}
		}
	}

 actually_do_stuff:
	/* to be done */
	DPRINT("Initializing libirecovery\n");
	irecv_init();

#ifndef __APPLE__
	irecv_set_debug_level(3);
#endif

	/* Poll for DFU mode */
	while (poll_device(DFU)) {
		sleep(1);
	}

	/* Check the device */
	if (strcmp(client->serial, "89000000000001")) {
		err = irecv_get_device(client, &device);
		if (device == NULL || device->index == DEVICE_UNKNOWN) {
#ifdef _WIN32
			STATUS("[!] Make sure your device is in DFU mode, and that iTunes is installed and closed!\n");
#endif
			FATAL("Bad device. errno %d\n", err);
		}
		STATUS("[*] Device found.\n");
		DPRINT("Device found: name: %s, processor s5l%dxsi\n",
		       device->product, device->chip_id);
		DPRINT("iBoot information: %s\n", client->serial);

		if (device->chip_id == 8900) {
			FATAL
			    ("It looks like your device is in WTF mode already! Get into DFU mode.\n");
		}

	} else {
		DPRINT("Using 8900 device descriptor.\n");
		STATUS("[*] S5L8900XRB device found.\n");

		device = malloc(sizeof(irecv_device));
		memset(device, 0, sizeof(irecv_device));
		device->chip_id = 8900;
		device->product = "s5l8900xall";
	}

	Dictionary *temporaryDict =
	    (Dictionary *) getValueByKey(info, "FirmwareInfo");
	StringValue *urlKey = NULL;
	if (temporaryDict != NULL)
		urlKey = (StringValue *) getValueByKey(temporaryDict, "URL");
	if (urlKey != NULL) {
		char *p = NULL, dup[256];
		int len;

		memset(dup, 0, 256);

		device->url = urlKey->value;
		if (temporaryDict != NULL)
			urlKey = (StringValue *) getValueByKey(temporaryDict, "URL");
		if (urlKey != NULL)
			p = urlKey->value;
		if(!p)
			goto out;

		p = strstr(p, device->product);
		if(!p)
			goto out;

		len = strlen(p);
		if(len <= 0)
			goto out;

		strncpy(dup, p, len - sizeof("Restore.ipsw"));
		version = strdup(dup);
		
	}
out:

	if (url) {
		processedname = malloc(strlen(url) + sizeof("file://"));
		if (!processedname) {
			printf("Could not allocate memory\n");
			exit(-1);
		}
		memset(processedname, 0, strlen(url) + sizeof("file://"));
		snprintf(processedname, strlen(url) + sizeof("file://"),
			 "file://%s", url);
		device->url = processedname;
	}

	if(download) {
		download_all_the_files(Firmware);
		exit(0);
	}

	STATUS("[*] Exploiting bootrom...\n");
	/* What jailbreak exploit is this thing capable of? */
	if (device->chip_id == 8930 || device->chip_id == 8922
	    || device->chip_id == 8920) {
		DPRINT
		    ("This device is compatible with the limera1n exploit. Sending.\n");
		err = limera1n();
		if (err) {
			FATAL("Error during limera1ning.\n");
		}
		if (pwndfu == true) {
			printf
			    ("bootrom is owned. feel free to restore custom ipsws.\n");
			exit(0);
		}
	} else if (device->chip_id == 8720) {
		DPRINT
		    ("This device is compatible with the steaks4uce exploit. Sending.\n");
		err = steaks4uce();
		if (err) {
			FATAL("Error during steaks4uceing.\n");
		}
		if (pwndfu == true) {
			printf
			    ("bootrom is owned. feel free to restore custom ipsws.\n");
			exit(0);
		}
	} else if (device->chip_id == 8900) {
		DPRINT
		    ("This device is compatible with the pwnage2 exploit. Sending.\n");
		err = pwnage2();
		if (err) {
			ERR("Error during pwnage2ing.\n");
		}
		if (pwndfu == true) {
			printf
			    ("bootrom is owned. feel free to restore custom ipsws.\n");
			exit(0);
		}
	} else {
		FATAL("Support for the S5L%dX isn't done yet.\n",
		      device->chip_id);
	}

	/* We are owned now! */
	DPRINT("Bootrom is pwned now! :D\n");

	/* upload iBSS */
	if (ramdisk)
		UsingRamdisk = TRUE;

	STATUS("[*] Uploading stage zero (iBSS)...\n");
	upload_image(Firmware.item[IBSS], 0, 1, 0);
#ifndef _WIN32
	client = irecv_reconnect(client, 30);
#else
	client = irecv_reconnect(client, 5);
#endif

	STATUS("[*] Uploading stage one (iBEC)...\n");
	upload_image(Firmware.item[IBEC], 0, 1, 0);
#ifndef _WIN32
	client = irecv_reconnect(client, 45);
#else
	client = irecv_reconnect(client, 10);
#endif

	STATUS("[*] Waiting for reset...\n");
#ifndef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif
	irecv_reset(client);
	irecv_set_interface(client, 0, 0);
	irecv_set_interface(client, 1, 1);

	if (pwnrecovery) {
		FATAL
		    ("Device has a pwned iBEC uploaded. Do whatever you want \n");
	}

	/* upload logo */
	STATUS("[*] Uploading boot logo...\n");
	if (bootlogo) {
		userprovided = 1;
		Firmware.item[APPLELOGO].name = bootlogo;
	}

	upload_image(Firmware.item[APPLELOGO], 2, 0, userprovided);
	userprovided = 0;
	irecv_send_command(client, "setpicture 0");
	irecv_send_command(client, "bgcolor 0 0 0");
#ifndef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif

	/* upload ramdisk */
	if (ramdisk) {
		STATUS("[*] Uploading ramdisk...\n");
		Firmware.item[RESTORERAMDISK].name = ramdisk;
		upload_image(Firmware.item[RESTORERAMDISK], 4, 0, 1);
		irecv_reset(client);
		sleep(5);
		irecv_close(client);
		irecv_exit();

		DPRINT("Reinitializing libirecovery.\n");

		STATUS("[!] HACK-O-RAMA WARNING: TO GET THIS WORKING, YOU MUST REMOVE DEVICE WHEN IT TIMES OUT ON INTERFACE RESET.\n");
		sleep(5);
		irecv_init();

		while (poll_device(RECOVERYMODE)) {
			sleep(1);
		}

		DPRINT("sending ramdisk command\n");
		irecv_send_command(client, "ramdisk");

		irecv_reset_counters(client);
	}

	/* upload devicetree */
	STATUS("[*] Uploading device tree...\n");
	upload_image(Firmware.item[DEVICETREE], 1, 1, 0);
#ifndef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif

	irecv_send_command(client, "devicetree");
#ifndef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif

	/* upload kernel */
	STATUS("[*] Uploading kernel...\n");
	if (kernelcache) {
		Firmware.item[KERNELCACHE].name = kernelcache;
		userprovided = 1;
	}
	upload_image(Firmware.item[KERNELCACHE], 3, 1, 0);
	userprovided = 0;
#ifndef _WIN32
	client = irecv_reconnect(client, 5);
#else
	client = irecv_reconnect(client, 2);
#endif

	/* BootX */
	STATUS("[*] Booting.\n");
	DPRINT("Booting kernel.\n");
	irecv_send_command(client, "bootx");

	return 0;
}
