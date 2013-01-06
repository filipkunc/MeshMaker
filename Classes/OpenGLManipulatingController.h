//
//  OpenGLManipulatingController.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 8/4/09.
//  For license see LICENSE.TXT
//

#import "OpenGLManipulatingModel.h"

@interface OpenGLManipulatingController : NSObject <OpenGLManipulating>
{
	IOpenGLManipulatingModel *model;
	IOpenGLManipulatingModelMesh *modelMesh;
	IOpenGLManipulatingModelItem *modelItem;
	
	Vector3D *selectionCenter;
	Quaternion *selectionRotation;
	Vector3D *selectionEuler;
	Vector3D *selectionScale;
	uint selectedCount;
	NSInteger lastSelectedIndex;
	
	Matrix4x4 *modelTransform;
	Vector3D *modelPosition;
	Quaternion *modelRotation;
	Vector3D *modelScale;
    
    enum ManipulatorType currentManipulator;
}

@property (readwrite, assign) IOpenGLManipulatingModel *model;
@property (readonly, assign) Matrix4x4 *modelTransform;
@property (readonly) NSInteger lastSelectedIndex;

- (void)addSelectionObserver:(id)observer;
- (void)removeSelectionObserver:(id)observer;
- (float)transformValueAtIndex:(uint)index
			   withManipulator:(enum ManipulatorType)manipulatorType;
- (void)setTransformValue:(float)value
				  atIndex:(uint)index
		  withManipulator:(enum ManipulatorType)manipulatorType;
- (void)willChangeSelection;
- (void)didChangeSelection;
- (void)setPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale;

@end
