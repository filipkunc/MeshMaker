//
//  FPList.h
//  GLMotionMaker
//
//  Created by Filip Kunc on 10/15/10.
//  For license see LICENSE.TXT
//

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
    
    template <class U>
    friend class FPList;
};

template <class T>
class FPList
{
private:
    FPNode<T> *begin;
    FPNode<T> *end;
    int count;
public:
    FPList()
    {
        begin = new FPNode<T>();
        end = new FPNode<T>();
        
        begin->next = end;
        end->previous = begin;
        count = 0;
    }
    
    ~FPList()
    {
        RemoveAll();
        delete begin;
        delete end;
    }
    
    FPNode<T> *Begin() { return begin->next; }
    FPNode<T> *End() { return end; }
    
    FPNode<T> *First(int n)
    {
        FPNode<T> *current = begin->next;
        
        int i = 0;
        
        while (current != end)
        {
            FPNode<T> *next = current->next;
            
            if (i == n)
                return current;
            
            current = next;
        }
        
        return NULL;
    }
    
    FPNode<T> *Last(int n)
    {
        FPNode<T> *current = end->previous;
        
        int i = 0;
        
        while (current != begin)
        {
            FPNode<T> *next = current->previous;
            
            if (i == n)
                return current;
            
            current = next;
        }
        
        return NULL;
    }    
    
    FPNode<T> *First()
    {
        if (begin->next != end)
            return begin->next;
        return NULL;
    }
    
    FPNode<T> *Last()
    {
        if (end->previous != begin)
            return end->previous;
        return NULL;
    }
    
    int Count()
    {
        return count;
    }
    
    void Remove(FPNode<T> *node)
    {
        FPNode<T> *next = node->next;
        FPNode<T> *previous = node->previous;
    
        next->previous = previous;
        previous->next = next;
        
        delete node;
        
        count--;
    }
    
    void RemoveAll()
    {
        FPNode<T> *current = begin->next;
        
        while (current != end)
        {
            current = current->next;
            delete current->previous;
        }
        
        begin->next = end;
        end->previous = begin;
        
        count = 0;
    }
    
    FPNode<T> *Add(const T &data)
    {
        FPNode<T> *newOne = new FPNode<T>(data);
        FPNode<T> *previous = end->previous;
        newOne->next = end;
        newOne->previous = previous;
        previous->next = newOne;
        end->previous = newOne;
        
        count++;
        
        return newOne;
    }
    
    void Iterate(void (^Iterator)(FPNode<T> *node, bool *stop))
    {
        bool stopLoop = NO; 
        FPNode<T> *current = begin->next;
        
        while (current != end)
        {
            FPNode<T> *next = current->next;
            Iterator(current, &stopLoop);
            if (stopLoop)
                return;
            current = next;
        }        
    }
    
    bool Contains(const T &item)
    {
        FPNode<T> *current = begin->next;
        
        while (current != end)
        {
            if (current->data == item)
                return true;

            current = current->next;
        }        
        return false;
    }
};
