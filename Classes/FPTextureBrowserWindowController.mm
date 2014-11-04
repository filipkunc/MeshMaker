//
//  FPTextureBrowserWindowController.mm
//  MeshMaker
//
//  Created by Filip Kunc on 4/14/12.
//  For license see LICENSE.TXT
//

#import "FPTextureBrowserWindowController.h"
#import "FPImageView.h"

@interface FPTextureBrowserWindowController ()  <NSTableViewDataSource, NSTableViewDelegate>
{
    IBOutlet NSTableView *textureList;
    IBOutlet FPImageView *textureView;
}

@end

@implementation FPTextureBrowserWindowController

- (id)init
{
    self = [super initWithWindowNibName:@"FPTextureBrowserWindowController"];
    return self;
}

- (void)windowDidLoad
{
    [super windowDidLoad];
    
    // Implement this method to handle any initialization after your window controller's window has been loaded from its nib file.
}

- (void)reloadData
{
    [textureList reloadData];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    return _delegate.textures->count();
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSString *name = _delegate.textures->textureAtIndex(static_cast<unsigned int>(row))->name();
    if (name != nil)
        return [name lastPathComponent];
    return @"<Empty>";
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
    int selectedRow = static_cast<int>(textureList.selectedRow);
    if (selectedRow >= 0 && selectedRow < (int)_delegate.textures->count())
    {
        Texture *texture = _delegate.textures->textureAtIndex(selectedRow);
        
        [textureView setImageName:texture->name()];
        [textureView setImage:texture->image()];
    }
    else
    {
        [textureView setImageName:nil];
        [textureView setImage:nil];
    }
}

- (IBAction)setTexture:(id)sender
{
    Texture *texture;
    
    int selectedRow = static_cast<int>(textureList.selectedRow);
    if (selectedRow >= 0 && selectedRow < (int)_delegate.textures->count())
    {
        texture = _delegate.textures->textureAtIndex(selectedRow);
    }
    else
    {
        texture = new Texture();
        _delegate.textures->addTexture(texture);
    }
    
    texture->setName([textureView imageName]);
    texture->setImage([textureView image]);
    [textureList reloadData];
        
    Mesh2 *mesh = _delegate.items->currentMesh();
    if (mesh != NULL)
        mesh->setTexture(texture);
    
    [_delegate setNeedsDisplayOnAllViews];
}

- (IBAction)newTexture:(id)sender
{
    _delegate.textures->addTexture(new Texture());
    [textureList reloadData];
}

- (IBAction)deleteTexture:(id)sender
{
    int selectedRow = static_cast<int>(textureList.selectedRow);
    if (selectedRow >= 0 && selectedRow < (int)_delegate.textures->count())
    {
        _delegate.textures->removeTextureAtIndex(selectedRow, *_delegate.items);
        [textureList reloadData];
    }
}

@end

