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

@interface VLOAppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow * window;
@property (assign) IBOutlet NSView * windowView;
@property (nonatomic, retain) IBOutlet NSView * mainView;
@property (nonatomic, retain) IBOutlet NSView * extrasView;
@property (assign) NSRect mainSubviewFrame;

-(IBAction)buttonJailbreak:(id)sender;
-(IBAction)buttonExtras:(id)sender;
-(IBAction)buttonPwnDFU:(id)sender;
-(IBAction)buttonExitRecovery:(id)sender;
-(IBAction)buttonBootTethered:(id)sender;
-(IBAction)button1337Mode:(id)sender;
-(IBAction)buttonMainMenu:(id)sender;

@end
