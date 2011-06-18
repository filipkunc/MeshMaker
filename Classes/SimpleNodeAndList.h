//
//  SimpleNodeAndList.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/5/11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#pragma once

#import "FPNode.h"

template <class TData>
class SimpleNode : public FPNode<SimpleNode<TData>, TData>
{
public:
    SimpleNode() : FPNode<SimpleNode<TData>, TData>() { }
    SimpleNode(const TData &data) : FPNode<SimpleNode<TData>, TData>(data) { }
    virtual ~SimpleNode() { }
};

template <class TData>
class SimpleList : public FPList<SimpleNode<TData>, TData>
{
public:
    SimpleList() : FPList<SimpleNode<TData>, TData> () { }
    virtual ~SimpleList() { }
};


