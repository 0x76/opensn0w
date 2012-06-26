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

#import "VLOAppDelegate.h"

@implementation VLOAppDelegate

@synthesize window = _window;
@synthesize windowView;
@synthesize mainView;
@synthesize extrasView;
@synthesize mainSubviewFrame;

- (void)dealloc
{
    [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Disable window resizing
    [[_window standardWindowButton:NSWindowZoomButton] setEnabled:NO];
    [_window setStyleMask:[_window styleMask]^NSResizableWindowMask];
	
    // Keep window visible when inactive
    [_window setHidesOnDeactivate:NO];
	
    // Display start screen
    [mainView setFrame:[windowView frame]];
    [[_window contentView] replaceSubview:windowView with:mainView];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{ 
    return YES; 
}

-(IBAction)buttonJailbreak:(id)sender
{
    
}

-(IBAction)buttonExtras:(id)sender
{
    [extrasView setFrame:[mainView frame]];
    [[_window contentView] replaceSubview:mainView with:extrasView];
}

-(IBAction)buttonPwnDFU:(id)sender
{
    
}

-(IBAction)buttonExitRecovery:(id)sender
{
    
}

-(IBAction)buttonBootTethered:(id)sender
{
    
}

-(IBAction)button1337Mode:(id)sender
{
    
}

-(IBAction)buttonMainMenu:(id)sender
{
    [mainView setFrame:[extrasView frame]];
    [[_window contentView] replaceSubview:extrasView with:mainView];
}

@end
