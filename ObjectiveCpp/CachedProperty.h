//
//  CachedProperty.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/20/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>

@interface CachedProperty : NSObject
{
	NSString *name;
	unichar type;
}

@property (readonly, retain) NSString *name;
@property (readonly, assign) unichar type;

- (id)initWithName:(NSString *)aName type:(unichar)aType;
- (NSCell *)cell;

@end
