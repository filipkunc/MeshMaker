//
//  JSWrappers.mm
//  MeshMaker
//
//  Created by Filip Kunc on 10/27/12.
//
//

#include "JSWrappers.h"

#if defined(__APPLE__)

@implementation ItemCollectionWrapper

+ (NSString *)webScriptNameForSelector:(SEL)sel
{
    if (sel == @selector(at:))
        return @"at";
    return nil;
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector { return NO; }
+ (BOOL)isKeyExcludedFromWebScript:(const char *)name { return NO; }

- (id)initWithItemCollection:(ItemCollection *)itemCollection
{
    self = [super init];
    if (self)
    {
        _itemCollection = itemCollection;
    }
    return self;
}

- (uint)count
{
    return _itemCollection->count();
}

- (ItemWrapper *)at:(uint)index
{
    return [[ItemWrapper alloc] initWithItem:_itemCollection->itemAtIndex(index)];
}

@end

@implementation ItemWrapper

- (id)initWithItem:(Item *)item
{
    self = [super init];
    if (self)
    {
        _item = item;
    }
    return self;
}

- (VertexNodeIterator *)vertexIterator
{
    return [[VertexNodeIterator alloc] initWithBegin:_item->mesh->vertices().begin() end:_item->mesh->vertices().end()];
}

- (TriangleNodeIterator *)triQuadIterator
{
    return [[TriangleNodeIterator alloc] initWithBegin:_item->mesh->triangles().begin() end:_item->mesh->triangles().end()];
}

- (EdgeNodeIterator *)edgeIterator
{
    return [[EdgeNodeIterator alloc] initWithBegin:_item->mesh->vertexEdges().begin() end:_item->mesh->vertexEdges().end()];
}

- (uint)vertexCount { return _item->mesh->vertexCount(); }
- (uint)triQuadCount { return _item->mesh->triangleCount(); }
- (BOOL)selected { return _item->selected; }
- (void)setSelected:(BOOL)value { _item->selected = value; }

- (void)removeDegeneratedTriangles { _item->mesh->removeDegeneratedTriangles(); }
- (void)removeNonUsedVertices { _item->mesh->removeNonUsedVertices(); }
- (void)removeNonUsedTexCoords { _item->mesh->removeNonUsedTexCoords(); }
- (void)mergeSelected { _item->mesh->mergeSelected(); }
- (void)splitSelected { _item->mesh->splitSelected(); }
- (void)detachSelected { _item->mesh->detachSelected(); }
- (void)duplicateSelectedTriangles { _item->mesh->duplicateSelectedTriangles(); }
- (void)flipSelected { _item->mesh->flipSelected(); }
- (void)flipAllTriangles { _item->mesh->flipAllTriangles(); }
- (void)extrudeSelectedTriangles { _item->mesh->extrudeSelectedTriangles(); }
- (void)triangulate { _item->mesh->triangulate(); }
- (void)triangulateSelectedQuads { _item->mesh->triangulateSelectedQuads(); }
- (void)openSubdivision { _item->mesh->openSubdivision(); }
- (void)loopSubdivision { _item->mesh->loopSubdivision(); }
- (void)makeTexCoords { _item->mesh->makeTexCoords(); }
- (void)makeEdges { _item->mesh->makeEdges(); }
- (void)updateSelection { _item->mesh->setSelectionMode(_item->mesh->selectionMode()); }
- (void)setSelectionModeVertices { _item->mesh->setSelectionMode(MeshSelectionMode::Vertices); }
- (void)setSelectionModeTriQuads { _item->mesh->setSelectionMode(MeshSelectionMode::Triangles); }
- (void)setSelectionModeEdges { _item->mesh->setSelectionMode(MeshSelectionMode::Edges); }
- (void)removeSelected { _item->removeSelected(); }

- (VertexWrapper *)addVertexWithX:(float)x y:(float)y z:(float)z
{
    return [[VertexWrapper alloc] initWithNode:_item->mesh->addVertex(Vector3D(x, y, z))];
}

- (TriangleWrapper *)addTriangleWithFirst:(VertexWrapper *)v0 second:(VertexWrapper *)v1 third:(VertexWrapper *)v2
{
    return [[TriangleWrapper alloc] initWithNode:_item->mesh->addTriangle(v0.node, v1.node, v2.node)];
}

- (TriangleWrapper *)addQuadWithFirst:(VertexWrapper *)v0 second:(VertexWrapper *)v1 third:(VertexWrapper *)v2 fourth:(VertexWrapper *)v3
{
    return [[TriangleWrapper alloc] initWithNode:_item->mesh->addQuad(v0.node, v1.node, v2.node, v3.node)];
}

- (void)removeTriQuad:(TriangleWrapper *)triQuad
{
    TriangleNode *current = triQuad.node;
    _item->mesh->removeTriQuad(current);
    triQuad.node = current;
}

+ (NSString *)webScriptNameForSelector:(SEL)sel
{
    if (sel == @selector(addVertexWithX:y:z:))
        return @"addVertex";
    if (sel == @selector(addTriangleWithFirst:second:third:))
        return @"addTriangle";
    if (sel == @selector(addQuadWithFirst:second:third:fourth:))
        return @"addQuad";
    if (sel == @selector(removeTriQuad:))
        return @"removeTriQuad";
    
    return nil;
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector { return NO; }
+ (BOOL)isKeyExcludedFromWebScript:(const char *)name { return NO; }

@end

@implementation VertexWrapper

@synthesize node;

- (BOOL)selected { return node->data().selected; }
- (void)setSelected:(BOOL)selected { node->data().selected = selected; }
- (float)x { return node->data().position.x; }
- (void)setX:(float)x { node->data().position.x = x; }
- (float)y { return node->data().position.y; }
- (void)setY:(float)y { node->data().position.y = y; }
- (float)z { return node->data().position.z; }
- (void)setZ:(float)z { node->data().position.z = z; }
- (uint)index { return node->algorithmData.index; }
- (void)setIndex:(uint)index { node->algorithmData.index = index; }
- (uint)edgeCount { return node->_edges.count(); }
- (VertexNodeEdgeIterator *)edgeIterator { return [[VertexNodeEdgeIterator alloc] initWithBegin:node->_edges.begin() end:node->_edges.end()]; }

- (id)initWithNode:(VertexNode *)vertexNode
{
    self = [super init];
    if (self)
    {
        node = vertexNode;
    }
    return self;
}

+ (NSString *)webScriptNameForSelector:(SEL)sel
{
    if (sel == @selector(setSelected:))
        return @"setSelected";
    if (sel == @selector(setX:))
        return @"setX";
    if (sel == @selector(setY:))
        return @"setY";
    if (sel == @selector(setZ:))
        return @"setZ";
    if (sel == @selector(setIndex:))
        return @"setIndex";
    
    return nil;
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector { return NO; }
+ (BOOL)isKeyExcludedFromWebScript:(const char *)name { return NO; }

@end

@implementation TriangleWrapper

@synthesize node;

- (BOOL)isQuad { return node->data().isQuad(); }
- (uint)count { return node->data().count(); }
- (BOOL)selected { return node->data().selected; }
- (void)setSelected:(BOOL)selected { node->data().selected = selected; }

- (id)initWithNode:(TriangleNode *)triangleNode
{
    self = [super init];
    if (self)
    {
        node = triangleNode;
    }
    return self;
}

- (VertexWrapper *)vertexAtIndex:(uint)index
{
    return [[VertexWrapper alloc] initWithNode:node->data().vertex(index)];
}

- (void)setVertex:(VertexWrapper *)vertex atIndex:(uint)index
{
    node->data().setVertex(index, vertex.node);
}

- (EdgeWrapper *)edgeAtIndex:(uint)index
{
    return [[EdgeWrapper alloc] initWithNode:node->data().vertexEdge(index)];
}

- (void)setEdge:(EdgeWrapper *)edge atIndex:(uint)index
{
    node->data().setVertexEdge(index, edge.node);
}

- (VertexWrapper *)vertexNotInEdge:(EdgeWrapper *)edge
{
    return [[VertexWrapper alloc] initWithNode:node->data().vertexNotInEdge(&edge.node->data())];
}

+ (NSString *)webScriptNameForSelector:(SEL)sel
{
    if (sel == @selector(setSelected:))
        return @"setSelected";
    if (sel == @selector(vertexAtIndex:))
        return @"vertex";
    if (sel == @selector(setVertex:atIndex:))
        return @"setVertex";
    if (sel == @selector(edgeAtIndex:))
        return @"edge";
    if (sel == @selector(setEdge:atIndex:))
        return @"setEdge";
    if (sel == @selector(vertexNotInEdge:))
        return @"vertexNotInEdge";
    
    return nil;
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector { return NO; }
+ (BOOL)isKeyExcludedFromWebScript:(const char *)name { return NO; }

@end

@implementation EdgeWrapper

@synthesize node;

- (BOOL)selected { return node->data().selected; }
- (void)setSelected:(BOOL)selected { node->data().selected = selected; }

- (VertexWrapper *)half
{
    if (node->data().half)
        return [[VertexWrapper alloc] initWithNode:node->data().half];
    return nil;
}

- (void)setHalf:(VertexWrapper *)half
{
    if (half)
        node->data().half = half.node;
    else
        node->data().half = NULL;
}

- (id)initWithNode:(VertexEdgeNode *)edgeNode
{
    self = [super init];
    if (self)
    {
        node = edgeNode;
    }
    return self;
}

- (VertexWrapper *)vertexAtIndex:(uint)index
{
    VertexNode *vertex = node->data().vertex(index);
    if (vertex)
        return [[VertexWrapper alloc] initWithNode:vertex];
    return nil;
}

- (void)setVertex:(VertexWrapper *)vertex atIndex:(uint)index
{
    if (vertex)
        node->data().setVertex(index, vertex.node);
    else
        node->data().setVertex(index, NULL);
}

- (TriangleWrapper *)triangleAtIndex:(uint)index
{
    TriangleNode *triangle = node->data().triangle(index);
    if (triangle)
        return [[TriangleWrapper alloc] initWithNode:triangle];
    return nil;
}

- (void)setTriangle:(TriangleWrapper *)triangle atIndex:(uint)index
{
    if (triangle)
        node->data().setTriangle(index, triangle.node);
    else
        node->data().setTriangle(index, NULL);
}

- (VertexWrapper *)oppositeVertex:(VertexWrapper *)vertex
{
    return [[VertexWrapper alloc] initWithNode:node->data().opposite(vertex.node)];
}

+ (NSString *)webScriptNameForSelector:(SEL)sel
{
    if (sel == @selector(setSelected:))
        return @"setSelected";
    if (sel == @selector(vertexAtIndex:))
        return @"vertex";
    if (sel == @selector(setVertex:atIndex:))
        return @"setVertex";
    if (sel == @selector(triangleAtIndex:))
        return @"triangle";
    if (sel == @selector(setTriangle:atIndex:))
        return @"setTriangle";
    if (sel == @selector(setHalf:))
        return @"setHalf";
    if (sel == @selector(oppositeVertex:))
        return @"oppositeVertex";
    
    return nil;
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector { return NO; }
+ (BOOL)isKeyExcludedFromWebScript:(const char *)name { return NO; }

@end

@implementation VertexNodeEdgeWrapper

- (Vertex2VEdgeNode *)simpleNode
{
    return _simpleNode;
}

- (void)setSimpleNode:(Vertex2VEdgeNode *)simpleNode
{
    _simpleNode = simpleNode;
    self.node = _simpleNode->data();
}

- (id)initWithSimpleNode:(Vertex2VEdgeNode *)edgeNode
{
    self = [self initWithNode:edgeNode->data()];
    if (self)
    {
        _simpleNode = edgeNode;
    }
    return self;
}

@end

#define ImplementIterator(Name, TNode, nodeProperty) \
@implementation Name \
\
- (BOOL)finished { return self.nodeProperty == end; } \
\
- (id)initWithBegin:(TNode *)theBegin end:(TNode *)theEnd \
{ \
    self = [super init]; \
    if (self) \
    { \
        begin = theBegin; \
        end = theEnd; \
        self.nodeProperty = begin; \
    } \
    return self; \
} \
\
- (void)moveStart { self.nodeProperty = begin; } \
- (void)moveNext { self.nodeProperty = self.nodeProperty->next(); } \
\
+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector \
{ \
    return NO; \
} \
+ (BOOL)isKeyExcludedFromWebScript:(const char *)name { return NO; } \
\
@end

ImplementIterator(VertexNodeIterator, VertexNode, node)
ImplementIterator(TriangleNodeIterator, TriangleNode, node)
ImplementIterator(EdgeNodeIterator, VertexEdgeNode, node)
ImplementIterator(VertexNodeEdgeIterator, Vertex2VEdgeNode, simpleNode)

#elif defined(WIN32)

namespace MeshMakerCppCLI
{

ItemCollectionWrapper::ItemCollectionWrapper(ItemCollection *itemCollection)
{
	_itemCollection = itemCollection;
}

int ItemCollectionWrapper::count()
{
    return _itemCollection->count();
}

ItemWrapper ^ItemCollectionWrapper::at(int index)
{
	return gcnew ItemWrapper(_itemCollection->itemAtIndex(index));
}

ItemWrapper::ItemWrapper(Item *item)
{
	_item = item;
}

VertexNodeIterator ^ItemWrapper::vertexIterator()
{
	return gcnew VertexNodeIterator(_item->mesh->vertices().begin(), _item->mesh->vertices().end());
}

TriangleNodeIterator ^ItemWrapper::triQuadIterator()
{
	return gcnew TriangleNodeIterator(_item->mesh->triangles().begin(), _item->mesh->triangles().end());
}

EdgeNodeIterator ^ItemWrapper::edgeIterator()
{
	return gcnew EdgeNodeIterator(_item->mesh->vertexEdges().begin(), _item->mesh->vertexEdges().end());
}

int ItemWrapper::vertexCount() { return _item->mesh->vertexCount(); }
int ItemWrapper::triQuadCount() { return _item->mesh->triangleCount(); }
bool ItemWrapper::selected() { return _item->selected; }
void ItemWrapper::setSelected(bool selected) { _item->selected = selected; }

void ItemWrapper::removeDegeneratedTriangles() { _item->mesh->removeDegeneratedTriangles(); }
void ItemWrapper::removeNonUsedVertices() { _item->mesh->removeNonUsedVertices(); }
void ItemWrapper::removeNonUsedTexCoords() { _item->mesh->removeNonUsedTexCoords(); }
void ItemWrapper::mergeSelected() { _item->mesh->mergeSelected(); }
void ItemWrapper::splitSelected() { _item->mesh->splitSelected(); }
void ItemWrapper::detachSelected() { _item->mesh->detachSelected(); }
void ItemWrapper::duplicateSelectedTriangles() { _item->mesh->duplicateSelectedTriangles(); }
void ItemWrapper::flipSelected() { _item->mesh->flipSelected(); }
void ItemWrapper::flipAllTriangles() { _item->mesh->flipAllTriangles(); }
void ItemWrapper::extrudeSelectedTriangles() { _item->mesh->extrudeSelectedTriangles(); }
void ItemWrapper::triangulate() { _item->mesh->triangulate(); }
void ItemWrapper::triangulateSelectedQuads() { _item->mesh->triangulateSelectedQuads(); }
void ItemWrapper::openSubdivision() { _item->mesh->openSubdivision(); }
void ItemWrapper::loopSubdivision() { _item->mesh->loopSubdivision(); }
void ItemWrapper::makeTexCoords() { _item->mesh->makeTexCoords(); }
void ItemWrapper::makeEdges() { _item->mesh->makeEdges(); }
void ItemWrapper::updateSelection() { _item->mesh->setSelectionMode(_item->mesh->selectionMode()); }
void ItemWrapper::setSelectionModeVertices() { _item->mesh->setSelectionMode(MeshSelectionMode::Vertices); }
void ItemWrapper::setSelectionModeTriQuads() { _item->mesh->setSelectionMode(MeshSelectionMode::Triangles); }
void ItemWrapper::setSelectionModeEdges() { _item->mesh->setSelectionMode(MeshSelectionMode::Edges); }
void ItemWrapper::removeSelected() { _item->removeSelected(); }

VertexWrapper ^ItemWrapper::addVertex(float x, float y, float z)
{
	return gcnew VertexWrapper(_item->mesh->addVertex(Vector3D(x, y, z)));
}

TriangleWrapper ^ItemWrapper::addTriangle(VertexWrapper ^v0, VertexWrapper ^v1, VertexWrapper ^v2)
{
	return gcnew TriangleWrapper(_item->mesh->addTriangle(v0->node(), v1->node(), v2->node()));
}

TriangleWrapper ^ItemWrapper::addQuad(VertexWrapper ^v0, VertexWrapper ^v1, VertexWrapper ^v2, VertexWrapper ^v3)
{
	return gcnew TriangleWrapper(_item->mesh->addQuad(v0->node(), v1->node(), v2->node(), v3->node()));
}

void ItemWrapper::removeTriQuad(TriangleWrapper ^triQuad)
{
    TriangleNode *current = triQuad->node();
    _item->mesh->removeTriQuad(current);
	triQuad->setNode(current);
}

VertexWrapper::VertexWrapper(VertexNode *node)
{
	_node = node;
}

VertexNode *VertexWrapper::node() {	return _node; }
void VertexWrapper::setNode(VertexNode *value) { _node = value; }
bool VertexWrapper::selected() { return _node->data().selected; }
void VertexWrapper::setSelected(bool selected) { _node->data().selected = selected; }
float VertexWrapper::x() { return _node->data().position.x; }
void VertexWrapper::setX(float x) { _node->data().position.x = x; }
float VertexWrapper::y() { return _node->data().position.y; }
void VertexWrapper::setY(float y) { _node->data().position.y = y; }
float VertexWrapper::z() { return _node->data().position.z; }
void VertexWrapper::setZ(float z) { _node->data().position.z = z; }
int VertexWrapper::index() { return _node->algorithmData.index; }
void VertexWrapper::setIndex(int index) { _node->algorithmData.index = index; }
int VertexWrapper::edgeCount() { return _node->_edges.count(); }
VertexNodeEdgeIterator ^VertexWrapper::edgeIterator() { return gcnew VertexNodeEdgeIterator(_node->_edges.begin(), _node->_edges.end()); }

TriangleWrapper::TriangleWrapper(TriangleNode *node)
{
	_node = node;
}

TriangleNode *TriangleWrapper::node() { return _node; }
void TriangleWrapper::setNode(TriangleNode *node) { _node = node; }
bool TriangleWrapper::isQuad() { return _node->data().isQuad(); }
int TriangleWrapper::count() { return _node->data().count(); }
bool TriangleWrapper::selected() { return _node->data().selected; }
void TriangleWrapper::setSelected(bool selected) { _node->data().selected = selected; }

VertexWrapper ^TriangleWrapper::vertex(int index)
{
	return gcnew VertexWrapper(_node->data().vertex(index));
}

void TriangleWrapper::setVertex(VertexWrapper ^vertex, int index)
{
	_node->data().setVertex(index, vertex->node());
}

EdgeWrapper ^TriangleWrapper::edge(int index)
{
	return gcnew EdgeWrapper(_node->data().vertexEdge(index));
}

void TriangleWrapper::setEdge(EdgeWrapper ^edge, int index)
{
	_node->data().setVertexEdge(index, edge->node());
}

VertexWrapper ^TriangleWrapper::vertexNotInEdge(EdgeWrapper ^edge)
{
	return gcnew VertexWrapper(_node->data().vertexNotInEdge(&edge->node()->data()));
}

EdgeWrapper::EdgeWrapper(VertexEdgeNode *node)
{
	_node = node;
}

VertexEdgeNode *EdgeWrapper::node() { return _node; }
void EdgeWrapper::setNode(VertexEdgeNode *node) { _node = node; }
bool EdgeWrapper::selected() { return _node->data().selected; }
void EdgeWrapper::setSelected(bool selected) { _node->data().selected = selected; }

VertexWrapper ^EdgeWrapper::half()
{
	if (_node->data().half)
        return gcnew VertexWrapper(_node->data().half);
    return nullptr;
}

void EdgeWrapper::setHalf(VertexWrapper ^half)
{
    if (half)
        _node->data().half = half->node();
    else
        _node->data().half = NULL;
}

VertexWrapper ^EdgeWrapper::vertex(int index)
{
    VertexNode *vertex = _node->data().vertex(index);
    if (vertex)
        return gcnew VertexWrapper(vertex);
    return nullptr;
}

void EdgeWrapper::setVertex(VertexWrapper ^vertex, int index)
{
    if (vertex)
        _node->data().setVertex(index, vertex->node());
    else
        _node->data().setVertex(index, NULL);
}

TriangleWrapper ^EdgeWrapper::triangle(int index)
{
    TriangleNode *triangle = _node->data().triangle(index);
    if (triangle)
        return gcnew TriangleWrapper(triangle);
    return nullptr;
}

void EdgeWrapper::setTriangle(TriangleWrapper ^triangle, int index)
{
    if (triangle)
        _node->data().setTriangle(index, triangle->node());
    else
        _node->data().setTriangle(index, NULL);
}

VertexWrapper ^EdgeWrapper::oppositeVertex(VertexWrapper ^vertex)
{
	return gcnew VertexWrapper(_node->data().opposite(vertex->node()));
}

VertexNodeEdgeWrapper::VertexNodeEdgeWrapper(Vertex2VEdgeNode *edgeNode)
	: EdgeWrapper(edgeNode->data())
{
	_simpleNode = edgeNode;
}

Vertex2VEdgeNode *VertexNodeEdgeWrapper::simpleNode()
{
	return _simpleNode;
}

void VertexNodeEdgeWrapper::setSimpleNode(Vertex2VEdgeNode *simpleNode)
{
    _simpleNode = simpleNode;
    setNode(_simpleNode->data());
}

#define ImplementIterator(Name, TBase, TNode, nodeProperty, setNodeProperty) \
\
Name::Name(TNode *theBegin, TNode *theEnd) \
	: TBase(theBegin) \
{ \
	begin = theBegin; \
	end = theEnd; \
} \
\
bool Name::finished() { return this->nodeProperty() == end; } \
void Name::moveStart() { this->setNodeProperty(begin); } \
void Name::moveNext() { this->setNodeProperty(this->nodeProperty()->next()); }

ImplementIterator(VertexNodeIterator, VertexWrapper, VertexNode, node, setNode)
ImplementIterator(TriangleNodeIterator, TriangleWrapper, TriangleNode, node, setNode)
ImplementIterator(EdgeNodeIterator, EdgeWrapper, VertexEdgeNode, node, setNode)
ImplementIterator(VertexNodeEdgeIterator, VertexNodeEdgeWrapper, Vertex2VEdgeNode, simpleNode, setSimpleNode)

}

#endif