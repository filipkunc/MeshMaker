//
//  FPArrayCache.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 7/23/11.
//  For license see LICENSE.TXT
//

#pragma once

template <class T>
class FPArrayCache
{
private:
    T *_array;
    unsigned int _count;
    unsigned int _capacity;
    bool _isValid;
    
public:
    FPArrayCache()
    {
        _count = 0;
        _capacity = 0;
        _array = NULL;
        _isValid = false;
    }
    
    ~FPArrayCache() 
    { 
        if (_array)
            delete [] _array; 
    }
    
    operator T *() { return _array; }
                  
    unsigned int count() const { return _count; }
    bool isValid() const { return _isValid; }
    void setValid(bool valid) { _isValid = valid; }

    void clear()
    {
        _count = 0;
        _isValid = false;
    }
    
    void trim()
    {
        if (_capacity > _count)
            realloc();
        _isValid = false;
    }

    void resize(unsigned int count)
    {
        _count = count;
        
        if (_capacity < _count)
            realloc();            
        
        _isValid = false;
    }    
private:    
    void realloc()
    {
        _capacity = _count; 
        
        if (_array)
            delete [] _array;
        
        if (_count > 0)
            _array = new T[_count];
        else
            _array = NULL;            
    }
};