//
//  MyTestObject.h
//  PropertyGrid
//
//  Created by Filip Kunc on 1/23/10.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#include "Vector3D.h"

@interface MyTestObject : NSObject 
{
	float a, b;
	Vector3D test;
}

@property (readwrite, assign) float a, b;
@property (readwrite, assign) Vector3D test;

@end
