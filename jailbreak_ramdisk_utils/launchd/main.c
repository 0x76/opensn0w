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

#include <stdio.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include "hfs_mount.h"

char* cache_env[] = {
	"DYLD_SHARED_CACHE_DONT_VALIDATE=1",
	"DYLD_SHARED_CACHE_DIR=/System/Library/Caches/com.apple.dyld",
	"DYLD_SHARED_REGION=private"
};

typedef enum _partition_types {
	LWVM = 0,
	LEGACY = 1,
	OLD = 2
} partition_types;

typedef struct _jailbreak_context {
	int system_version;
	partition_types partition_type;
} jailbreak_context, *jailbreak_context_t;

const char* _root_partition_names[] = {
	"/dev/disk0s1s1",
	"/dev/disk0s1",
	"/dev/disk0s1"
};

const char* _user_partition_names[] = {
	"/dev/disk0s1s2",
	"/dev/disk0s2s1"
	"/dev/disk0s2"
};

char* _fsck_root_partition_lwvm[] = {"/sbin/fsck_hfs", "-fy", "/dev/disk0s1s1", NULL};
char* _fsck_root_partition[] = {"/sbin/fsck_hfs", "-fy", "/dev/disk0s1", NULL};
char* _fsck_user_partition_lwvm[] = {"/sbin/fsck_hfs", "-fy", "/dev/disk0s1s2", NULL};
char* _fsck_user_partition[] = {"/sbin/fsck_hfs", "-fy", "/dev/disk0s2s1", NULL};
	
int main(int argc, char* argv[], char* env[]) {
	jailbreak_context jb_ctx;
	struct stat status;
	int ret = 0, i;

	for(i = 0; i < 100; i++) {
		printf("\n");
	}
	
	printf("Waiting for disk to mount...\n");

	while(stat("/dev/disk0s1", &status) != 0) { /* lwvm base partition */
		printf("Please wait...\n");
		sleep(1);
	}
	sleep(2);
	
	printf("disk0s1 is alive. Houston, we hear you loud and clear.\n");

	/* at this point, check the disk */
    printf("Waiting for partitions...\n");
    for(i = 0; i < 10; i++) {
        if(!stat("/dev/disk0s2s1", &status)) {
            system("/sbin/fsck_hfs -fy /dev/disk0s2s1");
			system("/sbin/mount_hfs /dev/disk0s2s1 /mnt1");
            break;
		}
        if(!stat("/dev/disk0s1s2", &status)) {
            system("/sbin/fsck_hfs -fy /dev/disk0s1s2");
			system("/sbin/mount_hfs /dev/disk0s1s2 /mnt2");
            break;
		}
        if(!stat("/dev/disk0s1", &status)) {
            system("/sbin/fsck_hfs -fy /dev/disk0s1");
			system("/sbin/mount_hfs /dev/disk0s1 /mnt1");
            break;
		}
        if(!stat("/dev/disk0s1s1", &status)) {
            system("/sbin/fsck_hfs -fy /dev/disk0s1s1");
			system("/sbin/mount_hfs /dev/disk0s1s1 /mnt1");
            break;
		}
        sleep(5);
	}
	
	/* mount /dev, we're going to chroot into it. */
	printf("Filesystem mounted to /mnt1.\n");

	printf("Making RAM disk rw...\n");
    system("mount /");
	
	/* mount user partition */
	
	printf(" #######  ##    ##\n");
    printf("##     ## ##   ## \n");
    printf("##     ## ##  ##  \n");
    printf("##     ## #####   \n");
    printf("##     ## ##  ##  \n");
    printf("##     ## ##   ## \n"); 
    printf(" #######  ##    ##\n");

	/* BUT FOR NOW JUST SPIN */
	while(1);
	
	return 0;
}
