//
//  LibPD.h
//  HallucinAudio
//
//  Created by Rafael Vega on 5/5/13.
//
//

#import <Foundation/Foundation.h>
#import <Cordova/CDV.h>
#import "PdBase.h"
#import "PdAudioController.h"

@interface PureData : CDVPlugin <PdReceiverDelegate>

@property (nonatomic, retain) PdAudioController *audioController;

- (void)configurePlayback:(CDVInvokedUrlCommand*)command;
- (void)openFile:(CDVInvokedUrlCommand*)command;

@end
