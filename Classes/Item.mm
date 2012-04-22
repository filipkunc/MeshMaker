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
		mesh = [[Mesh alloc] init];
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
		
		mesh = [[Mesh alloc] initWithReadStream:stream];
	}
	return self;
}

- (void)encodeWithWriteStream:(MemoryWriteStream *)stream
{
    [stream writeBytes:position length:sizeof(Vector3D)];
    [stream writeBytes:rotation length:sizeof(Quaternion)];
    [stream writeBytes:scale length:sizeof(Vector3D)];
    [stream writeBytes:&selected length:sizeof(BOOL)];
	
	[mesh encodeWithWriteStream:stream];
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
	
    [newItem->mesh mergeWithMesh:self->mesh];
    [newItem->mesh setColor:self->mesh.color];
    [newItem->mesh setImage:self->mesh.image];
    
	[newItem setSelected:[self selected]];
	
	return newItem;
}

- (uint)count 
{ 
    return mesh.count;
}

- (void)willSelectThrough:(BOOL)selectThrough
{
    [mesh willSelectThrough:selectThrough];
}

- (BOOL)needsCullFace
{
    return mesh.needsCullFace;
}

- (void)didSelect
{
    [mesh didSelect];
}

- (void)getSelectionCenter:(Vector3D *)aCenter 
				  rotation:(Quaternion *)aRotation
					 scale:(Vector3D *)aScale
{
    [mesh getSelectionCenter:aCenter rotation:aRotation scale:aScale];
}

- (void)transformSelectedByMatrix:(Matrix4x4 *)matrix
{
    [mesh transformSelectedByMatrix:matrix];
}

- (BOOL)isSelectedAtIndex:(uint)index
{
    return [mesh isSelectedAtIndex:index];
}

- (void)setSelected:(BOOL)isSelected atIndex:(uint)index
{
    [mesh setSelected:isSelected atIndex:index];
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
            [mesh drawWithMode:ViewModeSolidFlat scale:*scale selected:selected forSelection:forSelection];
        }
        else
        {
            if (viewMode == ViewModeMixedWireSolid)
            {
                glDisable(GL_DEPTH_TEST);
                [mesh drawWithMode:viewMode scale:*scale selected:selected forSelection:forSelection];
                glEnable(GL_DEPTH_TEST);
            }
            else
            {
                [mesh drawWithMode:viewMode scale:*scale selected:selected forSelection:forSelection];
            }
        }
		glPopMatrix();
	}
}

- (void)drawAllForSelection:(BOOL)forSelection
{
    if (forSelection)
    {
        [mesh drawAllForSelection:forSelection withMode:ViewModeSolidFlat];
    }
    else
    {
        if (viewMode == ViewModeMixedWireSolid)
        {
            glDisable(GL_DEPTH_TEST);
            [mesh drawAllForSelection:forSelection withMode:viewMode];
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            [mesh drawAllForSelection:forSelection withMode:viewMode];
        }
    }
}

- (void)flipSelected
{
    [mesh flipSelected];
}

- (void)flipAllTriangles
{
    [mesh flipAllTriangles];
}

- (void)loopSubdivision
{
    [mesh loopSubdivision];
}

- (void)detachSelected
{
    [mesh detachSelected];
}

- (void)extrudeSelected
{
    [mesh extrudeSelected];
}

- (void)duplicateSelected
{
    [mesh duplicateSelected];
}

- (void)removeSelected
{
    [mesh removeSelected];
}

- (void)hideSelected
{
    [mesh hideSelected];
}

- (void)unhideAll
{
    [mesh unhideAll];
}

- (void)cleanTexture
{
    [mesh cleanTexture];
}

- (NSColor *)selectionColor
{
    return [mesh color];
}

- (void)setSelectionColor:(NSColor *)selectionColor
{
    [mesh setColor:selectionColor];
}

- (enum ViewMode)viewMode
{
    return viewMode;
}

- (void)setViewMode:(enum ViewMode)aViewMode
{
    viewMode = aViewMode;
    if (viewMode == ViewModeUnwrap)
        mesh->mesh->setUnwrapped(true);
    else
        mesh->mesh->setUnwrapped(false);
}

@end
