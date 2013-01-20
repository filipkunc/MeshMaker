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
#elif defined(__linux__)
#include <vector>
using namespace std;
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
#elif defined(__linux__)
private:
    vector<unsigned char> *_bytes;
    unsigned int _lastReadPosition;
    unsigned int _version;
public:
    MemoryReadStream(vector<unsigned char> *bytes);
    ~MemoryReadStream();
#endif    
    unsigned int version() { return _version; }
    void setVersion(unsigned int value) { _version = value; }
    void readBytes(void *buffer, unsigned int length);

    template <class T>
    T read()
    {
        T t;
        readBytes(&t, sizeof(T));
        return t;
    }
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
#elif defined(__linux__)
private:
    vector<unsigned char> *_bytes;
    unsigned int _lastWritePosition;
    unsigned int _version;
public:
    MemoryWriteStream(vector<unsigned char> *bytes);
    ~MemoryWriteStream();
#endif
    unsigned int version() { return _version; }
    void setVersion(unsigned int value) { _version = value; }
    void writeBytes(const void *buffer, unsigned int length);
    
    template <class T>
    void write(const T &value)
    {
        writeBytes(&value, sizeof(T));
    }
};
