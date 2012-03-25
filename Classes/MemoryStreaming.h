//
//  CppFileStreaming.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/21/09.
//  For license see LICENSE.TXT
//

#import "MemoryStream.h"

@protocol MemoryStreaming

- (id)initWithReadStream:(MemoryReadStream *)stream;
- (void)encodeWithWriteStream:(MemoryWriteStream *)stream;

@end
