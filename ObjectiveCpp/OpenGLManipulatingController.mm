//
//  OpenGLManipulatingController.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 8/4/09.
//  For license see LICENSE.TXT
//

#import "OpenGLManipulatingController.h"


@implementation OpenGLManipulatingController

@synthesize currentManipulator, selectedCount, model, lastSelectedIndex;

- (id)init
{
	self = [super init];
	if (self)
	{
		selectionCenter = new Vector3D();
		selectionRotation = new Quaternion();
		selectionEuler = new Vector3D();
		selectionScale = new Vector3D(1, 1, 1);
		selectedCount = 0;
		lastSelectedIndex = -1;
		currentManipulator = ManipulatorTypeDefault;
		modelTransform = new Matrix4x4();
		modelPosition = new Vector3D();
		modelRotation = new Quaternion();
		modelScale = new Vector3D(1, 1, 1);
	}
	return self;
}

- (void)dealloc
{
	delete selectionCenter;
	delete selectionRotation;
	delete selectionEuler;
	delete selectionScale;
	delete modelTransform;
	delete modelPosition;
	delete modelRotation;
	delete modelScale;
	[super dealloc];
}

- (void)setModel:(id<OpenGLManipulatingModel>)value
{
	model = value;
	id newModel = value;
	if ([newModel conformsToProtocol:@protocol(OpenGLManipulatingModelMesh)])
		modelMesh = (id<OpenGLManipulatingModelMesh>)value;
	else 
		modelMesh = nil;
	if ([newModel conformsToProtocol:@protocol(OpenGLManipulatingModelItem)])
		modelItem = (id<OpenGLManipulatingModelItem>)value;
	else 
		modelItem = nil;
}

- (void)addObserver:(id)observer forKeyPath:(NSString *)keyPath
{
	[self addObserver:observer
		   forKeyPath:keyPath
			  options:NSKeyValueObservingOptionOld | NSKeyValueObservingOptionNew
			  context:NULL];
}

- (void)addSelectionObserver:(id)observer
{
	[self addObserver:observer forKeyPath:@"selectionX"];
	[self addObserver:observer forKeyPath:@"selectionY"];
	[self addObserver:observer forKeyPath:@"selectionZ"];
}

- (void)removeSelectionObserver:(id)observer
{
	[self removeObserver:observer forKeyPath:@"selectionX"];
	[self removeObserver:observer forKeyPath:@"selectionY"];
	[self removeObserver:observer forKeyPath:@"selectionZ"];
}

- (void)setPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale
{
	*modelPosition = aPosition;
	*modelRotation = aRotation;
	*modelScale = aScale;
	modelTransform->TranslateRotateScale(aPosition, aRotation, aScale);
}

- (float)selectionX
{
	return [self selectionValueAtIndex:0];
}

- (float)selectionY
{
	return [self selectionValueAtIndex:1];
}

- (float)selectionZ
{
	return [self selectionValueAtIndex:2];
}

- (void)setSelectionX:(float)value
{
	[self setSelectionValue:value atIndex:0];
}

- (void)setSelectionY:(float)value
{
	[self setSelectionValue:value atIndex:1];
}

- (void)setSelectionZ:(float)value
{
	[self setSelectionValue:value atIndex:2];
}

- (void)setNilValueForKey:(NSString *)key
{
	[self setValue:[NSNumber numberWithFloat:0.0f] forKey:key];
}

- (float)selectionValueAtIndex:(uint)index
{
	switch (currentManipulator)
	{
		case ManipulatorTypeTranslation:
			return (*selectionCenter)[index];
		case ManipulatorTypeRotation:
			return (*selectionEuler)[index] * RAD_TO_DEG;
		case ManipulatorTypeScale:
			return (*selectionScale)[index];
		case ManipulatorTypeDefault:
		default:
			return 0.0f;
	}
}

- (void)setSelectionValue:(float)value atIndex:(uint)index
{
	switch (currentManipulator)
	{
		case ManipulatorTypeTranslation:
		{
			Vector3D offset = Vector3D();
			offset[index] = value - (*selectionCenter)[index];
			[self moveSelectedByOffset:offset];
		}break;
		case ManipulatorTypeRotation:
		{
			(*selectionEuler)[index] = value * DEG_TO_RAD;
			if (selectedCount == 1)
			{
				selectionRotation->FromEulerAngles(*selectionEuler);
				if (lastSelectedIndex > -1 && modelItem != nil)
					[modelItem setRotation:*selectionRotation atIndex:lastSelectedIndex];
			}
			else
			{
				Quaternion offset = Quaternion();
				offset.FromEulerAngles(*selectionEuler);
				offset = offset * selectionRotation->Conjugate();
				[self rotateSelectedByOffset:offset];
			}
		}break;
		case ManipulatorTypeScale:
		{
			if (selectedCount == 1)
			{
				(*selectionScale)[index] = value;
				if (lastSelectedIndex > -1 && modelItem != nil)
					[modelItem setScale:*selectionScale atIndex:lastSelectedIndex];
			}
			else
			{
				Vector3D offset = Vector3D();
				offset[index] = value - (*selectionScale)[index];
				[self scaleSelectedByOffset:offset];
			}
		}break;
		default:
			break;
	}
}

