//
//  AppDelegate.m
//  Morphasynth
//
//  Created by Rafael Vega on 3/13/13.
//  Copyright (c) 2013 RafaelVega. All rights reserved.
//

#import "AppDelegate.h"
#import "ViewController.h"
#import "PdBase.h"

extern void expr_setup(void);
extern void limit_setup(void);
extern void list2symbol_setup(void);
extern void resonant_hpf_setup(void);
extern void moog_lopass_setup(void);

@implementation AppDelegate

- (void)dealloc
{
    [_window release];
    [_viewController release];
    [super dealloc];
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    [self initPd];
    
    self.window = [[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]] autorelease];
    // Override point for customization after application launch.
    self.viewController = [[[ViewController alloc] initWithNibName:@"ViewController" bundle:nil] autorelease];
    self.window.rootViewController = self.viewController;
    [self.window makeKeyAndVisible];
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

#pragma mark - PdStuff
-(void)initPd{
    self.audioController = [[[PdAudioController alloc] init] autorelease];
	[self.audioController configurePlaybackWithSampleRate:44100 numberChannels:2 inputEnabled:NO mixingEnabled:NO];
    
    expr_setup();
    limit_setup();
    list2symbol_setup();
    resonant_hpf_setup();
    moog_lopass_setup();
    
	NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"patches"];
    [PdBase setDelegate:self];
    [PdBase openFile:@"5.ios.pd" path:path];
    
    // The patch might need it's path in the filesystem
    [PdBase sendMessage:path withArguments:nil toReceiver:@"path"];

	// Start DSP processing
    [self.audioController setActive:YES];
	
    //[self.audioController print];
}

- (void)receivePrint:(NSString *)message {
    NSLog(@"Pd Console: %@", message);
}


@end
