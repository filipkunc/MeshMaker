//
//  MemoryStream.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/24/12.
//  For license see LICENSE.TXT
//

#pragma once

#if defined(__APPLE__)
#include <Foundation/Foundation.h>
#elif defined(WIN32)
#include <windows.h>
#include <vcclr.h>
using namespace System;
using namespace System::IO;
#endif

class MemoryReadStream
{
#if defined(__APPLE__)
private:
    __unsafe_unretained NSData *_data;
    NSUInteger _lastReadPosition;
    unsigned int _version;
public:
    MemoryReadStream(NSData *data);
    ~MemoryReadStream();
#elif defined(WIN32)
private:
	gcroot<System::IO::MemoryStream ^> _stream;
	unsigned int _version;
public:
	MemoryReadStream(System::IO::MemoryStream ^stream);
	~MemoryReadStream();
#endif    
    unsigned int version() { return _version; }
    void setVersion(unsigned int value) { _version = value; }
    void readBytes(void *buffer, unsigned int length);
};

class MemoryWriteStream
{
#if defined(__APPLE__)
private:
    __unsafe_unretained NSMutableData *_data;
    unsigned int _version;
public:
    MemoryWriteStream(NSMutableData *data);
    ~MemoryWriteStream();
#elif defined(WIN32)
private:
	gcroot<System::IO::MemoryStream ^> _stream;
    unsigned int _version;
public:
	MemoryWriteStream(System::IO::MemoryStream ^stream);
    ~MemoryWriteStream();
#endif
    unsigned int version() { return _version; }
    void setVersion(unsigned int value) { _version = value; }
    void writeBytes(void *buffer, unsigned int length);
};