- (void)setCurrentManipulator:(enum ManipulatorType)value
{
	[self willChangeSelection];
	currentManipulator = value;
	[self didChangeSelection];
}

- (void)updateSelection
{
	[self willChangeSelection];
	
	if (modelMesh != nil)
	{
		[modelMesh getSelectionCenter:selectionCenter rotation:selectionRotation scale:selectionScale];
		selectedCount = 0;
		for (uint i = 0; i < [model count]; i++)
		{
			if ([model isSelectedAtIndex:i])
				selectedCount++;
		}
	}
	else if (modelItem != nil)
	{
		*selectionCenter = Vector3D();
		*selectionRotation = Quaternion();
		*selectionScale = Vector3D(1, 1, 1);
		selectedCount = 0;
		lastSelectedIndex = -1;
		for (uint i = 0; i < [model count]; i++)
		{
			if ([model isSelectedAtIndex:i])
			{
				selectedCount++;
				*selectionCenter += [modelItem positionAtIndex:i];
				lastSelectedIndex = i;
			}
		}
		if (selectedCount > 0)
		{
			*selectionCenter /= (float)selectedCount;
			if (selectedCount == 1 && lastSelectedIndex > -1)
			{
				*selectionRotation = [modelItem rotationAtIndex:lastSelectedIndex];
				*selectionScale = [modelItem scaleAtIndex:lastSelectedIndex];
			}
		}
		else
		{
			*selectionCenter = Vector3D();
		}
	}
	selectionRotation->ToEulerAngles(*selectionEuler);
	selectionCenter->Transform(*modelTransform);
	[self didChangeSelection];
}

- (void)willChangeSelection
{
	[self willChangeValueForKey:@"selectionX"];
	[self willChangeValueForKey:@"selectionY"];
	[self willChangeValueForKey:@"selectionZ"];	
}

- (void)didChangeSelection
{
	[self didChangeValueForKey:@"selectionX"];
	[self didChangeValueForKey:@"selectionY"];
	[self didChangeValueForKey:@"selectionZ"];	
}

- (Vector3D)selectionCenter
{
	return *selectionCenter;
}

- (void)setSelectionCenter:(Vector3D)value
{
	[self willChangeSelection];
	*selectionCenter = value;
	[self didChangeSelection];
}

- (Quaternion)selectionRotation
{
	return *selectionRotation;
}

- (void)setSelectionRotation:(Quaternion)value
{
	[self willChangeSelection];
	*selectionRotation = value;
	selectionRotation->ToEulerAngles(*selectionEuler);
	[self didChangeSelection];
}

- (Vector3D)selectionScale
{
	return *selectionScale;
}

- (void)setSelectionScale:(Vector3D)value
{
	[self willChangeSelection];
	*selectionScale = value;
	[self didChangeSelection];
}

- (void)moveSelectedByOffset:(Vector3D)offset
{	
	Vector3D transformedOffset = offset;
	Matrix4x4 m, r, s;
	Quaternion inverseRotation = modelRotation->Conjugate();
	
	Vector3D inverseScale = *modelScale;
	for (int i = 0; i < 3; i++)
		inverseScale[i] = 1.0f / inverseScale[i];
	
	inverseRotation.ToMatrix(r);
	s.Scale(inverseScale);
	m = s * r;
	transformedOffset.Transform(m);
	
	if (modelMesh != nil)
	{
		[modelMesh moveSelectedByOffset:transformedOffset];
	}
	else if (modelItem != nil)
	{
		for (uint i = 0; i < [model count]; i++)
		{
			if ([model isSelectedAtIndex:i])
				[modelItem moveByOffset:transformedOffset atIndex:i];
		}
	}
		
	[self setSelectionCenter:*selectionCenter + offset];
}

