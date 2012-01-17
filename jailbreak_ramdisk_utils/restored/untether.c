/*
 * opensn0w
 *
 * Ramdisk Utilities
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

#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <stdint.H>
#include <sys/stat.h>
#include <net/if.h>
#include <assert.h>
#include <CoreFoundation/CoreFoundation.h>
#include <AvailabilityMacros.h>
#define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_5
#include <IOKit/IOCFPlugIn.h>

#include "xpwn/plist.h"

#include "IOUSBDeviceControllerLib.h"

#include "hfs_mount.h"

int install(const char* src, const char* dst, int uid, int gid, int mode);

#define printf	printf("%s: ", __FUNCTION__), printf

int untether(char* platform, char* build) {
	char *buffer;

	buffer = malloc(256);
	if(!buffer) {
		printf("Cannot allocate memory.\n");
		return -1;
	}
	memset(buffer, 0, 256);
	
	/* check for sanity */
	if(!platform) {
		printf("Bad platform.\n");
		return -1;
	}
	
	if(!build) {
		printf("Bad build.\n");
		return -1;
	}
	
	printf("GET A LOAD OF THIS.\n");
	
	mkdir("/mnt2/jailbreak", 755);
	symlink("/mnt2/jailbreak", "/untether_binaries");
	system("tar xvf /untether.tar.gz -C /untether_binaries");
	
	if(!strcmp(build, "9A405")) {
		unsigned char original[] = {'p', 'r', 'o', 'f', 'i', 'l', 'e', 's'};
		unsigned char patch[] = {'p', 'r', 'o', 'f', 'i', 'l', '3', 's'};
		
		FILE *fp = fopen("/mnt1/usr/sbin/racoon", "rb+");
		char* buffer;
		if(!fp) {
			printf("Bad file\n");
			return -1;
		}
		
		int size = fseek(fp, 0, SEEK_END);
		
		fseek(fp, 0, SEEK_SET);
		buffer = malloc(size);
		if(!buffer) {
			printf("Out of memory\n");
			return -1;
		}
		
		fread(buffer, 1, size, fp);
		if(!memcmp((char*)(buffer + 457428), original, 8)) {
			printf("Found byte pattern to patch...");
			memcpy((char*)(buffer + 457428), patch, 8);
			printf("patched.\n");
		}
		printf("Writing to /usr/sbin/racoon...\n");
		fwrite(buffer, 1, size, fp);
		fclose(fp);
		
		mkdir("/mnt1/usr/share/corona", 755);
		install("/untether_binaries/common/9A405/vnimage.clean", "/mnt1/usr/share/corona/vnimage.clean", 0, 0, 644);
		install("/untether_binaries/common/9A405/jb.plist", "/mnt1/usr/share/corona/jb.plist", 0, 0, 644);
		install("/untether_binaries/common/9A405/launchd.conf", "/mnt1/etc/launchd.conf", 0, 0, 644);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/payload", platform);
		install(buffer, "/mnt1/usr/share/corona/payload", 0, 0, 644);

		sprintf(buffer, "/untether_binaries/%s/9A405/payload-vars", platform);
		install(buffer, "/mnt1/usr/share/corona/payload-vars", 0, 0, 644);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/racoon-exploit.conf", platform);
		install(buffer, "/mnt1/usr/share/corona/racoon-exploit.conf", 0, 0, 644);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/sysent_1c50", platform);
		install(buffer, "/mnt1/usr/share/corona/sysent_1c50", 0, 0, 644);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/vnimage.overflow", platform);
		install(buffer, "/mnt1/usr/share/corona/vnimage.overflow", 0, 0, 644);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/vnimage.payload", platform);
		install(buffer, "/mnt1/usr/share/corona/vnimage.payload", 0, 0, 644);
		
		system("chmod 644 /mnt1/usr/share/corona/*");
	}
	
	return 0;
	
}