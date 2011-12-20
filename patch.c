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
	FILE *fp = fopen(filename, "rb");
	int size, i;
	char *filelocation, *buffer;

	/* check to make sure the file exists */
	if(!fp)
		return -1;

	/* get size */
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	/* alloc buffer */
	buffer = malloc(size);

	if(!buffer)
		return -1;

	/* read file into it */
	fread(buffer, 1, size, fp);

	/* NOT DONE */
	for(i = 0; i < size; i++) {
		filelocation = &buffer[i];
	}
}
