/* opensn0w
 * An open-source jailbreaking utility.
 * Brought to you by rms, acfrazier & Maximus
 * Special thanks to iH8sn0w & MuscleNerd
 *
 * This file is from xpwn.
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

#include <stdlib.h>
#include <stdio.h>
#include <hfs/hfsplus.h>
#ifdef _WIN32
#define exit(x) system("pause"), exit(0)
#endif
void hfs_panic(const char* hfs_panicString) {
  fprintf(stderr, "%s\n", hfs_panicString);
  exit(1);
}

void printUnicode(HFSUniStr255* str) {
  int i;
  
  for(i = 0; i < str->length; i++) {
    printf("%c", (char)(str->unicode[i] & 0xff));
  }
}

char* unicodeToAscii(HFSUniStr255* str) {
  int i;
  char* toReturn;
  
  toReturn = (char*) malloc(sizeof(char) * (str->length + 1));
  
  for(i = 0; i < str->length; i++) {
    toReturn[i] = (char)(str->unicode[i] & 0xff);
  }
  toReturn[i] = '\0';
  
  return toReturn;
}
