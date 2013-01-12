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
#if defined(__APPLE__)
    union
#elif defined(WIN32)
    struct
#endif
    AlgorithmData
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
    FPList<VertexTriangleNode, TriangleNode *> _triangles;
    FPList<VertexVEdgeNode<T>, VEdgeNode<T> *> _edges;
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
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            if (node->data() == triangle)
                _triangles.remove(node);
        }
    }
    void removeFromTriangles()
    {
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            node->data()->data().removeVertex(this);
        }
        
        _triangles.removeAll(); 
    }
    
    void removeFromSelectedTriangles()
    {
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
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
        for (VertexVEdgeNode<T> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
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
        for (VertexVEdgeNode<T> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            node->data()->data().removeVertex(this);
        }
        
        _edges.removeAll();
    }
    
    void replaceVertex(VNode *newVertex)
    {
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            node->data()->replaceVertex(this, newVertex);
        }
        
        for (VertexVEdgeNode<T> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            node->data()->replaceVertex(this, newVertex);
        }
        
        _triangles.removeAll();
        _edges.removeAll();
    }
    
    void replaceVertexInSelectedTriangles(VNode *newVertex)
    {
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            if (node->data()->data().selected)
            {
                node->data()->replaceVertex(this, newVertex);
                _triangles.remove(node);
            }
        } 
    }
    
    void updateTriangleNormals()
    {
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            node->data()->data().normalsAreValid = false;
            node->data()->data().computeNormalsIfNeeded();
        }
    }    
    
    void computeNormal()
    {
        float count = 0;
		Vector3D normal = Vector3D();
        
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            normal += node->data()->data().vertexNormal;
            count++;
        }
        
        normal /= count;
		algorithmData.normal = normal;
    }
    
    void resetCacheIndices()
    {
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            node->cacheIndices[0] = -1;
            node->cacheIndices[1] = -1;
        }
    }
    
    void setCacheIndexForTriangleNode(TriangleNode *triangleNode, uint cacheIndex, uint cacheIndexPosition)
    {
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            if (node->data() == triangleNode)
            {
                node->cacheIndices[cacheIndexPosition] = cacheIndex;
                break;
            }
        }
    }
    
    void setCacheIndexForEdgeNode(VEdgeNode<T> *edgeNode, uint cacheIndex)
    {
        for (VertexVEdgeNode<T> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            if (node->data() == edgeNode)
            {
                node->cacheIndex = cacheIndex;
                break;
            }
        }
    }
    
    void addAffectedVertices(vector<VNode *> &affectedVertices)
    {
        for (VertexTriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        {
            const Triangle2 &triangle = node->data()->data();
            uint count = triangle.count();
            
            for (uint i = 0; i < count; i++)
            {
                VertexNode *vertexNode = triangle.vertex(i);
                if (!vertexNode->data().selected && vertexNode->sharedEdge(this) == NULL)
                    affectedVertices.push_back(vertexNode);
            }
        }
        
        for (VertexVEdgeNode<T> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            VertexNode *vertexNode = node->data()->data().opposite(this);
            if (!vertexNode->data().selected)
               affectedVertices.push_back(vertexNode);
        }
    }
    
    VEdgeNode<T> *sharedEdge(const VNode *otherVertex) const
    {
        for (VertexVEdgeNode<T> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            if (node->data()->data().containsVertex(otherVertex))
                return node->data();
        }
        return NULL;
    }
    
    VEdgeNode<T> *nextEdgeInLoop(const VEdge<T> &edge) const
    {
        if (_edges.count() != 4)
            return NULL;
        
        for (VertexVEdgeNode<T> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            if (node->data()->data().sharedTriangle(edge) == NULL)
                return node->data();
        }
        
        return NULL;
    }
};
