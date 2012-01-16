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

#include <sys/stat.h>
#include <sys/wait.h>

#include "utils.h"
#include "syscalls.h"
#include "hfs_mount.h"

char* cache_env[] = {
	"DYLD_SHARED_CACHE_DONT_VALIDATE=1",
	"DYLD_SHARED_CACHE_DIR=/System/Library/Caches/com.apple.dyld",
	"DYLD_SHARED_REGION=private"
};

typedef enum _partition_types {
	LWVM = 0,
	LEGACY = 1
} partition_types;

typedef struct _jailbreak_context {
	int system_version;
	partition_types partition_type;
} jailbreak_context, *jailbreak_context_t;

const char* _root_partition_names[] = {
	"/dev/disk0s1s1",
	"/dev/disk0s1"
};

const char* _user_partition_names[] = {
	"/dev/disk0s1s2",
	"/dev/disk0s2s1"
};

char* _fsck_root_partition_lwvm[] = {"/sbin/fsck_hfs", "-fy", "/dev/disk0s1s1", NULL};
char* _fsck_root_partition[] = {"/sbin/fsck_hfs", "-fy", "/dev/disk0s1", NULL};
char* _fsck_user_partition_lwvm[] = {"/sbin/fsck_hfs", "-fy", "/dev/disk0s1s2", NULL};
char* _fsck_user_partition[] = {"/sbin/fsck_hfs", "-fy", "/dev/disk0s2s1", NULL};
	
int main(int argc, char* argv[], char* env[]) {
	jailbreak_context jb_ctx;
	struct stat status;
	int ret = 0;
	
	/* zero context */
	memset((void*)&jb_ctx, 0, sizeof(jailbreak_context));
	
	/* open console */
	console = open("/dev/console", O_WRONLY);
	dup2(console, 1);
	dup2(console, 2);
	
	puts("Waiting for disk to mount...\n");
	while(stat("/dev/disk0s1", &status) != 0) { /* lwvm base partition */
		puts("Please wait...\n");
		sleep(1);
	}
	
	puts("disk0s1 is alive. Houston, we hear you loud and clear.\n");

	/* at this point, check the disk */
	if (hfs_mount(_root_partition_names[LEGACY], "/mnt", MNT_ROOTFS | MNT_RDONLY) != 0) {
		if (hfs_mount(_root_partition_names[LWVM], "/mnt", MNT_ROOTFS | MNT_RDONLY) != 0) {
			puts("Unable to mount filesystem!\n");
			return -1;
			
		} else {
			jb_ctx.system_version = 5;
			jb_ctx.partition_type = LWVM;
		}
	}
	
	/* mount /dev, we're going to chroot into it. */
	puts("Filesystem mounted to /mnt.");
	
	puts("Going to mount /dev as devfs...\n");
	if(mount("devfs", "/mnt/dev", 0, NULL) != 0) {
		puts("Unable to mount /dev as devfs!\n");
		unmount("/mnt", 0);
		return -1;
	}
	
	/* check rootfs integrity */
	if(jb_ctx.partition_type == LWVM) {
		ret = fsexec(_fsck_root_partition_lwvm, env);
	} else {
		ret = fsexec(_fsck_root_partition, env);		
	}
	
	if(ret != 0) {
		puts("AW DAMN. Fscking the rootfs failed.\n");
		unmount("/mnt/dev", 0);
		unmount("/mnt", 0);
		return -1;
	}
	
	/* remount the fs */
	puts("Remounting FS...\n");
	
	if(jb_ctx.partition_type == LWVM) {
		ret = hfs_mount(_root_partition_names[LWVM], "/mnt", MNT_ROOTFS | MNT_UPDATE);
	} else {
		ret = hfs_mount(_root_partition_names[LEGACY], "/mnt", MNT_ROOTFS | MNT_UPDATE);
	}
	
	if(ret != 0) {
		puts("Failed to remount rootfs.\n");
		unmount("/mnt/dev", 0);
		unmount("/mnt", 0);
		return -1;
	}
	
	/* mount user partition */
	
	/* BUT FOR NOW JUST SPIN */
	while(1);
	
	return 0;
}
