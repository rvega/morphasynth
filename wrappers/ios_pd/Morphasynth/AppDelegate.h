//
//  AppDelegate.h
//  Morphasynth
//
//  Created by Rafael Vega on 3/13/13.
//  Copyright (c) 2013 RafaelVega. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "PdBase.h"
#import "PdAudioController.h"

@class ViewController;

@interface AppDelegate : UIResponder <UIApplicationDelegate, PdReceiverDelegate>

@property (strong, nonatomic) UIWindow *window;
@property (nonatomic, retain) PdAudioController *audioController;
@property (strong, nonatomic) ViewController *viewController;

@end
