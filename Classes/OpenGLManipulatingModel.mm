//
//  OpenGLManipulatingModel.mm
//  MeshMaker
//
//  Created by Filip Kunc on 1/3/13.
//
//

#import "OpenGLManipulatingModel.h"

@implementation OpenglManipulatingModelWrapper

- (enum ViewMode)viewMode
{
    return _cppModel->viewMode();
}

- (void)setViewMode:(enum ViewMode)viewMode
{
    _cppModel->setViewMode(viewMode);
}

- (uint)count
{
    return _cppModel->count();
}

- (BOOL)isSelectedAtIndex:(uint)index
{
    return _cppModel->isSelectedAtIndex(index);
}

- (void)setSelected:(BOOL)selected atIndex:(uint)index
{
    _cppModel->setSelectedAtIndex(index, selected);
}

- (void)expandSelectionFromIndex:(uint)index invert:(BOOL)invert
{
    _cppModel->expandSelectionFromIndex(index, invert);
}

- (void)duplicateSelected
{
    _cppModel->duplicateSelected();
}

- (void)removeSelected
{
    _cppModel->removeSelected();
}

- (void)hideSelected
{
    _cppModel->hideSelected();
}

- (void)unhideAll
{
    _cppModel->unhideAll();
}

@end

@implementation OpenglManipulatingModelMeshWrapper

- (IOpenGLManipulatingModelMesh *)meshModel
{
    return (IOpenGLManipulatingModelMesh *)self.cppModel;
}

- (void)setMeshModel:(IOpenGLManipulatingModelMesh *)meshModel
{
    self.cppModel = meshModel;
}

- (void)getSelectionCenter:(Vector3D *)center
				  rotation:(Quaternion *)rotation
					 scale:(Vector3D *)scale
{
    return self.meshModel->getSelectionCenterRotationScale(*center, *rotation, *scale);
}

- (void)transformSelectedByMatrix:(Matrix4x4 *)matrix
{
    return self.meshModel->transformSelectedByMatrix(*matrix);
}

- (void)drawAllForSelection:(BOOL)forSelection
{
    self.meshModel->drawAllForSelection(forSelection);
}

- (BOOL)useGLProject
{
    return self.meshModel->useGLProject();
}

- (void)glProjectSelectWithX:(int)x
                           y:(int)y
                       width:(int)width
                      height:(int)height
                   transform:(Matrix4x4 *)matrix
               selectionMode:(enum OpenGLSelectionMode)selectionMode
{
    self.meshModel->glProjectSelect(x, y, width, height, *matrix, selectionMode);
}

@end

@implementation OpenglManipulatingModelItemWrapper

- (IOpenGLManipulatingModelItem *)itemModel
{
    return (IOpenGLManipulatingModelItem *)self.cppModel;
}

- (void)setItemModel:(IOpenGLManipulatingModelItem *)itemModel
{
    self.cppModel = itemModel;
}

- (Vector3D)positionAtIndex:(uint)index
{
    return self.itemModel->positionAtIndex(index);
}

- (Quaternion)rotationAtIndex:(uint)index
{
    return self.itemModel->rotationAtIndex(index);
}

- (Vector3D)scaleAtIndex:(uint)index
{
    return self.itemModel->scaleAtIndex(index);
}

- (void)setPosition:(Vector3D)position atIndex:(uint)index
{
    self.itemModel->setPositionAtIndex(index, position);
}

- (void)setRotation:(Quaternion)rotation atIndex:(uint)index
{
    self.itemModel->setRotationAtIndex(index, rotation);
}

- (void)setScale:(Vector3D)scale atIndex:(uint)index
{
    self.itemModel->setScaleAtIndex(index, scale);
}

- (void)moveByOffset:(Vector3D)offset atIndex:(uint)index
{
    self.itemModel->moveByOffset(index, offset);
}

- (void)rotateByOffset:(Quaternion)offset atIndex:(uint)index
{
    self.itemModel->rotateByOffset(index, offset);
}

- (void)scaleByOffset:(Vector3D)offset atIndex:(uint)index
{
    self.itemModel->scaleByOffset(index, offset);
}

- (void)drawAtIndex:(uint)index forSelection:(BOOL)forSelection
{
    self.itemModel->drawAtIndex(index, forSelection);
}

@end