- (void)rotateSelectedByOffset:(Quaternion)offset
{	
	if (modelMesh != nil)
	{
		Vector3D rotationCenter = *selectionCenter;
		rotationCenter.Transform(modelTransform->Inverse());
		[modelMesh moveSelectedByOffset:-rotationCenter];
		[modelMesh rotateSelectedByOffset:offset];
		[modelMesh moveSelectedByOffset:rotationCenter];
		[self setSelectionRotation:offset * (*selectionRotation)];
	}
	else if (modelItem != nil)
	{
		if ([self selectedCount] > 1)
		{
			Vector3D rotationCenter = *selectionCenter;
			rotationCenter.Transform(modelTransform->Inverse());
			for (uint i = 0; i < [model count]; i++)
			{
				if ([model isSelectedAtIndex:i])
				{
					Vector3D itemPosition = [modelItem positionAtIndex:i];
					itemPosition -= rotationCenter;
					itemPosition.Transform(offset);
					itemPosition += rotationCenter;
					[modelItem setPosition:itemPosition atIndex:i];
					[modelItem rotateByOffset:offset atIndex:i];
				}
			}
			[self setSelectionRotation:offset * (*selectionRotation)];
		}
		else if (lastSelectedIndex > -1)
		{
			[modelItem rotateByOffset:offset atIndex:lastSelectedIndex];
			[self setSelectionRotation:[modelItem rotationAtIndex:lastSelectedIndex]];
		}		
	}
}

- (void)scaleSelectedByOffset:(Vector3D)offset
{
	if (modelMesh != nil)
	{
		Vector3D rotationCenter = *selectionCenter;
		rotationCenter.Transform(modelTransform->Inverse());
		[modelMesh moveSelectedByOffset:-rotationCenter];
		[modelMesh scaleSelectedByOffset:offset + Vector3D(1, 1, 1)];
		[modelMesh moveSelectedByOffset:rotationCenter];
	}
	else if (modelItem != nil)
	{
		if ([self selectedCount] > 1)
		{
			Vector3D rotationCenter = *selectionCenter;
			rotationCenter.Transform(modelTransform->Inverse());
			for (uint i = 0; i < [model count]; i++)
			{
				if ([model isSelectedAtIndex:i])
				{
					Vector3D itemPosition = [modelItem positionAtIndex:i];
					itemPosition -= rotationCenter;
					itemPosition.x *= 1.0f + offset.x;
					itemPosition.y *= 1.0f + offset.y;
					itemPosition.z *= 1.0f + offset.z;
					itemPosition += rotationCenter;
					[modelItem setPosition:itemPosition atIndex:i];
					[modelItem scaleByOffset:offset atIndex:i];
				}
			}
		}
		else if (lastSelectedIndex > -1)
		{
			[modelItem scaleByOffset:offset atIndex:lastSelectedIndex];
		}
	}
	[self setSelectionScale:*selectionScale + offset];
}

- (void)drawWithMode:(enum ViewMode)mode
{
	glPushMatrix();
	glMultMatrixf(modelTransform->m);
	for (uint i = 0; i < [model count]; i++)
	{
		[model drawAtIndex:i forSelection:NO withMode:mode];
	}
	glPopMatrix();
}

- (void)willSelect
{
	id aModel = model;
	if ([aModel respondsToSelector:@selector(willSelect)])
	{
		[model willSelect];
	}
}

- (void)didSelect
{
	id aModel = model;
	if ([aModel respondsToSelector:@selector(didSelect)])
	{
		[model didSelect];
	}
	[self updateSelection];
}

- (uint)selectableCount
{
	return [model count];
}

- (void)drawForSelectionAtIndex:(uint)index
{
	glPushMatrix();
	glMultMatrixf(modelTransform->m);
	[model drawAtIndex:index forSelection:YES withMode:ViewModeSolid];
	glPopMatrix();
}

- (void)selectObjectAtIndex:(uint)index
				   withMode:(enum OpenGLSelectionMode)selectionMode
{
	switch (selectionMode) 
	{
		case OpenGLSelectionModeAdd:
			[model setSelected:YES atIndex:index];
			break;
		case OpenGLSelectionModeSubtract:
			[model setSelected:NO atIndex:index];
			break;
		case OpenGLSelectionModeInvert:
			[model setSelected:![model isSelectedAtIndex:index] atIndex:index];
			break;
		default:
			break;
	}
}

- (void)changeSelection:(BOOL)isSelected
{
	[self willSelect];
	for (uint i = 0; i < [model count]; i++)
		[model setSelected:isSelected atIndex:i];
	[self didSelect];
	[self updateSelection];
}

- (void)invertSelection
{
	[self willSelect];
	for (uint i = 0; i < [model count]; i++)
		[model setSelected:![model isSelectedAtIndex:i] atIndex:i];
	[self didSelect];
	[self updateSelection];
}

- (void)cloneSelected
{
	[model cloneSelected];
	[self updateSelection];
}

- (void)removeSelected
{	
	[model removeSelected];
	[self updateSelection];
}

@end

