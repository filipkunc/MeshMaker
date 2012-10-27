//
//  JSWrappers.mm
//  MeshMaker
//
//  Created by Filip Kunc on 10/27/12.
//
//

#import "JSWrappers.h"

@implementation ItemCollection (JSWrappers)

+ (NSString *)webScriptNameForSelector:(SEL)sel
{
    if (sel == @selector(itemAtIndex:))
        return @"at";
    
    return nil;
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector { return NO; }
+ (BOOL)isKeyExcludedFromWebScript:(const char *)name { return NO; }

@end

@implementation Item (JSWrappers)

- (VertexNodeIterator *)vertexIterator
{
    return [[VertexNodeIterator alloc] initWithBegin:mesh->vertices().begin() end:mesh->vertices().end()];
}

- (TriangleNodeIterator *)triQuadIterator
{
    return [[TriangleNodeIterator alloc] initWithBegin:mesh->triangles().begin() end:mesh->triangles().end()];
}

- (EdgeNodeIterator *)edgeIterator
{
    return [[EdgeNodeIterator alloc] initWithBegin:mesh->vertexEdges().begin() end:mesh->vertexEdges().end()];
}

- (uint)vertexCount
{
    return mesh->vertexCount();
}

- (uint)triQuadCount
{
    return mesh->triangleCount();
}

- (void)makeEdges
{
    mesh->makeEdges();
}

- (void)updateSelection
{
    mesh->setSelectionMode(mesh->selectionMode());
}

- (VertexWrapper *)addVertexWithX:(float)x y:(float)y z:(float)z
{
    return [[VertexWrapper alloc] initWithNode:mesh->addVertex(Vector3D(x, y, z))];
}

- (TriangleWrapper *)addTriangleWithFirst:(VertexWrapper *)v0 second:(VertexWrapper *)v1 third:(VertexWrapper *)v2
{
    return [[TriangleWrapper alloc] initWithNode:mesh->addTriangle(v0.node, v1.node, v2.node)];
}

- (TriangleWrapper *)addQuadWithFirst:(VertexWrapper *)v0 second:(VertexWrapper *)v1 third:(VertexWrapper *)v2 fourth:(VertexWrapper *)v3
{
    return [[TriangleWrapper alloc] initWithNode:mesh->addQuad(v0.node, v1.node, v2.node, v3.node)];
}

- (void)removeTriQuad:(TriangleWrapper *)triQuad
{
    TriangleNode *current = triQuad.node;
    mesh->removeTriQuad(current);
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
- (SimpleNodeEdgeIterator *)edgeIterator { return [[SimpleNodeEdgeIterator alloc] initWithBegin:node->_edges.begin() end:node->_edges.end()]; }

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

@implementation SimpleNodeEdgeWrapper

- (SimpleNode<VertexEdgeNode *> *)simpleNode
{
    return _simpleNode;
}

- (void)setSimpleNode:(SimpleNode<VertexEdgeNode *> *)simpleNode
{
    _simpleNode = simpleNode;
    self.node = _simpleNode->data();
}

- (id)initWithSimpleNode:(SimpleNode<VertexEdgeNode *> *)edgeNode
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
@end

ImplementIterator(VertexNodeIterator, VertexNode, node)
ImplementIterator(TriangleNodeIterator, TriangleNode, node)
ImplementIterator(EdgeNodeIterator, VertexEdgeNode, node)
ImplementIterator(SimpleNodeEdgeIterator, SimpleNode<VertexEdgeNode *>, simpleNode)
