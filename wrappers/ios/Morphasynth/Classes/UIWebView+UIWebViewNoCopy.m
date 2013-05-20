//
//  UIWebView+UIWebViewNoCopy.m
//  Morphasynth
//
//  Created by Rafael Vega on 5/20/13.
//
//

#import "UIWebView+UIWebViewNoCopy.h"

@implementation UIWebView (UIWebViewNoCopy)

- (BOOL)canPerformAction:(SEL)action withSender:(id)sender{
  if (action == @selector(copy:) || action == @selector(paste:)|| action == @selector(cut:)){
    return NO;
  }
  return [super canPerformAction:action withSender:sender];
}

@end
