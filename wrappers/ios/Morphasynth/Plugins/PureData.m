//
//  LibPD.m
//  HallucinAudio
//
//  Created by Rafael Vega on 5/5/13.
//
//

#import "PureData.h"
#import "PdBase.h"
#import "PdFile.h"

@implementation PureData

////////////////////////////////////////////////////////////////////////
#pragma mark --
#pragma mark Externals

/**
  * Go ahead and setup your externals here.
  */
extern void expr_setup(void);
extern void limit_setup(void);
extern void list2symbol_setup(void);
extern void resonant_hpf_setup(void);
extern void moog_lopass_setup(void);
extern void matrix_setup(void);
extern void directory_setup(void);
extern void repack_setup(void);
extern void sum_setup(void);

-(void) setupExternals{
  expr_setup();
  limit_setup();
  list2symbol_setup();
  resonant_hpf_setup();
  moog_lopass_setup();
  matrix_setup();
  directory_setup();
  repack_setup();
  sum_setup();
}


////////////////////////////////////////////////////////////////////////
#pragma mark --
#pragma mark Accessors

@synthesize audioController = _audioController;

// This getter will instantiate _audioController when needed.
- (PdAudioController*)audioController{
  if(_audioController == nil){
    self.audioController = [[[PdAudioController alloc] init] autorelease];
  }
  return _audioController;
}

////////////////////////////////////////////////////////////////////////
#pragma mark --
#pragma mark Javascript Visible Methods

- (void)configurePlayback:(CDVInvokedUrlCommand*)command {
  int sampleRate = [(NSNumber*)[command.arguments objectAtIndex:0] intValue];
  int numberChannels = [(NSNumber*)[command.arguments objectAtIndex:1] intValue];
  int inputEnabled = [(NSNumber*)[command.arguments objectAtIndex:2] boolValue];
  int mixingEnabled = [(NSNumber*)[command.arguments objectAtIndex:3] boolValue];
  
	PdAudioStatus status = [self.audioController configurePlaybackWithSampleRate:sampleRate numberChannels:numberChannels inputEnabled:inputEnabled mixingEnabled:mixingEnabled];

  CDVPluginResult* pluginResult = nil;
  if(status == PdAudioOK){
    [self setupExternals];
    
    pluginResult = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
  }
  else if(status == PdAudioPropertyChanged){
    sampleRate = [self.audioController sampleRate];
    numberChannels = [self.audioController numberChannels];
    inputEnabled = [self.audioController inputEnabled];
    mixingEnabled = [self.audioController mixingEnabled];
    NSDictionary* params = [NSDictionary dictionaryWithObjectsAndKeys:
      [NSNumber numberWithInt:sampleRate], @"sampleRate",
      [NSNumber numberWithInt:numberChannels], @"numberChannels",
      [NSNumber numberWithBool:inputEnabled], @"inputEnabled",
      [NSNumber numberWithBool:mixingEnabled], @"mixingEnabled",
    nil];
    [self setupExternals];
    
    pluginResult = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK messageAsDictionary:params];
  }
  else{ // if(status == pdAudioError)
    // TODO: test this.
    NSString* msg = [NSString stringWithFormat:@"Could not configure audio playback. SampleRate:%i, NumChannels:%i, inputEnabled:%d, mixingEnabled:%d", sampleRate, numberChannels, inputEnabled, mixingEnabled];
    pluginResult = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:msg];
  }

  [self.commandDelegate sendPluginResult:pluginResult callbackId:command.callbackId];

  // TODO: where should I put this?
  [PdBase setDelegate:self];
}

- (void)openFile:(CDVInvokedUrlCommand*)command {
  NSString* dir = [command.arguments objectAtIndex:0];
  NSString* fileName = [command.arguments objectAtIndex:1];
  NSString* path = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:dir];

  CDVPluginResult* pluginResult = nil;
  if([PdBase openFile:fileName path:path]==nil){
    NSString* msg = [NSString stringWithFormat:@"Could not open PD patch %@/%@", path, fileName];
    pluginResult = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:msg];
  }
  else{
    // TODO: return dollar-zero?
    pluginResult = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
  }
  [self.commandDelegate sendPluginResult:pluginResult callbackId:command.callbackId];
}

- (void)setActive:(CDVInvokedUrlCommand*)command {
  BOOL active = [[command.arguments objectAtIndex:0] boolValue];

  [self.audioController setActive:active];

  CDVPluginResult* pluginResult = nil;
  if([self.audioController isActive]!=active){
    NSString* msg = [NSString stringWithFormat:@"Could not (de)activate audio controller. active: %d", active];
    pluginResult = [CDVPluginResult resultWithStatus:CDVCommandStatus_ERROR messageAsString:msg];
  }
  else{
    pluginResult = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
  }
  [self.commandDelegate sendPluginResult:pluginResult callbackId:command.callbackId];
}

- (void)sendFloat:(CDVInvokedUrlCommand*)command {
  float f = [[command.arguments objectAtIndex:0] floatValue];
  NSString* receiver = [command.arguments objectAtIndex:1];

  [PdBase sendFloat:f toReceiver:receiver];

  CDVPluginResult* pluginResult = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
  [self.commandDelegate sendPluginResult:pluginResult callbackId:command.callbackId];

}

- (void)sendNoteOn:(CDVInvokedUrlCommand*)command {
  float note = [[command.arguments objectAtIndex:0] floatValue];
  float vel = [[command.arguments objectAtIndex:0] floatValue];
  [PdBase sendNoteOn:0 pitch:note velocity:vel];

  CDVPluginResult* pluginResult = [CDVPluginResult resultWithStatus:CDVCommandStatus_OK];
  [self.commandDelegate sendPluginResult:pluginResult callbackId:command.callbackId];
}

  // TODO: where should I put this?
  /* [PdBase setDelegate:self]; */
- (void)receivePrint:(NSString *)message {
  NSLog(@"Pd Console: %@", message);
}

@end
