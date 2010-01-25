//
//  MyTestObject.h
//  PropertyGrid
//
//  Created by Filip Kunc on 1/23/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>

@interface MyTestObject : NSObject 
{
	float a, b;
}

@property (readwrite, assign) float a, b;

@end
