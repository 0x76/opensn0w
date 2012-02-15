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
#include <openssl/sha.h>
#include "cert.h"
#include "crc.h"

const uint8_t key837[] = {
    0x18, 0x84, 0x58, 0xA6, 0xD1, 0x50, 0x34, 0xDF,
    0xE3, 0x86, 0xF2, 0x3B, 0x61, 0xD4, 0x37, 0x74
};

const uint8_t image_magic[] = {
    '8', '9', '0', '0'
};

const uint8_t image_version[] = {
    '1', '.', '0', '\0'
};

int fetch_image_for_wtf(const char *path, const char *output, const char *url)
{
	DPRINT("Fetching %s...\n", path);
	STATUS("[*] Fetching %s...\n", path);
	if (download_file_from_zip(url, path, output, NULL) != 0) {
		ERR("Unable to fetch %s\n", path);
		return -1;
	}
    
	return 0;
}

int pwnage2() {
   	struct stat statb;
    char filename[] = "Firmware/dfu/WTF.s5l8900xall.RELEASE.dfu",
    out[] = "WTF.s5l8900xall.RELEASE.dfu",
    url[] = "http://appldnld.apple.com.edgesuite.net/content.info.apple.com/iPhone/061-7481.20100202.4orot/iPhone1,1_3.1.3_7E18_Restore.ipsw";
    uint8_t *buf;
    const char header[] = {0xff, 0xff, 0xff, 0xff, 0xac, 0x05, 0x00, 0x01, 0x55, 0x46, 0x44, 0x10};
    uint8_t tmpbuf[255];
    FILE *fp;
    int wtf_length, buffer_size, i, c;
    unsigned int crc = 0xFFFFFFFF;
    
	if (stat(filename, &statb) != 0) {
		if (fetch_image_for_wtf(filename, out, url) < 0) {
			ERR("Unable to upload WTF image\n");
			return -1;
		}
	}
    
    fp = fopen(out, "rb");
    fseek(fp, 0, SEEK_END);
    wtf_length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    buffer_size = wtf_length + 0x10;
    
    buf = malloc(buffer_size);
    if(!buf) {
        ERR("Malloc failed.\n");
        return -1;
    }
    
    /* clear out first 0x800 bytes */
    
    DPRINT("Preparing WTF buffer\n");
    
    memset(buf, 0, 0x800);
    fread(buf, 1, wtf_length, fp);

    /* update dfu crc */
    memcpy((void*)(buf + wtf_length), header, 0xC);
    crc = update_crc(crc, buf, 0x800 + wtf_length + ___cert_len + 0xC);
    for(i = 0; i < 4; i++) {
        buf[wtf_length + 0xC + i] = crc & 0xFF;
        crc = crc >> 8;
    }
    hex_dump(&buf[wtf_length - 0x200], 0x210);
    DPRINT("WTF image prepared!\n");
    
    /* send it */
    i = 0;
    c = 0;
    
	while(i < ((wtf_length + 0x10) + 0x800))
	{
		int sl = ((0x800 + wtf_length + ___cert_len + 0x10) - i);
        
		if(sl < 0)
            sl = 0;
		if(sl > 0x800)
            sl = 0x800;
        
        irecv_control_transfer(client, 0x21, 1, c, 0, &buf[i], sl, 1000);
        
		int b = 0;
        
		while(irecv_control_transfer(client, 0xA1, 3, 0, 0, tmpbuf, 6, 1000) == 6 && b < 5)
		{
			b++;
			if(tmpbuf[4] == 5)
                break;
		}
        
		i += 0x800;
		c++;
    }
    
    DPRINT("Done.");
    return 0;
}