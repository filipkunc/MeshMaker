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

@end
