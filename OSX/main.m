/* opensn0w 
 * An open-source jailbreaking utility.
 * Brought to you by rms, acfrazier & Maximus
 * Special thanks to iH8sn0w & MuscleNerd
 * OSX GUI by x76
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

#import <Cocoa/Cocoa.h>
#include "main.h"

int main(int argc, char *argv[])
{
//SUX: If trying to debug in Xcode, s/(argc == 1)/(argc == 3)/
    if(argc == 1)
        return NSApplicationMain(argc, (const char **)argv);
    else
        return opensn0w_main(argc, argv);
}
