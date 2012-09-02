//
//  Vertex.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#pragma once

template <class T>
class VNode : public FPNode<VNode<T>, T>
{
public:
    union AlgorithmData
    {
        uint index;
        Vector3D normal;
        VNode *duplicatePair;
        
        AlgorithmData()
        {
            memset(this, 0, sizeof(AlgorithmData));
        }
        
        AlgorithmData(const AlgorithmData &data)
        {
            memcpy(this, &data, sizeof(AlgorithmData));
        }
        
        void clear()
        {
            memset(this, 0, sizeof(AlgorithmData));
        }
    };
    
public:
    SimpleList<TriangleNode *> _triangles;
    SimpleList<VEdgeNode<T> *> _edges;
public:
    float selectionWeight;
    AlgorithmData algorithmData;
    
    VNode() : FPNode<VNode<T>, T>() { }
    VNode(const T &vertex) : FPNode<VNode<T>, T>(vertex) { } 
    virtual ~VNode() 
    { 
        removeFromTriangles();
        removeFromEdges();
    }
    
    bool isUsed() const { return _triangles.count() > 0; }
    void addTriangle(TriangleNode *triangle) { _triangles.add(triangle); }
    void removeTriangle(TriangleNode *triangle)
    {
        for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            if (node->data() == triangle)
                _triangles.remove(node);
        }
    }
    void removeFromTriangles()
    {
        for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            node->data()->data().removeVertex(this);
        }
        
        _triangles.removeAll(); 
    }
    
    void removeFromSelectedTriangles()
    {
        for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            if (node->data()->data().selected)
            {
                node->data()->data().removeVertex(this);
                _triangles.remove(node);
            }
        }
    }
    
    void addEdge(VEdgeNode<T> *edge)
    {
        _edges.add(edge);
    }
    
    void removeEdge(VEdgeNode<T> *edge)
    {
        for (SimpleNode<VEdgeNode<T> *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            if (node->data() == edge)
                _edges.remove(node);
        }
    }
    
    void removeEdges()
    {
        _edges.removeAll();
    }
    
    void removeFromEdges()
    {
        for (SimpleNode<VEdgeNode<T> *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            node->data()->data().removeVertex(this);
        }
        
        _edges.removeAll();
    }
    
    void replaceVertex(VNode *newVertex)
    {
        for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            node->data()->replaceVertex(this, newVertex);
        }
        
        for (SimpleNode<VEdgeNode<T> *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            node->data()->replaceVertex(this, newVertex);
        }
        
        _triangles.removeAll();
        _edges.removeAll();
    }
    
    void replaceVertexInSelectedTriangles(VNode *newVertex)
    {
        for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            if (node->data()->data().selected)
            {
                node->data()->replaceVertex(this, newVertex);
                _triangles.remove(node);
            }
        } 
    }
    
    void computeNormal()
    {
        float count = 0;
        algorithmData.normal = Vector3D();
        
        for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            algorithmData.normal += node->data()->data().vertexNormal;
            count++;
        }
        
        algorithmData.normal /= count;
    }
    
    VEdgeNode<T> *sharedEdge(VNode *otherVertex)
    {
        for (SimpleNode<VEdgeNode<T> *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            if (node->data()->data().containsVertex(otherVertex))
                return node->data();
        }
        return NULL;
    }
};
