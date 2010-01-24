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
	NSString *attributes;
}

@property (readonly, retain) NSString *name;
@property (readonly, retain) NSString *attributes;

- (id)initWithName:(NSString *)aName attributes:(NSString *)anAttributes;
- (unichar)type;
- (NSCell *)cell;

@end
