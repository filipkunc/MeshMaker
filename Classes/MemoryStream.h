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
    unsigned int _version;
}

@property (readwrite, assign) unsigned int version;

- (id)initWithData:(NSData *)data;
- (void)readBytes:(void *)buffer length:(NSUInteger)length;

@end

@interface MemoryWriteStream : NSObject
{
    NSMutableData *_data;
    unsigned int _version;
}

@property (readwrite, assign) unsigned int version;

- (id)initWithData:(NSMutableData *)data;
- (void)writeBytes:(void *)buffer length:(NSUInteger)length;

@end
