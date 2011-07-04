/*
 *  MeshHelpers.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/23/09.
 *  For license see LICENSE.TXT
 *
 */

#import "MeshHelpers.h"

void VertexNode::addTriangle(TriangleNode *triangle)
{
    _triangles.add(triangle);
}

void VertexNode::removeTriangle(TriangleNode *triangle)
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (node->data == triangle)
            _triangles.remove(node);
    }
}

void VertexNode::removeFromTriangles()
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        node->data->data.removeVertex(this);
    }
    
    _triangles.removeAll();
}

void VertexNode::addEdge(EdgeNode *edge)
{
    _edges.add(edge);
}

void VertexNode::removeEdge(EdgeNode *edge)
{
    for (SimpleNode<EdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data == edge)
            _edges.remove(node);
    }
}

void VertexNode::removeEdges()
{
    _edges.removeAll();
}

void VertexNode::removeFromEdges()
{
    for (SimpleNode<EdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        node->data->data.removeVertex(this);
    }
    
    _edges.removeAll();
}

void VertexNode::replaceVertex(VertexNode *newVertex)
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        node->data->replaceVertex(this, newVertex);
    }
    
    for (SimpleNode<EdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        node->data->replaceVertex(this, newVertex);
    }
    
    _triangles.removeAll();
    _edges.removeAll();
}

void VertexNode::replaceVertexInSelectedTriangles(VertexNode *newVertex)
{
    for (SimpleNode<TriangleNode *> *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        if (node->data->data.selected)
        {
            node->data->replaceVertex(this, newVertex);
            _triangles.remove(node);
        }
    }    
}

EdgeNode *VertexNode::sharedEdge(VertexNode *otherVertex)
{
    for (SimpleNode<EdgeNode *> *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (node->data->data.containsVertex(otherVertex))
            return node->data;
    }
    return NULL;
}

Triangle2::Triangle2() : selected(false)
{
    for (int i = 0; i < 3; i++)
    {
        _vertices[i] = NULL;
        _edges[i] = NULL;
    }
}

Triangle2::Triangle2(VertexNode *vertices[3]) : selected(false)
{
    for (int i = 0; i < 3; i++)
    {
        _vertices[i] = vertices[i];
        _edges[i] = NULL;
    }
}

void TriangleNode::addToVertices()
{
    for (int i = 0; i < 3; i++)
    {
        if (data._vertices[i])
            data._vertices[i]->addTriangle(this);
    }
}

void TriangleNode::removeFromVertices()
{
    for (int i = 0; i < 3; i++)
    {
        if (data._vertices[i])
        {
            data._vertices[i]->removeTriangle(this);
            data._vertices[i] = NULL;
        }
    }
}

void TriangleNode::removeFromEdges()
{
    for (int i = 0; i < 3; i++)
    {
        if (data._edges[i])
        {
            data._edges[i]->data.removeTriangle(this);
            data._edges[i] = NULL;
        }
    }
}

void TriangleNode::replaceVertex(VertexNode *currentVertex, VertexNode *newVertex)
{
    for (int i = 0; i < 3; i++)
    {
        if (data._vertices[i] == currentVertex)
        {
            data._vertices[i] = newVertex;
            newVertex->addTriangle(this);
            break;
        }
    }
}

void Triangle2::removeVertex(VertexNode *vertex)
{
    for (int i = 0; i < 3; i++)
    {
        if (_vertices[i] == vertex)
        {
            _vertices[i] = NULL;
            break;
        }
    }
}

void Triangle2::removeEdge(EdgeNode *edge)
{
    for (int i = 0; i < 3; i++)
    {
        if (_edges[i] == edge)
        {
            _edges[i] = NULL;
            break;
        }
    }
}

void Triangle2::removeEdges()
{
    for (int i = 0; i < 3; i++)
        _edges[i] = NULL;
}

bool Triangle2::isDegenerated() const
{
    if (containsVertex(NULL))
        return true;
    
    if (containsEdge(NULL))
        return true;
    
    if (_vertices[0] == _vertices[1])
		return true;
	if (_vertices[0] == _vertices[2])
		return true;
	if (_vertices[1] == _vertices[2])
		return true;
	
	return false;
}

bool Triangle2::containsVertex(const VertexNode *vertex) const
{
    for (int i = 0; i < 3; i++)
	{
		if (_vertices[i] == vertex)
			return true;
	}
	return false;
}

bool Triangle2::containsEdge(const EdgeNode *edge) const
{
    for (int i = 0; i < 3; i++)
    {
        if (_edges[i] == edge)
            return true;
    }
    return false;
}

void Triangle2::getVertexPositions(Vector3D vertexPositions[3]) const
{
    for (int i = 0; i < 3; i++)
        vertexPositions[i] = _vertices[i]->data.position;
}

void Triangle2::flip()
{
    swap(_vertices[0], _vertices[2]);    
}

int Triangle2::indexOfVertex(const VertexNode *vertex) const
{
    for (int i = 0; i < 3; i++)
    {
        if (_vertices[i] == vertex)
            return i;
    }
    return -1;
}

void Triangle2::sortVertices(VertexNode *&v1, VertexNode *&v2) const
{
    int index1 = indexOfVertex(v1);
    int index2 = indexOfVertex(v2);
    
    if (index1 == 1 && index2 == 0)
        swap(v1, v2);
    else if (index1 == 2 && index2 == 1)
        swap(v1, v2);
    else if (index1 == 0 && index2 == 2)
        swap(v1, v2);
}

