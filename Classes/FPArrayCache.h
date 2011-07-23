//
//  FPArrayCache.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/23/11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#pragma once

template <class T>
class FPArrayCache
{
private:
    T *_array;
    int _count;
    bool _isValid;
public:
    FPArrayCache()
    {
        _count = 0;
        _array = NULL;
        _isValid = false;
    }
    
    ~FPArrayCache() 
    { 
        if (_array)
            delete [] _array; 
    }
    
    operator T *() { return _array; }
                  
    int count() const { return _count; }
    bool isValid() const { return _isValid; }
    void setValid(bool valid) { _isValid = valid; }

    void resize(int count) 
    {
        if (_count != count)
            _count = count;
        
        if (_array)
            delete [] _array;
        _array = new T[_count];
        
        _isValid = false;
    }
};