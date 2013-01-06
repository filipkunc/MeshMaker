//
//  OpenGLManipulatingController.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 8/4/09.
//  For license see LICENSE.TXT
//

#import "OpenGLManipulatingController.h"

@implementation OpenGLManipulatingController

@synthesize selectedCount, lastSelectedIndex, modelTransform;

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
        currentManipulator = ManipulatorTypeDefault;
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

- (BOOL)selectionColorEnabled
{
    return YES;
    
//    if ([model respondsToSelector:@selector(selectionColor)])
//        return YES;
//    
//    return NO;
}

- (NSColor *)selectionColor
{
    if (self.selectionColorEnabled)
        return model->selectionColor();
    return nil;
}

- (void)setSelectionColor:(NSColor *)selectionColor
{
    if (self.selectionColorEnabled)
        model->setSelectionColor(selectionColor);
}

- (enum ViewMode)viewMode
{
    return model->viewMode();
}

- (void)setViewMode:(enum ViewMode)viewMode
{
    model->setViewMode(viewMode);
}

- (IOpenGLManipulatingModel *)model
{
    return model;
}

- (void)setModel:(IOpenGLManipulatingModel *)value
{
	model = value;
    modelMesh = dynamic_cast<IOpenGLManipulatingModelMesh *>(value);
    modelItem = dynamic_cast<IOpenGLManipulatingModelItem *>(value);
}

- (void)addObserver:(id)observer forKeyPath:(NSString *)keyPath
{
	[self addObserver:observer
		   forKeyPath:keyPath
			  options:NSKeyValueObservingOptionNew
			  context:NULL];
}

- (void)addSelectionObserver:(id)observer
{
	[self addObserver:observer forKeyPath:@"selectionX"];
	[self addObserver:observer forKeyPath:@"selectionY"];
	[self addObserver:observer forKeyPath:@"selectionZ"];
    [self addObserver:observer forKeyPath:@"selectionColor"];
    [self addObserver:observer forKeyPath:@"selectionColorEnabled"];
    [self addObserver:observer forKeyPath:@"viewMode"];
}

- (void)removeSelectionObserver:(id)observer
{
	[self removeObserver:observer forKeyPath:@"selectionX"];
	[self removeObserver:observer forKeyPath:@"selectionY"];
	[self removeObserver:observer forKeyPath:@"selectionZ"];
    [self removeObserver:observer forKeyPath:@"selectionColor"];
    [self removeObserver:observer forKeyPath:@"selectionColorEnabled"];
    [self removeObserver:observer forKeyPath:@"viewMode"];
}

- (void)setPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale
{
	*modelPosition = aPosition;
	*modelRotation = aRotation;
	*modelScale = aScale;
	modelTransform->TranslateRotateScale(aPosition, aRotation, aScale);
}

- (enum ManipulatorType)currentManipulator
{
    return currentManipulator;
}

- (void)setCurrentManipulator:(enum ManipulatorType)value
{
    [self willChangeSelection];
    currentManipulator = value;
    [self didChangeSelection];
}

- (float)selectionX
{
	return [self transformValueAtIndex:0 withManipulator:currentManipulator];
}

- (float)selectionY
{
	return [self transformValueAtIndex:1 withManipulator:currentManipulator];
}

- (float)selectionZ
{
	return [self transformValueAtIndex:2 withManipulator:currentManipulator];
}

- (void)setSelectionX:(float)value
{
	[self setTransformValue:value atIndex:0 withManipulator:currentManipulator];
}

- (void)setSelectionY:(float)value
{
	[self setTransformValue:value atIndex:1 withManipulator:currentManipulator];
}

- (void)setSelectionZ:(float)value
{
	[self setTransformValue:value atIndex:2 withManipulator:currentManipulator];
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
				if (lastSelectedIndex > -1 && modelItem != NULL)
                    modelItem->setRotationAtIndex(lastSelectedIndex, *selectionRotation);
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
				if (lastSelectedIndex > -1 && modelItem != NULL)
                    modelItem->setScaleAtIndex(lastSelectedIndex, *selectionScale);
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
	[self willChangeSelection];
	
	if (modelMesh != NULL)
	{
        modelMesh->getSelectionCenterRotationScale(*selectionCenter, *selectionRotation, *selectionScale);
		selectedCount = 0;
		for (uint i = 0; i < model->count(); i++)
		{
            if (model->isSelectedAtIndex(i))
				selectedCount++;
		}
	}
	else if (modelItem != NULL)
	{
		*selectionCenter = Vector3D();
		*selectionRotation = Quaternion();
		*selectionScale = Vector3D(1, 1, 1);
		selectedCount = 0;
		lastSelectedIndex = -1;
		for (uint i = 0; i < model->count(); i++)
		{
            if (model->isSelectedAtIndex(i))
			{
				selectedCount++;
				*selectionCenter += modelItem->positionAtIndex(i);
				lastSelectedIndex = i;
			}
		}
		if (selectedCount > 0)
		{
			*selectionCenter /= (float)selectedCount;
			if (selectedCount == 1 && lastSelectedIndex > -1)
			{
				*selectionRotation = modelItem->rotationAtIndex(lastSelectedIndex);
				*selectionScale = modelItem->scaleAtIndex(lastSelectedIndex);
			}
		}
		else
		{
			*selectionCenter = Vector3D();
		}
	}
	*selectionEuler = selectionRotation->ToEulerAngles();
    *selectionCenter = modelTransform->Transform(*selectionCenter);
	[self didChangeSelection];
}

