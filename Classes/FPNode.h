//
//  FPNode.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/4/11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#import "FPList.h"

template <class T>
class FPNode
{
private:
    FPNode *next;
    FPNode *previous;
public:
    T data;
    
    FPNode()
    {
        next = NULL;
        previous = NULL;
    }
    
    FPNode(const T &data)
    {
        this->data = data;
        next = NULL;
        previous = NULL;
    }
    
    ~FPNode()
    {
        
    }
    
    FPNode *Next() { return next; }
    FPNode *Previous() { return previous; }
    
    template <class U, class V>
    friend class FPList;
};
