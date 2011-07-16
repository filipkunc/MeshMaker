//
//  OpenGLManipulatingController.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 8/4/09.
//  For license see LICENSE.TXT
//

#import "OpenGLManipulatingController.h"
#import "Mesh.h"

@implementation OpenGLManipulatingController

@synthesize selectedCount, lastSelectedIndex;

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
}

- (id<OpenGLManipulatingModel>)model
{
    return model;
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

- (void)addTransformationObserver:(id)observer
{
	[self addObserver:observer forKeyPath:@"positionX"];
	[self addObserver:observer forKeyPath:@"positionY"];
	[self addObserver:observer forKeyPath:@"positionZ"];
	
	[self addObserver:observer forKeyPath:@"rotationX"];
	[self addObserver:observer forKeyPath:@"rotationY"];
	[self addObserver:observer forKeyPath:@"rotationZ"];
	
	[self addObserver:observer forKeyPath:@"scaleX"];
	[self addObserver:observer forKeyPath:@"scaleY"];
	[self addObserver:observer forKeyPath:@"scaleZ"];
}

- (void)removeTransformationObserver:(id)observer
{
	[self removeObserver:observer forKeyPath:@"positionX"];
	[self removeObserver:observer forKeyPath:@"positionY"];
	[self removeObserver:observer forKeyPath:@"positionZ"];
	
	[self removeObserver:observer forKeyPath:@"rotationX"];
	[self removeObserver:observer forKeyPath:@"rotationY"];
	[self removeObserver:observer forKeyPath:@"rotationZ"];
	
	[self removeObserver:observer forKeyPath:@"scaleX"];
	[self removeObserver:observer forKeyPath:@"scaleY"];
	[self removeObserver:observer forKeyPath:@"scaleZ"];
}

- (void)setPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale
{
	*modelPosition = aPosition;
	*modelRotation = aRotation;
	*modelScale = aScale;
	modelTransform->TranslateRotateScale(aPosition, aRotation, aScale);
}

- (float)positionX
{
	return [self transformValueAtIndex:0 withManipulator:ManipulatorTypeTranslation];
}

- (float)positionY
{
	return [self transformValueAtIndex:1 withManipulator:ManipulatorTypeTranslation];
}

- (float)positionZ
{
	return [self transformValueAtIndex:2 withManipulator:ManipulatorTypeTranslation];
}

- (float)rotationX
{
	return [self transformValueAtIndex:0 withManipulator:ManipulatorTypeRotation];
}

- (float)rotationY
{
	return [self transformValueAtIndex:1 withManipulator:ManipulatorTypeRotation];
}

- (float)rotationZ
{
	return [self transformValueAtIndex:2 withManipulator:ManipulatorTypeRotation];
}

- (float)scaleX
{
	return [self transformValueAtIndex:0 withManipulator:ManipulatorTypeScale];
}

- (float)scaleY
{
	return [self transformValueAtIndex:1 withManipulator:ManipulatorTypeScale];
}

- (float)scaleZ
{
	return [self transformValueAtIndex:2 withManipulator:ManipulatorTypeScale];
}

- (void)setPositionX:(float)value
{
	[self setTransformValue:value atIndex:0 withManipulator:ManipulatorTypeTranslation];
}

- (void)setPositionY:(float)value
{
	[self setTransformValue:value atIndex:1 withManipulator:ManipulatorTypeTranslation];
}

- (void)setPositionZ:(float)value
{
	[self setTransformValue:value atIndex:2 withManipulator:ManipulatorTypeTranslation];
}

- (void)setRotationX:(float)value
{
	[self setTransformValue:value atIndex:0 withManipulator:ManipulatorTypeRotation];
}

- (void)setRotationY:(float)value
{
	[self setTransformValue:value atIndex:1 withManipulator:ManipulatorTypeRotation];
}

- (void)setRotationZ:(float)value
{
	[self setTransformValue:value atIndex:2 withManipulator:ManipulatorTypeRotation];
}

- (void)setScaleX:(float)value
{
	[self setTransformValue:value atIndex:0 withManipulator:ManipulatorTypeScale];
}

- (void)setScaleY:(float)value
{
	[self setTransformValue:value atIndex:1 withManipulator:ManipulatorTypeScale];
}

- (void)setScaleZ:(float)value
{
	[self setTransformValue:value atIndex:2 withManipulator:ManipulatorTypeScale];
}

- (void)setNilValueForKey:(NSString *)key
{
	[self setValue:[NSNumber numberWithFloat:0.0f] forKey:key];
}

- (float)transformValueAtIndex:(uint)index
			   withManipulator:(enum ManipulatorType)manipulatorType
{
	switch (manipulatorType)
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

- (void)setTransformValue:(float)value 
				  atIndex:(uint)index
		  withManipulator:(enum ManipulatorType)manipulatorType
{
	switch (manipulatorType)
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

- (void)updateSelection
{
	[self willChangeTransformation];
	
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
	*selectionEuler = selectionRotation->ToEulerAngles();
    *selectionCenter = modelTransform->Transform(*selectionCenter);
	[self didChangeTransformation];
}

- (void)willChangeTransformation
{
	[self willChangeValueForKey:@"positionX"];
	[self willChangeValueForKey:@"positionY"];
	[self willChangeValueForKey:@"positionZ"];	
	
	[self willChangeValueForKey:@"rotationX"];
	[self willChangeValueForKey:@"rotationY"];
	[self willChangeValueForKey:@"rotationZ"];	
	
	[self willChangeValueForKey:@"scaleX"];
	[self willChangeValueForKey:@"scaleY"];
	[self willChangeValueForKey:@"scaleZ"];	
}

- (void)didChangeTransformation
{
	[self didChangeValueForKey:@"positionX"];
	[self didChangeValueForKey:@"positionY"];
	[self didChangeValueForKey:@"positionZ"];	
	
	[self didChangeValueForKey:@"rotationX"];
	[self didChangeValueForKey:@"rotationY"];
	[self didChangeValueForKey:@"rotationZ"];	
	
	[self didChangeValueForKey:@"scaleX"];
	[self didChangeValueForKey:@"scaleY"];
	[self didChangeValueForKey:@"scaleZ"];	
}

- (Vector3D)selectionCenter
{
	return *selectionCenter;
}

- (void)setSelectionCenter:(Vector3D)value
{
	[self willChangeTransformation];
	*selectionCenter = value;
	[self didChangeTransformation];
}

- (Quaternion)selectionRotation
{
	return *selectionRotation;
}

- (void)setSelectionRotation:(Quaternion)value
{
	[self willChangeTransformation];
	*selectionRotation = value;
	*selectionEuler = selectionRotation->ToEulerAngles();
	[self didChangeTransformation];
}

- (Vector3D)selectionScale
{
	return *selectionScale;
}

- (void)setSelectionScale:(Vector3D)value
{
	[self willChangeTransformation];
	*selectionScale = value;
	[self didChangeTransformation];
}

- (void)moveSelectedByOffset:(Vector3D)offset
{	
	Vector3D transformedOffset;
	Matrix4x4 m, r, s;
	Quaternion inverseRotation = modelRotation->Conjugate();
	
	Vector3D inverseScale = *modelScale;
	for (int i = 0; i < 3; i++)
		inverseScale[i] = 1.0f / inverseScale[i];
	
	r = inverseRotation.ToMatrix();
	s.Scale(inverseScale);
	m = s * r;
	transformedOffset = m.Transform(offset);
	
	if (modelMesh != nil)
	{
        Matrix4x4 translate;
        translate.Translate(transformedOffset);        
		[modelMesh transformSelectedByMatrix:&translate];
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
		rotationCenter = modelTransform->Inverse().Transform(rotationCenter);
        
        Matrix4x4 t1, t2, r;
        t1.Translate(-rotationCenter);
        t2.Translate(rotationCenter);
        r = offset.ToMatrix();
        
        Matrix4x4 m = t2 * r * t1;
        [modelMesh transformSelectedByMatrix:&m];

		[self setSelectionRotation:offset * (*selectionRotation)];
	}
	else if (modelItem != nil)
	{
		if ([self selectedCount] > 1)
		{
			Vector3D rotationCenter = *selectionCenter;
			rotationCenter = modelTransform->Inverse().Transform(rotationCenter);            
            Matrix4x4 offsetMatrix = offset.ToMatrix();
			for (uint i = 0; i < [model count]; i++)
			{
				if ([model isSelectedAtIndex:i])
				{
					Vector3D itemPosition = [modelItem positionAtIndex:i];
					itemPosition -= rotationCenter;
					itemPosition = offsetMatrix.Transform(itemPosition);
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
		rotationCenter = modelTransform->Inverse().Transform(rotationCenter);
        
        Matrix4x4 t1, t2, s;
        t1.Translate(-rotationCenter);
        t2.Translate(rotationCenter);
        s.Scale(offset + Vector3D(1, 1, 1));
        
        Matrix4x4 m = t2 * s * t1;
        [modelMesh transformSelectedByMatrix:&m];
	}
	else if (modelItem != nil)
	{
		if ([self selectedCount] > 1)
		{
			Vector3D rotationCenter = *selectionCenter;
			rotationCenter = modelTransform->Inverse().Transform(rotationCenter);
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

- (void)drawWithMode:(enum ViewMode)mode forSelection:(BOOL)forSelection
{
    if ([model isKindOfClass:[Mesh class]])
    {
        glPushMatrix();
        glMultMatrixf(modelTransform->m);
        [model drawAllForSelection:forSelection withMode:mode];
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glMultMatrixf(modelTransform->m);
        for (uint i = 0; i < [model count]; i++)
        {
            [model drawAtIndex:i forSelection:forSelection withMode:mode];
        }
        glPopMatrix();
    }
}

- (void)willSelect
{
	if ([model respondsToSelector:@selector(willSelect)])
		[model willSelect];
}

- (void)didSelect
{
	if ([model respondsToSelector:@selector(didSelect)])
		[model didSelect];
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

- (void)drawAllForSelection
{
    if ([model respondsToSelector:@selector(drawAllForSelection:withMode:)])
    {
        glPushMatrix();
        glMultMatrixf(modelTransform->m);
        [model drawAllForSelection:YES withMode:ViewModeSolid];
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glMultMatrixf(modelTransform->m);
        for (uint i = 0; i < [model count]; i++)
        {
            uint colorIndex = i + 1;
            glColor4ubv((GLubyte *)&colorIndex);
            [model drawAtIndex:i forSelection:YES withMode:ViewModeSolid];
        }
        glPopMatrix();        
    }
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

- (BOOL)isObjectSelectedAtIndex:(uint)index
{
	return [model isSelectedAtIndex:index];
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

- (void)duplicateSelected
{
	[model duplicateSelected];
	[self updateSelection];
}

- (void)removeSelected
{	
	[model removeSelected];
	[self updateSelection];
}

- (void)hideSelected
{
	[model hideSelected];
	[self updateSelection];
}

- (void)unhideAll
{
	[model unhideAll];
}

- (NSString *)nameAtIndex:(uint)index
{
	return [model nameAtIndex:index];
}

@end

