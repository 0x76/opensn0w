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

extern Dictionary *firmwarePatches, *patchDict, *info;

Dictionary* get_key_dictionary_from_bundle(char* member) {
	firmwarePatches = (Dictionary*)getValueByKey(info, "FirmwarePatches");
	patchDict = (Dictionary*) firmwarePatches->values;
	StringValue* fileValue = (StringValue*) getValueByKey(patchDict, "File");

	if(!strcasecmp(patchDict->dValue.key, member))
		return patchDict;

	while(patchDict != NULL) {
		fileValue = (StringValue*) getValueByKey(patchDict, "File");

		if(!strcasecmp(patchDict->dValue.key, member))
			return patchDict;

		patchDict = (Dictionary*) patchDict->dValue.next;
	}

	return NULL;
}

int patch_file(char* filename) {
	AbstractFile *template = NULL, *inFile, *certificate = NULL, *outFile, *newFile;
	unsigned int key[32], iv[16];
	unsigned int *pkey, *piv;
	char* inData;
	size_t inDataSize;
	char *buffer;
	Dictionary* data;

	template = createAbstractFileFromFile(fopen(filename, "rb"));

	/* open base template */
	if(!template) {
		printf("Cannot open template.\n");
		return -1;
	}

	data = get_key_dictionary_from_bundle(filename);

	StringValue* keyValue = (StringValue*) getValueByKey(data, "Key");
	StringValue* ivValue = (StringValue*) getValueByKey(data, "IV");

	if(keyValue) {
		sscanf(keyValue->value, "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x"
								"%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
			&key[0], &key[1], &key[2], &key[3], &key[4], &key[5], &key[6], &key[7], &key[8],
			&key[9], &key[10], &key[11], &key[12], &key[13], &key[14], &key[15],
			&key[16], &key[17], &key[18], &key[19], &key[20], &key[21], &key[22], &key[23], &key[24],
			&key[25], &key[26], &key[27], &key[28], &key[29], &key[30], &key[31]);
		pkey = key;
	}

	if(ivValue) {
		sscanf(ivValue->value, "%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x%2x",
			&iv[0], &iv[1], &iv[2], &iv[3], &iv[4], &iv[5], &iv[6], &iv[7], &iv[8],
			&iv[9], &iv[10], &iv[11], &iv[12], &iv[13], &iv[14], &iv[15]);
		piv = iv;
	}

	/* open file */
	inFile = openAbstractFile3(createAbstractFileFromFile(fopen(filename, "rb")), pkey, piv, 0);
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
	buffer = malloc(strlen(filename) + 5);
	if(!buffer) {
		printf("Cannot allocate memory\n");
		return -1;
	}
	memset(buffer, 0, strlen(filename) + 5);

	snprintf(buffer, strlen(filename) + 5, "%s.pwn", filename);

	/* open output */
	printf("opening %s as an abstract file\n", filename);

	outFile = createAbstractFileFromFile(fopen(buffer, "wb"));
	if(!outFile) {
		printf("Cannot open outfile\n");
		return -1;
	}

	printf("pwned file is %s, will upload later\n", buffer);

	newFile = duplicateAbstractFile2(template, outFile, pkey, piv, certificate);
	if(!newFile) {
		printf("Cannot open newfile\n");
		return -1;
	}

	/* write patched contents */
	printf("writing pwned file\n");
	newFile->write(newFile, inData, inDataSize);
	newFile->close(newFile);

	free(buffer);

	return 0;
}
