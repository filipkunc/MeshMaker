//
//  MemoryStream.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/24/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MemoryReadStream : NSObject
{
    NSData *_data;
    NSUInteger _lastReadPosition;
}

- (id)initWithData:(NSData *)data;
- (void)readBytes:(void *)buffer length:(NSUInteger)length;

@end

@interface MemoryWriteStream : NSObject
{
    NSMutableData *_data;
}

- (id)initWithData:(NSMutableData *)data;
- (void)writeBytes:(void *)buffer length:(NSUInteger)length;

@end
