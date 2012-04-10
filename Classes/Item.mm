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

- (void)drawWithMode:(enum ViewMode)mode forSelection:(BOOL)forSelection
{
	if (visible)
	{
		glPushMatrix();
		glTranslatef(position->x, position->y, position->z);
		Matrix4x4 rotationMatrix = rotation->ToMatrix();
		glMultMatrixf(rotationMatrix);
		[mesh drawWithMode:mode scale:*scale selected:selected forSelection:forSelection];
		glPopMatrix();
	}
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
	
	[[newItem mesh] mergeWithMesh:[self mesh]];
	
	[newItem setSelected:[self selected]];
	
	return newItem;
}

@end
