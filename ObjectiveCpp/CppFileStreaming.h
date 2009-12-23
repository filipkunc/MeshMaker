//
//  CppFileStreaming.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/21/09.
//  For license see LICENSE.TXT
//

#import <Cocoa/Cocoa.h>
#import <fstream>
using namespace std;

@protocol CppFileStreaming

- (void)encodeWithFileStream:(ofstream *)fout;
- (id)initWithFileStream:(ifstream *)fin;

@end
