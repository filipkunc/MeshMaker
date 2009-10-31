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
	enum ManipulatorType currentManipulator;
	
	Matrix4x4 *modelTransform;
	Vector3D *modelPosition;
	Quaternion *modelRotation;
	Vector3D *modelScale;
}

@property (readwrite, assign) id<OpenGLManipulatingModel> model;
@property (readonly, assign) NSInteger lastSelectedIndex;

- (void)addSelectionObserver:(id)observer;
- (void)removeSelectionObserver:(id)observer;
- (float)selectionValueAtIndex:(uint)index;
- (void)setSelectionValue:(float)value atIndex:(uint)index;
- (void)willChangeSelection;
- (void)didChangeSelection;
- (void)setPosition:(Vector3D)aPosition rotation:(Quaternion)aRotation scale:(Vector3D)aScale;

@end
