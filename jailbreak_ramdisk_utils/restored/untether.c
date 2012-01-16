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
	
	if(!strcmp(build, "9A405")) {
		mkdir("/mnt1/usr/share/corona", 755);
		install("/untether_binaries/common/9A405/vnimage.clean", "/mnt1/usr/share/corona/vnimage.clean", 0, 0, 755);
		install("/untether_binaries/common/9A405/jb.plist", "/mnt1/usr/share/corona/jb.plist", 0, 0, 755);
		install("/untether_binaries/common/9A405/launchd.conf", "/mnt1/etc/launchd.conf", 0, 0, 755);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/payload", platform);
		install(buffer, "/mnt1/usr/share/corona/payload", 0, 0, 755);

		sprintf(buffer, "/untether_binaries/%s/9A405/payload-vars", platform);
		install(buffer, "/mnt1/usr/share/corona/payload-vars", 0, 0, 755);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/racoon-exploit.conf", platform);
		install(buffer, "/mnt1/usr/share/corona/racoon-exploit.conf", 0, 0, 755);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/sysent_1c50", platform);
		install(buffer, "/mnt1/usr/share/corona/sysent_1c50", 0, 0, 755);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/vnimage.overflow", platform);
		install(buffer, "/mnt1/usr/share/corona/vnimage.overflow", 0, 0, 755);
		
		sprintf(buffer, "/untether_binaries/%s/9A405/vnimage.payload", platform);
		install(buffer, "/mnt1/usr/share/corona/vnimage.payload", 0, 0, 755);
	}
	
	return 0;
	
}