- (void)willChangeSelection
{
	[self willChangeValueForKey:@"selectionX"];
	[self willChangeValueForKey:@"selectionY"];
	[self willChangeValueForKey:@"selectionZ"];	
    [self willChangeValueForKey:@"selectionColor"];	
    [self willChangeValueForKey:@"selectionColorEnabled"];
    [self willChangeValueForKey:@"viewMode"];
}

- (void)didChangeSelection
{
	[self didChangeValueForKey:@"selectionX"];
	[self didChangeValueForKey:@"selectionY"];
	[self didChangeValueForKey:@"selectionZ"];
    [self didChangeValueForKey:@"selectionColor"];	
    [self didChangeValueForKey:@"selectionColorEnabled"];
    [self didChangeValueForKey:@"viewMode"];
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
	*selectionEuler = selectionRotation->ToEulerAngles();
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
	
	if (modelMesh != NULL)
	{
        Matrix4x4 translate;
        translate.Translate(transformedOffset);
        modelMesh->transformSelectedByMatrix(translate);
	}
	else if (modelItem != NULL)
	{
		for (uint i = 0; i < model->count(); i++)
		{
            if (model->isSelectedAtIndex(i))
                modelItem->moveByOffset(i, transformedOffset);
		}
	}
		
	[self setSelectionCenter:*selectionCenter + offset];
}

- (void)rotateSelectedByOffset:(Quaternion)offset
{	
	if (modelMesh != NULL)
	{
		Vector3D rotationCenter = *selectionCenter;
		rotationCenter = modelTransform->Inverse().Transform(rotationCenter);
        
        Matrix4x4 t1, t2, r;
        t1.Translate(-rotationCenter);
        t2.Translate(rotationCenter);
        r = offset.ToMatrix();
        
        Matrix4x4 m = t2 * r * t1;
        modelMesh->transformSelectedByMatrix(m);

		[self setSelectionRotation:offset * (*selectionRotation)];
	}
	else if (modelItem != NULL)
	{
		if ([self selectedCount] > 1)
		{
			Vector3D rotationCenter = *selectionCenter;
			rotationCenter = modelTransform->Inverse().Transform(rotationCenter);            
            Matrix4x4 offsetMatrix = offset.ToMatrix();
			for (uint i = 0; i < model->count(); i++)
			{
				if (model->isSelectedAtIndex(i))
				{
					Vector3D itemPosition = modelItem->positionAtIndex(i);
					itemPosition -= rotationCenter;
					itemPosition = offsetMatrix.Transform(itemPosition);
					itemPosition += rotationCenter;
                    modelItem->setPositionAtIndex(i, itemPosition);
                    modelItem->rotateByOffset(i, offset);
				}
			}
			[self setSelectionRotation:offset * (*selectionRotation)];
		}
		else if (lastSelectedIndex > -1)
		{
            modelItem->rotateByOffset(lastSelectedIndex, offset);
            [self setSelectionRotation:modelItem->rotationAtIndex(lastSelectedIndex)];
		}		
	}
}

