//
//  Item.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/28/09.
//  For license see LICENSE.TXT
//

#import "OpenGLDrawing.h"
#import "Item.h"

@implementation Item

@synthesize selected, mesh, visible;

- (id)init
{
	self = [super init];
	if (self)
	{
		position = new Vector3D();
		rotation = new Quaternion();
		scale = new Vector3D(1, 1, 1);
		mesh = new Mesh2();
		selected = NO;
		visible = YES;
        viewMode = ViewModeSolidFlat;
	}
	return self;
}

- (id)initWithPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale
{
	self = [self init];
	if (self)
	{
		*position = aPosition;
		*rotation = aRotation;
		*scale = aScale;
	}
	return self;
}

#pragma mark MemoryStreaming implementation

- (id)initWithReadStream:(MemoryReadStream *)stream
{
	self = [super init];
	if (self)
	{
		position = new Vector3D();
        [stream readBytes:position length:sizeof(Vector3D)];
		
		rotation = new Quaternion();
        [stream readBytes:rotation length:sizeof(Quaternion)];
		
		scale = new Vector3D();
        [stream readBytes:scale length:sizeof(Vector3D)];
		
        [stream readBytes:&selected length:sizeof(BOOL)];
		visible = YES;
		
		mesh = new Mesh2(stream);
	}
	return self;
}

- (void)encodeWithWriteStream:(MemoryWriteStream *)stream
{
    [stream writeBytes:position length:sizeof(Vector3D)];
    [stream writeBytes:rotation length:sizeof(Quaternion)];
    [stream writeBytes:scale length:sizeof(Vector3D)];
    [stream writeBytes:&selected length:sizeof(BOOL)];
	
    mesh->encode(stream);    
}

- (void)dealloc
{
	delete position;
	delete rotation;
	delete scale;
}

- (Vector3D)position
{
	return *position;
}

- (void)setPosition:(Vector3D)aPosition
{
	*position = aPosition;
}

- (Quaternion)rotation
{
	return *rotation;
}

- (void)setRotation:(Quaternion)aRotation
{
	*rotation = aRotation;
}

- (Vector3D)scale
{
	return *scale;
}

- (void)setScale:(Vector3D)aScale
{
	*scale = aScale;
}

- (Matrix4x4)transform
{
    Matrix4x4 m;
    m.TranslateRotateScale(*position, *rotation, *scale);
    return m;
}

- (void)moveByOffset:(Vector3D)offset
{
	*position += offset;
}

- (void)rotateByOffset:(Quaternion)offset
{
	*rotation = offset * *rotation;
}

- (void)scaleByOffset:(Vector3D)offset
{
	*scale += offset;
}

- (Item *)duplicate
{
	Item *newItem = [[Item alloc] init];

	[newItem setPosition:[self position]];
	[newItem setRotation:[self rotation]];
	[newItem setScale:[self scale]];
	
    newItem->mesh->merge(self->mesh);
    newItem->mesh->setColor(self->mesh->color());
    [newItem->mesh->texture() setCanvas:self->mesh->texture().canvas];
	[newItem setSelected:[self selected]];
	
	return newItem;
}

- (uint)count 
{ 
    return mesh->selectedCount();
}

- (void)willSelectThrough:(BOOL)selectThrough
{
    Mesh2::setSelectThrough(selectThrough);
}

- (BOOL)needsCullFace
{
    if (mesh->selectionMode() == MeshSelectionModeTriangles && Mesh2::selectThrough())
        return YES;
    return NO;
}

- (void)didSelect
{
    mesh->resetTriangleCache();
    mesh->computeSoftSelection();
}

- (void)getSelectionCenter:(Vector3D *)aCenter 
				  rotation:(Quaternion *)aRotation
					 scale:(Vector3D *)aScale
{
    mesh->getSelectionCenterRotationScale(*aCenter, *aRotation, *aScale);
}

- (void)transformSelectedByMatrix:(Matrix4x4 *)matrix
{
    mesh->transformSelected(*matrix);
}

- (BOOL)isSelectedAtIndex:(uint)index
{
    return mesh->isSelectedAtIndex(index);
}

- (void)setSelected:(BOOL)isSelected atIndex:(uint)index
{
    mesh->setSelectedAtIndex(isSelected, index);
}

- (void)expandSelectionFromIndex:(uint)index
{
    mesh->expandSelectionFromIndex(index);
}

- (void)drawForSelection:(BOOL)forSelection
{
	if (visible)
	{
		glPushMatrix();
		glTranslatef(position->x, position->y, position->z);
		Matrix4x4 rotationMatrix = rotation->ToMatrix();
		glMultMatrixf(rotationMatrix);
        if (forSelection)
        {
            mesh->draw(ViewModeSolidFlat, *scale, selected, forSelection);
        }
        else
        {
            if (viewMode == ViewModeMixedWireSolid)
            {
                glDisable(GL_DEPTH_TEST);
                mesh->draw(viewMode, *scale, selected, forSelection);
                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                mesh->draw(viewMode, *scale, selected, forSelection);
            }
        }
		glPopMatrix();
	}
}

- (void)drawAllForSelection:(BOOL)forSelection
{
    if (forSelection)
    {
        mesh->drawAll(ViewModeSolidFlat, forSelection);
    }
    else
    {
        if (viewMode == ViewModeMixedWireSolid)
        {
            glDisable(GL_DEPTH_TEST);
            mesh->drawAll(viewMode, forSelection);
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            mesh->drawAll(viewMode, forSelection);
        }
    }
}

- (BOOL)useGLProject
{
    return mesh->useGLProject();
}

- (void)glProjectSelectWithX:(int)x 
                           y:(int)y
                       width:(int)width 
                      height:(int)height 
                   transform:(Matrix4x4 *)matrix 
               selectionMode:(enum OpenGLSelectionMode)selectionMode
{
    mesh->glProjectSelect(x, y, width, height, *matrix, selectionMode);
}

- (void)duplicateSelected
{
    mesh->duplicateSelectedTriangles();
}

- (void)removeSelected
{
    mesh->removeSelected();
}

- (void)hideSelected
{
    mesh->hideSelected();
}

- (void)unhideAll
{
    mesh->unhideAll();
}

- (NSColor *)selectionColor
{
    return mesh->color();
}

- (void)setSelectionColor:(NSColor *)selectionColor
{
    mesh->setColor(selectionColor);
}

- (enum ViewMode)viewMode
{
    return viewMode;
}

- (void)setViewMode:(enum ViewMode)aViewMode
{
    viewMode = aViewMode;
    if (viewMode == ViewModeUnwrap)
        mesh->setUnwrapped(true);
    else
        mesh->setUnwrapped(false);
}

@end
