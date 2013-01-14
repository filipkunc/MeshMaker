//
//  MemoryStream.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/24/12.
//  For license see LICENSE.TXT
//

#include "MemoryStream.h"

#if defined(__APPLE__)

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

void MemoryWriteStream::writeBytes(const void *buffer, uint length)
{
    [_data appendBytes:buffer length:length];
}

#elif defined(WIN32)

MemoryReadStream::MemoryReadStream(System::IO::MemoryStream ^stream)
{
	_stream = stream;
    _version = 0;
}

MemoryReadStream::~MemoryReadStream()
{
	
}

void MemoryReadStream::readBytes(void *buffer, unsigned int length)
{
	array<Byte> ^bytes = gcnew array<Byte>(length);
	_stream->Read(bytes, 0, length);
	pin_ptr<Byte> bytesPointer = &bytes[0];
	memcpy(buffer, bytesPointer, length);
}

MemoryWriteStream::MemoryWriteStream(System::IO::MemoryStream ^stream)
{
    _stream = stream;
    _version = 0;
}

MemoryWriteStream::~MemoryWriteStream()
{
    
}

void MemoryWriteStream::writeBytes(const void *buffer, unsigned int length)
{
	array<Byte> ^bytes = gcnew array<Byte>(length);
	pin_ptr<Byte> bytesPointer = &bytes[0];
	memcpy(bytesPointer, buffer, length);
	_stream->Write(bytes, 0, length);
}


#endif