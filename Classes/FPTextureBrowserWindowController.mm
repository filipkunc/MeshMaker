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

- (IBAction)setTexture:(id)sender;
- (IBAction)addTexture:(id)sender;

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
- (void)setItems:(ItemCollection *)anItems textures:(TextureCollection *)aTextures
{
    items = anItems;
    textures = aTextures;
    [textureList reloadData];
}

- (NSInteger)numberOfRowsInTableView:(NSTableView *)tableView
{
    return textures->count();
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(NSInteger)row
{
    NSString *name = textures->textureAtIndex(row)->name();
    if (name != nil)
        return name;
    return @"<Empty>";
}

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
    int selectedRow = textureList.selectedRow;
    if (selectedRow >= 0 && selectedRow < (int)textures->count())
    {
        [textureView setImage:textures->textureAtIndex(selectedRow)->image()];
    }
    else
    {
        [textureView setImage:nil];
    }
}

- (IBAction)setTexture:(id)sender
{
    int selectedRow = textureList.selectedRow;
    if (selectedRow >= 0 && selectedRow < (int)textures->count())
    {
        Mesh2 *mesh = items->currentMesh();
        if (mesh != NULL)
        {
            Texture *texture = textures->textureAtIndex(selectedRow);
            texture->setName([textureView imageName]);
            texture->setImage([textureView image]);
            mesh->setTexture(texture);
        }
    }
}

- (IBAction)addTexture:(id)sender
{
    textures->addTexture(new Texture());
    [textureList reloadData];
}

@end

