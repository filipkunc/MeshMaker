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
	id<OpenGLManipulatingModel> model;
	id<OpenGLManipulatingModelMesh> modelMesh;
	id<OpenGLManipulatingModelItem> modelItem;
	
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
}

@property (readwrite, assign) id<OpenGLManipulatingModel> model;
@property (readonly) NSInteger lastSelectedIndex;

- (void)addTransformationObserver:(id)observer;
- (void)removeTransformationObserver:(id)observer;
- (float)transformValueAtIndex:(uint)index
			   withManipulator:(enum ManipulatorType)manipulatorType;
- (void)setTransformValue:(float)value
				  atIndex:(uint)index
		  withManipulator:(enum ManipulatorType)manipulatorType;
- (void)willChangeTransformation;
- (void)didChangeTransformation;
- (void)setPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale;

@end
