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

int patch_file(char* filename) {
	AbstractFile *template = NULL, *inFile, *certificate = NULL, *outFile, *newFile;
	unsigned int key[16];
	unsigned int iv[16];
	char* inData;
	size_t inDataSize;
	char *buffer;

	template = createAbstractFileFromFile(fopen(filename, "rb"));

	/* open base template */
	if(!template) {
		printf("Cannot open template.\n");
		return -1;
	}

	/* open file */
	inFile = openAbstractFile3(createAbstractFileFromFile(fopen(filename, "rb")), key, iv, 0);
	if(!inFile) {
		printf("Cannot open %s.\n", filename);
		return -1;
	}

	/* read it */
	inDataSize = (size_t) inFile->getLength(inFile);
	inData = (char*) malloc(inDataSize);
	inFile->read(inFile, inData, inDataSize);
	inFile->close(inFile);

	/* zero buffer */
	buffer = malloc(strlen(filename) + 4);
	if(!buffer) {
		printf("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 4);

	snprintf(buffer, strlen(filename) + 4, "%s.pwn", filename);

	/* open output */
	outFile = createAbstractFileFromFile(fopen(buffer, "wb"));
	if(!outFile) {
		printf("Cannot open outfile\n");
		return -1;
	}

	newFile = duplicateAbstractFile2(template, outFile, key, iv, certificate);
	if(!newFile) {
		printf("Cannot open newfile\n");
		return -1;
	}

	/* write patched contents */
	newFile->write(newFile, inData, inDataSize);
	newFile->close(newFile);

	free(buffer);

	return 0;
}
