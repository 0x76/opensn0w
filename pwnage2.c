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

//
// WtfDfu:
// http://appldnld.apple.com.edgesuite.net/content.info.apple.com/iPhone/061-6618.20090617.Xse7Y/x12220000_5_Recovery.ipsw
//

#define WTF_IPSW	"http://appldnld.apple.com.edgesuite.net/content.info.apple.com/iPhone/061-6618.20090617.Xse7Y/x12220000_5_Recovery.ipsw"

extern PartialZipProgressCallback callback;

/*!
 * \fn int dfu_fetch_image(const char *path, const char *output)
 * \brief Fetch a specified image from the Internet.
 *
 * \param path Filename in zip.
 * \param output Output filename.
 */

int dfu_fetch_image(const char *path, const char *output)
{
	DPRINT("Fetching %s...\n", path);
	STATUS("[*] Fetching %s...\n", path);
	if (download_file_from_zip(WTF_IPSW, path, output, callback) != 0) {
		ERR("Unable to fetch %s\n", path);
		return -1;
	}

	return 0;
}

int pwnage2()
{
	char path[] = "Firmware/dfu/WTF.s5l8900xall.RELEASE.dfu";
	char tmpbuf[255];
	char *out = strrchr(path, '/') + 1;
	struct stat buf;
	char *inData;
	size_t inDataSize;
	irecv_error_t error = IRECV_E_SUCCESS;
	AbstractFile *abstractFile;
	AbstractFile *outAbstractFile;

	DPRINT("Checking if %s file exists...\n", out);

	/* init buffer */
	memset(tmpbuf, 0, 255);

	if(stat(out, &buf) != 0) {
		if (dfu_fetch_image(path, out) < 0) {
			ERR("Unable to upload DFU image\n");
			return -1;
		}
	}

	snprintf(tmpbuf, 255, "%s.pwn", out);

	abstractFile = createAbstractFileFromFile(fopen(out, "rb"));
	if(!abstractFile) {
		ERR("Failed to open input WTF file as abstract.\n");
		return -1;
	}

	inDataSize = (size_t) abstractFile->getLength(abstractFile);
	inData = (char *)malloc(inDataSize);
	abstractFile->read(abstractFile, inData, inDataSize);
	abstractFile->close(abstractFile);

	outAbstractFile = createAbstractFileFromFile(fopen(tmpbuf, "wb"));
	if(!outAbstractFile) {
		ERR("Failed to open output WTF file as abstract.\n");
		return -1;
	}

	DPRINT("Writing to output...\n");

	outAbstractFile->write(outAbstractFile, inData, inDataSize);
	outAbstractFile->close(outAbstractFile);

	outAbstractFile = openAbstractFile(createAbstractFileFromFile(fopen(tmpbuf, "r+b")));
	if(!outAbstractFile) {
		ERR("Failed to open output WTF file as abstract.\n");
		return -1;
	}

	exploit8900(outAbstractFile);
	outAbstractFile->close(outAbstractFile);

	error = irecv_send_file(client, tmpbuf, 0);

	if (error != IRECV_E_SUCCESS) {
		ERR("%s\n", irecv_strerror(error));
		return -1;
	}

	return -1;
}
