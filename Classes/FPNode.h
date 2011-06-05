//
//  FPNode.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/4/11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#import "FPList.h"

template <class TNode, class TData>
class FPNode
{
private:
    TNode *next;
    TNode *previous;
public:
    TData data;
    
    FPNode()
    {
        next = NULL;
        previous = NULL;
    }
    
    FPNode(const TData &data)
    {
        this->data = data;
        next = NULL;
        previous = NULL;
    }
    
    virtual ~FPNode()
    {
        
    }
    
    TNode *Next() { return next; }
    TNode *Previous() { return previous; }
    
    template <class U, class V>
    friend class FPList;
};
