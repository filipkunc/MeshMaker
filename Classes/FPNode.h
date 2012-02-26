//
//  FPNode.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/4/11.
//  For license see LICENSE.TXT
//

#pragma once

#include "FPList.h"

template <class TNode, class TData>
class FPNode
{
private:
    TNode *_next;
    TNode *_previous;
    TData _data;
public:
    FPNode()
    {
        _next = NULL;
        _previous = NULL;
    }
    
    FPNode(const TData &data)
    {
        _data = data;
        _next = NULL;
        _previous = NULL;
    }
    
    virtual ~FPNode()
    {
        
    }
    
    TData &data() { return _data; }
    virtual void setData(const TData &data) { _data = data; }
    TNode *next() { return _next; }
    TNode *previous() { return _previous; }
    
    template <class U, class V>
    friend class FPList;
};


