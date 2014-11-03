//
//  MemoryStream.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/24/12.
//  For license see LICENSE.TXT
//

#pragma once

#include <Foundation/Foundation.h>

class MemoryReadStream
{
private:
    __unsafe_unretained NSData *_data;
    NSUInteger _lastReadPosition;
    unsigned int _version;
public:
    MemoryReadStream(NSData *data);
    ~MemoryReadStream();

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
private:
    __unsafe_unretained NSMutableData *_data;
    unsigned int _version;
public:
    MemoryWriteStream(NSMutableData *data);
    ~MemoryWriteStream();

    unsigned int version() { return _version; }
    void setVersion(unsigned int value) { _version = value; }
    void writeBytes(const void *buffer, unsigned int length);
    
    template <class T>
    void write(const T &value)
    {
        writeBytes(&value, sizeof(T));
    }
};
