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

- (void)removeTriQuad:(TriangleNodeIterator *)triQuadIterator
{
    TriangleNode *current = triQuadIterator.node;
    mesh->removeTriQuad(current);
    triQuadIterator.node = current;
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

+ (NSString *)webScriptNameForSelector:(SEL)sel
{
    if (sel == @selector(setSelected:))
        return @"setSelected";
    if (sel == @selector(vertexAtIndex:))
        return @"vertex";
    if (sel == @selector(setVertex:atIndex:))
        return @"setVertex";
    
    return nil;
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector { return NO; }
+ (BOOL)isKeyExcludedFromWebScript:(const char *)name { return NO; }

@end

@implementation EdgeWrapper

@synthesize node;

- (BOOL)selected { return node->data().selected; }
- (void)setSelected:(BOOL)selected { node->data().selected = selected; }

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
    return [[VertexWrapper alloc] initWithNode:node->data().vertex(index)];
}

- (void)setVertex:(VertexWrapper *)vertex atIndex:(uint)index
{
    node->data().setVertex(index, vertex.node);
}

- (TriangleWrapper *)triangleAtIndex:(uint)index
{
    return [[TriangleWrapper alloc] initWithNode:node->data().triangle(index)];
}

- (void)setTriangle:(TriangleWrapper *)triangle atIndex:(uint)index
{
    node->data().setTriangle(index, triangle.node);
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
    
    return nil;
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)aSelector { return NO; }
+ (BOOL)isKeyExcludedFromWebScript:(const char *)name { return NO; }

@end

@implementation VertexNodeIterator

- (BOOL)finished { return self.node == end; }

- (id)initWithBegin:(VertexNode *)theBegin end:(VertexNode *)theEnd
{
    self = [super init];
    if (self)
    {
        begin = theBegin;
        end = theEnd;
        self.node = begin;
    }
    return self;
}

- (void)moveStart { self.node = begin; }
- (void)moveNext { self.node = self.node->next(); }

@end

@implementation TriangleNodeIterator

- (BOOL)finished { return self.node == end; }

- (id)initWithBegin:(TriangleNode *)theBegin end:(TriangleNode *)theEnd
{
    self = [super init];
    if (self)
    {
        begin = theBegin;
        end = theEnd;
        self.node = begin;
    }
    return self;
}

- (void)moveStart { self.node = begin; }
- (void)moveNext { self.node = self.node->next(); }

@end

@implementation EdgeNodeIterator

- (BOOL)finished { return self.node == end; }

- (id)initWithBegin:(VertexEdgeNode *)theBegin end:(VertexEdgeNode *)theEnd
{
    self = [super init];
    if (self)
    {
        begin = theBegin;
        end = theEnd;
        self.node = begin;
    }
    return self;
}

- (void)moveStart { self.node = begin; }
- (void)moveNext { self.node = self.node->next(); }

@end