VertexNode *Triangle2::vertexNotInEdge(const Edge2 *edge) const
{
    for (int i = 0; i < 3; i++)
    {
        if (_vertices[i] != edge->vertex(0) &&
            _vertices[i] != edge->vertex(1))
            return _vertices[i];
    }
    return NULL;
}

Vector3D Triangle2::computeNormal() const
{
    Vector3D u = _vertices[0]->data.position - _vertices[1]->data.position;
	Vector3D v = _vertices[1]->data.position - _vertices[2]->data.position;
	return u.Cross(v);
}

Edge2::Edge2() : selected(false)
{
    for (int i = 0; i < 2; i++)
    {
        _vertices[i] = NULL;
        _triangles[i] = NULL;
    }
}

Edge2::Edge2(VertexNode *vertices[2]) : selected(false)
{
    for (int i = 0; i < 2; i++)
    {
        _vertices[i] = vertices[i];
        _triangles[i] = NULL;
    }
}

bool Edge2::isDegenerated() const
{
    if (_triangles[0] == NULL && _triangles[1] == NULL)
        return true;
    
    if (containsVertex(NULL))
        return true;
        
    if (_vertices[0] == _vertices[1])
        return true;
        
    return false;    
}

bool Edge2::containsVertex(const VertexNode *vertex) const
{
    for (int i = 0; i < 2; i++)
    {
        if (_vertices[i] == vertex)
            return true;        
    }
    return false;
}

void EdgeNode::addToVertices()
{
    for (int i = 0; i < 2; i++)
    {
        if (data._vertices[i])
            data._vertices[i]->addEdge(this);
    }
}

void EdgeNode::removeFromVertices()
{
    for (int i = 0; i < 2; i++)
    {
        if (data._vertices[i])
        {
            data._vertices[i]->removeEdge(this);
            data._vertices[i] = NULL;
        }
    }
}

void EdgeNode::removeFromTriangles()
{
    for (int i = 0; i < 2; i++)
    {
        if (data._triangles[i])
        {
            data._triangles[i]->data.removeEdge(this);
            data._triangles[i] = NULL;
        }
    }    
}

void Edge2::removeVertex(VertexNode *vertex)
{
    for (int i = 0; i < 2; i++)
    {
        if (_vertices[i] == vertex)
        {
            _vertices[i] = NULL;
            break;
        }
    }
}

void Edge2::removeTriangle(TriangleNode *triangle)
{
    for (int i = 0; i < 2; i++)
    {
        if (_triangles[i] == triangle)
        {
            _triangles[i] = NULL;
            break;
        }
    }
}

void EdgeNode::replaceVertex(VertexNode *currentVertex, VertexNode *newVertex)
{
    for (int i = 0; i < 2; i++)
    {
        if (data._vertices[i] == currentVertex)
        {
            data._vertices[i] = newVertex;
            newVertex->addEdge(this);
            break;
        }
    }   
}

bool Edge2::isNotShared() const
{
    for (int i = 0; i < 2; i++)
    {
        if (_triangles[i] == NULL || !_triangles[i]->data.selected)
            return true;
    }
    return false;    
}

void Edge2::turn()
{
    if (_triangles[0] == NULL || _triangles[1] == NULL)
        return;
    
    Triangle2 &t0 = _triangles[0]->data;
    Triangle2 &t1 = _triangles[1]->data;
    
    VertexNode *v0 = t0.vertexNotInEdge(this);
    VertexNode *v1 = t1.vertexNotInEdge(this);
    
//    Vector3D t0_normal = t0.computeNormal();
//    Vector3D t1_normal = t1.computeNormal();
    
    _triangles[0]->removeFromVertices();
    _triangles[1]->removeFromVertices();
    
    t0.setVertex(0, v1);
    t0.setVertex(1, v0);
    t0.setVertex(2, _vertices[0]);
    
    _triangles[0]->addToVertices();
    
//    if (t0.computeNormal().Dot(t0_normal) < 0.0f)
//        t0.flip();
    
    t1.setVertex(0, v0);
    t1.setVertex(1, v1);
    t1.setVertex(2, _vertices[1]);
    
    _triangles[1]->addToVertices();
    
//    if (t1.computeNormal().Dot(t1_normal) < 0.0f)
//        t1.flip();
    
    _vertices[0] = v0;
    _vertices[1] = v1;
}

Vector3D NormalFromTriangleVertices(Vector3D triangleVertices[3])
{
	// now is same as RedBook (OpenGL Programming Guide)
	Vector3D u = triangleVertices[0] - triangleVertices[1];
	Vector3D v = triangleVertices[1] - triangleVertices[2];
	return u.Cross(v);
}

void AddTriangle(vector<Triangle> &triangles, int index1, int index2, int index3)
{
    Triangle triangle;
    
    triangle.vertexIndices[0] = index1;
    triangle.vertexIndices[1] = index2;
    triangle.vertexIndices[2] = index3;
    
    triangles.push_back(triangle);
}

void AddQuad(vector<Triangle> &triangles, int index1, int index2, int index3, int index4)
{
    Triangle triangle1, triangle2;
	triangle1.vertexIndices[0] = index1;
	triangle1.vertexIndices[1] = index2;
	triangle1.vertexIndices[2] = index3;
	
	triangle2.vertexIndices[0] = index1;
	triangle2.vertexIndices[1] = index3;
	triangle2.vertexIndices[2] = index4;
    
    triangles.push_back(triangle1);
    triangles.push_back(triangle2);
}

