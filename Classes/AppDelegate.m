//
//  AppDelegate.m
//  MeshMaker
//
//  Created by Filip Kunc on 10/26/12.
//
//

#import "AppDelegate.h"

@implementation AppDelegate

- (void)applicationWillFinishLaunching:(NSNotification *)notification
{
    [[NSUserDefaults standardUserDefaults] registerDefaults:@{ @"WebKitDeveloperExtras" : @YES }];
}

- (IBAction)showHelp:(id)sender
{
    [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:@"http://filipkunc.github.com/MeshMaker/"]];
}

@end
