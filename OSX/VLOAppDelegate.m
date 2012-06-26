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
@synthesize windowSubviewFrame;
@synthesize currentSubview;
@synthesize mainView;
@synthesize extrasView;
@synthesize DFUView;
@synthesize recoveryFixView;
@synthesize leetView;
@synthesize topLabel; 
@synthesize bottomLabel;
@synthesize DFUModeHelperText;
@synthesize controlTimer;
@synthesize timerRunning;

-(void)dealloc
{
    [super dealloc];
}

-(NSString*)version
{
    NSString * version = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    NSString * build = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleVersion"];
    return [NSString stringWithFormat:@"%@%@", version, build];
}

-(void)updateSubview:(NSView *)newView
{
    [newView setFrame:windowSubviewFrame];
    if(currentSubview != nil)
    {
        [[_window contentView] replaceSubview:currentSubview with:newView];
    }
    [self setCurrentSubview:newView];
}

-(void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    // Disable window resizing
    [[_window standardWindowButton:NSWindowZoomButton] setEnabled:NO];
    [_window setStyleMask:[_window styleMask]^NSResizableWindowMask];
	
    // Keep window visible when inactive
    [_window setHidesOnDeactivate:NO];
	
    // Set top label
    [topLabel setStringValue:@"what would you like to do?"];
    
    // Set bottom label
    [bottomLabel setStringValue:[NSString stringWithFormat:@"opensn0w - version %@ - by vlo and friends", [self version]]];
    
    // Remember current view
    [self setWindowSubviewFrame:[windowView frame]];
    [self setCurrentSubview:windowView];
    
    // Display main menu
    [self updateSubview:mainView];
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender
{ 
    return YES; 
}

-(void)DFUModeHelper
{   
    // Should probably go back and care about thread safety
    [self setTimerRunning:NO];
    [controlTimer setTitle:@"Start Timer"];
    [DFUModeHelperText setStringValue:@"Waiting for device in DFU mode.\n\nFor assistance in entering DFU mode, connect device to computer, then power off. After device is off, press \"Start Timer\" below."];
    while(1)
    {
        
    }
    return;
}

-(IBAction)buttonJailbreak:(id)sender
{
    [topLabel setStringValue:@"let's get ready"];
    [self updateSubview:DFUView];
    [NSThread detachNewThreadSelector:@selector(DFUModeHelper) toTarget:self withObject:nil];
}

-(IBAction)buttonExtras:(id)sender
{
    [topLabel setStringValue:@"what would you like to do?"];
    [self updateSubview:extrasView];
}

-(IBAction)buttonPwnDFU:(id)sender
{
    [topLabel setStringValue:@"let's get ready"];
    [self updateSubview:DFUView];
    [NSThread detachNewThreadSelector:@selector(DFUModeHelper) toTarget:self withObject:nil];
}

-(IBAction)buttonExitRecovery:(id)sender
{
    //[topLabel setStringValue:@"fixing recovery loop"];
    [topLabel setStringValue:@"not implemented yet"];
    [self updateSubview:recoveryFixView];
}

-(IBAction)buttonBootTethered:(id)sender
{
    [topLabel setStringValue:@"let's get ready"];
    [self updateSubview:DFUView];
    [NSThread detachNewThreadSelector:@selector(DFUModeHelper) toTarget:self withObject:nil];
}

-(IBAction)buttonLeetMode:(id)sender
{
    [topLabel setStringValue:@"not implemented yet"];
    [self updateSubview:leetView];
}

-(IBAction)buttonMainMenu:(id)sender
{
    [topLabel setStringValue:@"what would you like to do?"];
    [self updateSubview:mainView];
}

-(IBAction)buttonControlTimer:(id)sender
{
    if(timerRunning)
    {
        // Reset timer
        [self setTimerRunning:NO];
        [controlTimer setTitle:@"Start Timer"];
    }
    else
    {
        // Start timer
        [self setTimerRunning:YES];
        [controlTimer setTitle:@"Reset Timer"];
    }
}

@end