- (void)scaleSelectedByOffset:(Vector3D)offset
{
	if (modelMesh != NULL)
	{
        Vector3D rotationCenter = *selectionCenter;
		rotationCenter = modelTransform->Inverse().Transform(rotationCenter);
        
        Matrix4x4 t1, t2, s;
        t1.Translate(-rotationCenter);
        t2.Translate(rotationCenter);
        s.Scale(offset + Vector3D(1, 1, 1));
        
        Matrix4x4 m = t2 * s * t1;
        modelMesh->transformSelectedByMatrix(m);
	}
	else if (modelItem != NULL)
	{
		if ([self selectedCount] > 1)
		{
			Vector3D rotationCenter = *selectionCenter;
			rotationCenter = modelTransform->Inverse().Transform(rotationCenter);
			for (uint i = 0; i < model->count(); i++)
			{
                if (model->isSelectedAtIndex(i))
				{
					Vector3D itemPosition = modelItem->positionAtIndex(i);
					itemPosition -= rotationCenter;
					itemPosition.x *= 1.0f + offset.x;
					itemPosition.y *= 1.0f + offset.y;
					itemPosition.z *= 1.0f + offset.z;
					itemPosition += rotationCenter;
                    modelItem->setPositionAtIndex(i, itemPosition);
                    modelItem->scaleByOffset(i, offset);
				}
			}
		}
		else if (lastSelectedIndex > -1)
		{
            modelItem->scaleByOffset(lastSelectedIndex, offset);
		}
	}
	[self setSelectionScale:*selectionScale + offset];
}

- (void)drawForSelection:(BOOL)forSelection
{
    if (modelMesh != nil)
    {
        glPushMatrix();
        glMultMatrixf(modelTransform->m);
        modelMesh->drawAllForSelection(forSelection);
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glMultMatrixf(modelTransform->m);
        for (uint i = 0; i < modelItem->count(); i++)
        {
            modelItem->drawAtIndex(i, forSelection);
        }
        glPopMatrix();
    }
}

- (void)willSelectThrough:(BOOL)selectThrough
{
    model->willSelectThrough(selectThrough);
}

- (BOOL)needsCullFace
{
    return model->needsCullFace();
}

- (void)didSelect
{
    model->didSelect();
	[self updateSelection];
}

- (uint)selectableCount
{
	return model->count();
}

- (void)drawForSelectionAtIndex:(uint)index
{
	glPushMatrix();
	glMultMatrixf(modelTransform->m);
    modelItem->drawAtIndex(index, true);
	glPopMatrix();
}

- (void)drawAllForSelection
{
    if (modelMesh != NULL)
    {
        glPushMatrix();
        glMultMatrixf(modelTransform->m);
        modelMesh->drawAllForSelection(true);
        glPopMatrix();
    }
    else
    {
        glPushMatrix();
        glMultMatrixf(modelTransform->m);
        for (uint i = 0; i < modelItem->count(); i++)
        {
            uint colorIndex = i + 1;
            glColor4ubv((GLubyte *)&colorIndex);
            modelItem->drawAtIndex(i, true);
        }
        glPopMatrix();        
    }
}

- (BOOL)useGLProject
{
    if (modelMesh != NULL)
        return modelMesh->useGLProject();
    return NO;
}

- (void)glProjectSelectWithX:(int)x 
                           y:(int)y
                       width:(int)width 
                      height:(int)height 
               selectionMode:(enum OpenGLSelectionMode)selectionMode
{
    if (modelMesh != NULL)
        modelMesh->glProjectSelect(x, y, width, height, *modelTransform, selectionMode);
}

- (void)selectObjectAtIndex:(uint)index
				   withMode:(enum OpenGLSelectionMode)selectionMode
{
	switch (selectionMode) 
	{
		case OpenGLSelectionModeAdd:
            model->setSelectedAtIndex(index, true);
			break;
		case OpenGLSelectionModeSubtract:
            model->setSelectedAtIndex(index, false);
			break;
		case OpenGLSelectionModeInvert:
            model->setSelectedAtIndex(index, !model->isSelectedAtIndex(index));
			break;
        case OpenGLSelectionModeExpand:
            model->expandSelectionFromIndex(index, false);
            break;
        case OpenGLSelectionModeInvertExpand:
            model->expandSelectionFromIndex(index, true);
            break;
		default:
			break;
	}
}

- (BOOL)isObjectSelectedAtIndex:(uint)index
{
    return model->isSelectedAtIndex(index);
}

- (void)changeSelection:(BOOL)isSelected
{
	[self willSelectThrough:NO];
	for (uint i = 0; i < model->count(); i++)
        model->setSelectedAtIndex(i, isSelected);
	[self didSelect];
	[self updateSelection];
}

- (void)invertSelection
{
	[self willSelectThrough:NO];
    for (uint i = 0; i < model->count(); i++)
        model->setSelectedAtIndex(i, !model->isSelectedAtIndex(i));
	[self didSelect];
	[self updateSelection];
}

- (void)duplicateSelected
{
    model->duplicateSelected();
	[self updateSelection];
}

- (void)removeSelected
{
    model->removeSelected();
	[self updateSelection];
}

- (void)hideSelected
{
    model->hideSelected();
	[self updateSelection];
}

- (void)unhideAll
{
    model->unhideAll();
}

@end

