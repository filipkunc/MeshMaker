//
//  FPImageView.m
//  MeshMaker
//
//  Created by Filip Kunc on 1/26/13.
//
//

#import "FPImageView.h"

@implementation FPImageView

- (void)setImage:(NSImage *)image
{
    [image setName:[_imageName.lastPathComponent stringByDeletingPathExtension]];
    [super setImage:image];
}

- (BOOL)performDragOperation:(id )sender
{
    if (![super performDragOperation:sender])
        return NO;

    NSString *filenamesXML = [[sender draggingPasteboard] stringForType:NSFilenamesPboardType];
    if (!filenamesXML)
        return YES;
    
    NSArray *filenames = [NSPropertyListSerialization
                          propertyListFromData:[filenamesXML dataUsingEncoding:NSUTF8StringEncoding]
                          mutabilityOption:NSPropertyListImmutable
                          format:nil
                          errorDescription:nil];
    
    if ([filenames count] >= 1)
        _imageName = filenames[0];
    else
        _imageName = nil;
    
    return YES;
}


@end
