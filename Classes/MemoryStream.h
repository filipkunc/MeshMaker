//
//  MemoryStream.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/24/12.
//  For license see LICENSE.TXT
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
