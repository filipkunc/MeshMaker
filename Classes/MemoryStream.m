//
//  MemoryStream.m
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/24/12.
//  For license see LICENSE.TXT
//

#import "MemoryStream.h"

@implementation MemoryReadStream

- (id)initWithData:(NSData *)data
{
    self = [super init];
    if (self)
    {
        _data = data;
        _lastReadPosition = 0;
    }
    return self;
}

- (void)readBytes:(void *)buffer length:(NSUInteger)length
{
    [_data getBytes:buffer range:NSMakeRange(_lastReadPosition, length)];
    _lastReadPosition += length;
}

@end

@implementation MemoryWriteStream

- (id)initWithData:(NSMutableData *)data
{
    self = [super init];
    if (self)
    {
        _data = data;
    }
    return self;
}

- (void)writeBytes:(void *)buffer length:(NSUInteger)length
{
    [_data appendBytes:buffer length:length];
}

@end
