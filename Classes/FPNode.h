//
//  FPNode.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/4/11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#pragma once

#include "FPList.h"

template <class TNode, class TData>
class FPNode
{
private:
    TNode *_next;
    TNode *_previous;
public:
    TData data;
    
    FPNode()
    {
        _next = NULL;
        _previous = NULL;
    }
    
    FPNode(const TData &data)
    {
        this->data = data;
        _next = NULL;
        _previous = NULL;
    }
    
    virtual ~FPNode()
    {
        
    }
    
    TNode *next() { return _next; }
    TNode *previous() { return _previous; }
    
    template <class U, class V>
    friend class FPList;
};


