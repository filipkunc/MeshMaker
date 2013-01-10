//
//  MemoryStream.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 3/24/12.
//  For license see LICENSE.TXT
//

#import <Foundation/Foundation.h>

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
    void readBytes(void *buffer, uint length);
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
    void writeBytes(void *buffer, uint length);
};

/*@interface MemoryReadStream : NSObject
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

@end*/
