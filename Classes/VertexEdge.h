//
//  VertexEdge.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#pragma once

template <class T>
class VEdge
{
private:
    VNode<T> *_vertices[2];
    TriangleNode *_triangles[2];
public:
    bool selected;
    bool visible;
    VNode<T> *half;
    
    VEdge() : selected(false), visible(true), half(NULL)
    {
        for (uint i = 0; i < 2; i++)
        {
            _vertices[i] = NULL;
            _triangles[i] = NULL;
        }
    }
    
    VEdge(VNode<T> *vertices[2]) : selected(false), visible(true), half(NULL)
    {
        for (uint i = 0; i < 2; i++)
        {
            _vertices[i] = vertices[i];
            _triangles[i] = NULL;
        }
    }
    
    bool isDegenerated() const
    {
        if (_triangles[0] == NULL && _triangles[1] == NULL)
            return true;
        
        if (containsVertex(NULL))
            return true;
        
        if (_vertices[0] == _vertices[1])
            return true;
        
        return false;
    }
    
    bool containsVertex(const VNode<T> *vertex) const
    {
        for (uint i = 0; i < 2; i++)
        {
            if (_vertices[i] == vertex)
                return true;        
        }
        return false;
    }
    
    bool containsTriangle(const TriangleNode *triangle) const
    {
        for (uint i = 0; i < 2; i++)
        {
            if (_triangles[i] == triangle)
                return true;
        }
        return false;
    }
    
    bool isNotShared() const
    {
        for (uint i = 0; i < 2; i++)
        {
            if (_triangles[i] == NULL || !_triangles[i]->data().selected)
                return true;
        }
        return false;
    }
    
    VertexNode *sharedVertex(const VEdge &edge) const
    {
        for (uint i = 0; i < 2; i++)
        {
            if (edge.containsVertex(_vertices[i]))
                return _vertices[i];
        }
        return NULL;
    }
    
    TriangleNode *sharedTriangle(const VEdge &edge) const
    {
        for (uint i = 0; i < 2; i++)
        {
            if (edge.containsTriangle(_triangles[i]))
                return _triangles[i];
        }
        return NULL;
    }
    
    VNode<T> *vertex(uint index) const { return _vertices[index]; }
    VNode<T> *texCoord(uint index) const { return _vertices[index]; }
    TriangleNode *triangle(uint index) const { return _triangles[index]; }
    
    void setTriangle(uint index, TriangleNode *value) { _triangles[index] = value; }
    void removeVertex(VNode<T> *vertex)
    {
        for (uint i = 0; i < 2; i++)
        {
            if (_vertices[i] == vertex)
            {
                _vertices[i] = NULL;
                break;
            }
        }
    }
    
    void removeTriangle(TriangleNode *triangle)
    {
        for (uint i = 0; i < 2; i++)
        {
            if (_triangles[i] == triangle)
            {
                _triangles[i] = NULL;
                break;
            }
        }
    }
    
    void turn()
    {
        if (_triangles[0] == NULL || _triangles[1] == NULL)
            return;
        
        Triangle2 &t0 = _triangles[0]->data();
        Triangle2 &t1 = _triangles[1]->data();
        
        VNode<T> *v0 = t0.vertexNotInEdge(this);
        VNode<T> *v1 = t1.vertexNotInEdge(this);
        
        _triangles[0]->removeFromVertices();
        _triangles[1]->removeFromVertices();
        
        t0.setVertex(0, v1);
        t0.setVertex(1, v0);
        t0.setVertex(2, _vertices[0]);
        
        _triangles[0]->addToVertices();
        
        t1.setVertex(0, v0);
        t1.setVertex(1, v1);
        t1.setVertex(2, _vertices[1]);
        
        _triangles[1]->addToVertices();
        
        _vertices[0] = v0;
        _vertices[1] = v1;
    }
    
    VNode<T> *opposite(VNode<T> *vertex) const
    {
        if (_vertices[0] == vertex)
            return _vertices[1];
        
        return _vertices[0];
    }
    
    void selectEdgesInLoop()
    {
        VNode<T> *vertex = _vertices[1];
        VEdge<T> *edge = this;
        
        while (!edge->selected)
        {
            edge->selected = true;
            
            VEdgeNode<T> *nextEdge = vertex->nextEdgeInLoop(*edge);
            if (nextEdge == NULL)
                return;
            
            edge = &nextEdge->data();
            vertex = edge->opposite(vertex);
        }
    }
    
    friend class VEdgeNode<T>;
};

template <class T>
class VEdgeNode : public FPNode<VEdgeNode<T>, VEdge<T> >
{
public:
    VEdgeNode() : FPNode<VEdgeNode<T>, VEdge<T> >() { }
    
    VEdgeNode(const VEdge<T> &edge) : FPNode<VEdgeNode<T>, VEdge<T> >(edge)
    {
        addToVertices();
    }
    
    virtual ~VEdgeNode()
    {
        removeFromVertices();
        removeFromTriangles();
    }
    
    virtual void setData(const VEdge<T> &data) 
    { 
        FPNode<VEdgeNode<T>, VEdge<T> >::setData(data);
        addToVertices();
    }    
    
    void addToVertices()
    {
        for (uint i = 0; i < 2; i++)
        {
            if (this->data()._vertices[i])
                this->data()._vertices[i]->addEdge(this);
        }
    }
    
    void removeFromVertices()
    {
        for (uint i = 0; i < 2; i++)
        {
            if (this->data()._vertices[i])
            {
                this->data()._vertices[i]->removeEdge(this);
                this->data()._vertices[i] = NULL;
            }
        }
    }
    
    void removeFromTriangles()
    {
        for (uint i = 0; i < 2; i++)
        {
            if (this->data()._triangles[i])
            {
                this->data()._triangles[i]->data().removeEdge(this);
                this->data()._triangles[i] = NULL;
            }
        }
    }
    
    void replaceVertex(VNode<T> *currentVertex, VNode<T> *newVertex)
    {
        for (uint i = 0; i < 2; i++)
        {
            if (this->data()._vertices[i] == currentVertex)
            {
                this->data()._vertices[i] = newVertex;
                newVertex->addEdge(this);
                break;
            }
        }
    }
};
