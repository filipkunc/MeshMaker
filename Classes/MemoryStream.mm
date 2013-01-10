//
//  MemoryStream.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/24/12.
//  For license see LICENSE.TXT
//

#import "MemoryStream.h"

MemoryReadStream::MemoryReadStream(NSData *data)
{
    _data = data;
    _lastReadPosition = 0;
    _version = 0;
}

MemoryReadStream::~MemoryReadStream()
{
}

void MemoryReadStream::readBytes(void *buffer, uint length)
{
    [_data getBytes:buffer range:NSMakeRange(_lastReadPosition, length)];
    _lastReadPosition += length;
}

MemoryWriteStream::MemoryWriteStream(NSMutableData *data)
{
    _data = data;
    _version = 0;
}

MemoryWriteStream::~MemoryWriteStream()
{
    
}

void MemoryWriteStream::writeBytes(void *buffer, uint length)
{
    [_data appendBytes:buffer length:length];
}

