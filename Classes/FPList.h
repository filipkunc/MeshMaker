//
//  FPList.h
//  GLMotionMaker
//
//  Created by Filip Kunc on 10/15/10.
//  For license see LICENSE.TXT
//

template <class TNode, class TData>
class FPList
{
private:
    TNode *begin;
    TNode *end;
    int count;
public:
    FPList()
    {
        begin = new TNode();
        end = new TNode();
        
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
    
    TNode *Begin() { return begin->next; }
    TNode *End() { return end; }
    
    TNode *First(int n)
    {
        TNode *current = begin->next;
        
        int i = 0;
        
        while (current != end)
        {
            TNode *next = current->next;
            
            if (i == n)
                return current;
            
            current = next;
        }
        
        return NULL;
    }
    
    TNode *Last(int n)
    {
        TNode *current = end->previous;
        
        int i = 0;
        
        while (current != begin)
        {
            TNode *next = current->previous;
            
            if (i == n)
                return current;
            
            current = next;
        }
        
        return NULL;
    }    
    
    TNode *First()
    {
        if (begin->next != end)
            return begin->next;
        return NULL;
    }
    
    TNode *Last()
    {
        if (end->previous != begin)
            return end->previous;
        return NULL;
    }
    
    int Count()
    {
        return count;
    }
    
    void Remove(TNode *node)
    {
        TNode *next = node->next;
        TNode *previous = node->previous;
    
        next->previous = previous;
        previous->next = next;
        
        delete node;
        
        count--;
    }
    
    void RemoveAll()
    {
        TNode *current = begin->next;
        
        while (current != end)
        {
            current = current->next;
            delete current->previous;
        }
        
        begin->next = end;
        end->previous = begin;
        
        count = 0;
    }
    
    TNode *Add(const TData &data)
    {
        TNode *newOne = new TNode(data);
        TNode *previous = end->previous;
        newOne->next = end;
        newOne->previous = previous;
        previous->next = newOne;
        end->previous = newOne;
        
        count++;
        
        return newOne;
    }   
};
