//
//  Triangle.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/8/12.
//  For license see LICENSE.TXT
//

#include "MeshHelpers.h"

const uint Triangle2::twoTriIndices[6] = { 0, 1, 2, 0, 2, 3 };

Triangle2::Triangle2() :
    selected(false),
    visible(true),
    _isQuad(false),
    normalsAreValid(false)
{
    for (uint i = 0; i < count(); i++)
    {
        setVertex(i, NULL);
        setVertexEdge(i, NULL);
        setTexCoord(i, NULL);
        setTexCoordEdge(i, NULL);
    }
}

Triangle2::Triangle2(VertexNode *vertices[], bool isQuad) :
    selected(false),
    visible(true),
    _isQuad(isQuad),
    normalsAreValid(false)
{
    for (uint i = 0; i < count(); i++)
    {
        setVertex(i, vertices[i]);
        setVertexEdge(i, NULL);
        setTexCoord(i, NULL);
        setTexCoordEdge(i, NULL);
    }
}

Triangle2::Triangle2(VertexNode *vertices[], TexCoordNode *texCoords[], bool isQuad) :
    selected(false),
    visible(true),
    _isQuad(isQuad),
    normalsAreValid(false)
{
    for (uint i = 0; i < count(); i++)
    {
        setVertex(i, vertices[i]);
        setVertexEdge(i, NULL);
        setTexCoord(i, texCoords[i]);
        setTexCoordEdge(i, NULL);
    }
}

const Triangle2::PackedNode &Triangle2::node(uint index) const
{
    if (index < count())
        return _nodes[index];

    throw MeshMaker::IndexOutOfRangeException();
}

Triangle2::PackedNode &Triangle2::node(uint index)
{
    if (index < count())
        return _nodes[index];

	throw MeshMaker::IndexOutOfRangeException();
}

void TriangleNode::addToVertices()
{
    for (uint i = 0; i < data().count(); i++)
    {
        if (data().vertex(i))
            data().vertex(i)->addTriangle(this);
    }
}

void TriangleNode::addToTexCoords()
{
    for (uint i = 0; i < data().count(); i++)
    {
        if (data().texCoord(i))
            data().texCoord(i)->addTriangle(this);
    }
}

void TriangleNode::removeFromVertices()
{
    for (uint i = 0; i < data().count(); i++)
    {
        if (data().vertex(i))
        {
            data().vertex(i)->removeTriangle(this);
            data().setVertex(i, NULL);
        }
    }
}

void TriangleNode::removeFromTexCoords()
{
    for (uint i = 0; i < data().count(); i++)
    {
        if (data().texCoord(i))
        {
            data().texCoord(i)->removeTriangle(this);
            data().setTexCoord(i, NULL);
        }
    }
}

void TriangleNode::removeFromEdges()
{
    for (uint i = 0; i < data().count(); i++)
    {
        if (data().vertexEdge(i))
        {
            data().vertexEdge(i)->data().removeTriangle(this);
            data().setVertexEdge(i, NULL);
        }
        
        if (data().texCoordEdge(i))
        {
            data().texCoordEdge(i)->data().removeTriangle(this);
            data().setTexCoordEdge(i, NULL);
        }
    }
}

void TriangleNode::replaceVertex(VertexNode *currentVertex, VertexNode *newVertex)
{
    for (uint i = 0; i < data().count(); i++)
    {
        if (data().vertex(i) == currentVertex)
        {
            data().setVertex(i, newVertex);
            newVertex->addTriangle(this);
            break;
        }
    }
}

void TriangleNode::replaceTexCoord(TexCoordNode *currentTexCoord, TexCoordNode *newTexCoord)
{
    for (uint i = 0; i < data().count(); i++)
    {
        if (data().texCoord(i) == currentTexCoord)
        {
            data().setTexCoord(i, newTexCoord);
            newTexCoord->addTriangle(this);
            break;
        }
    }
}

void Triangle2::setTexCoordByVertex(TexCoordNode *texCoord, VertexNode *vertex)
{
    for (uint i = 0; i < count(); i++)
    { 
        if (this->vertex(i) == vertex)
        {
            setTexCoord(i, texCoord);
            break;
        }
    }
}

void Triangle2::removeVertex(VertexNode *vertex)
{
    for (uint i = 0; i < count(); i++)
    {
        if (this->vertex(i) == vertex)
        {
            setVertex(i, NULL);
            break;
        }
    }
}

void Triangle2::removeTexCoord(TexCoordNode *texCoord)
{
    for (uint i = 0; i < count(); i++)
    {
        if (this->texCoord(i) == texCoord)
        {
            this->setTexCoord(i, NULL);
            break;
        }
    }
}

void Triangle2::removeVertexEdge(VertexEdgeNode *edge)
{
    for (uint i = 0; i < count(); i++)
    {
        if (vertexEdge(i) == edge)
        {
            setVertexEdge(i, NULL);
            break;
        }
    }
}

void Triangle2::removeTexCoordEdge(TexCoordEdgeNode *edge)
{
    for (uint i = 0; i < count(); i++)
    {
        if (texCoordEdge(i) == edge)
        {
            setTexCoordEdge(i, NULL);
            break;
        }
    }
}

void Triangle2::removeEdges()
{
    for (uint i = 0; i < count(); i++)
    {
        setVertexEdge(i, NULL);
        setTexCoordEdge(i, NULL);
    }
}

bool Triangle2::isDegeneratedAfterCollapseToTriangle()
{
    if (containsVertex(NULL))
        return true;
    
    if (containsVertexEdge(NULL))
        return true;
    
    for (uint i = 0; i < count() - 1; i++)
    {
        for (uint j = i + 1; j < count(); j++)
        {
            if (vertex(i) == vertex(j))
            {
                if (_isQuad)
                {
                    if (i < 3 && j < 3)
                        swap(node(i), node(3));
                    
                    node(3) = PackedNode();
                    _isQuad = false;
                    
                    return isDegeneratedAfterCollapseToTriangle();
                }
                return true;
            }
        }
    }
    	
	return false;
}

bool Triangle2::containsVertex(const VertexNode *vertex) const
{
    for (uint i = 0; i < count(); i++)
	{
		if (this->vertex(i) == vertex)
			return true;
	}
	return false;
}

bool Triangle2::containsVertexEdge(const VertexEdgeNode *edge) const
{
    for (uint i = 0; i < count(); i++)
    {
        if (vertexEdge(i) == edge)
            return true;
    }
    return false;
}

bool Triangle2::containsTexCoordEdge(const TexCoordEdgeNode *edge) const
{
    for (uint i = 0; i < count(); i++)
    {
        if (texCoordEdge(i) == edge)
            return true;
    }
    return false;
}

void Triangle2::flip()
{
    swap(_nodes[0].vertex, _nodes[2].vertex);
    swap(_nodes[0].texCoord, _nodes[2].texCoord);
}

uint Triangle2::indexOfVertex(const VertexNode *vertex) const
{
    for (uint i = 0; i < count(); i++)
    {
        if (this->vertex(i) == vertex)
            return i;
    }
    return UINT_MAX;
}

bool Triangle2::shouldSwapVertices(const VertexNode *v1, const VertexNode *v2) const
{
    uint index1 = indexOfVertex(v1);
    uint index2 = indexOfVertex(v2);
    
    if (index1 - 1 == index2)
        return true;
    
    if (index1 == 0 && index2 == count() - 1)
        return true;
    
    return false;
}

VertexEdgeNode *Triangle2::findSecondSplitEdgeNode(const VertexEdge &firstEdge) const
{
    for (uint i = 0; i < count(); i++)
    {
        VertexEdgeNode *secondEdgeNode = vertexEdge(i);
        VertexEdge &secondEdge = secondEdgeNode->data();
        if (firstEdge.half != secondEdge.half && secondEdge.selected)
        {
            if (isQuad() && secondEdgeNode->data().sharedVertex(firstEdge) != NULL)
            {
                return NULL;
            }
            
            return secondEdgeNode;
        }
    }
    return NULL;
}

VertexEdgeNode *Triangle2::nextEdgeInQuadLoop(const VertexEdge &edge) const
{
    if (!isQuad())
        return NULL;
    
    for (uint i = 0; i < count(); i++)
    {
        VertexEdgeNode *edgeNode = vertexEdge(i);
        VertexNode *sharedVertex = edgeNode->data().sharedVertex(edge);
        
        if (!sharedVertex)
            return edgeNode;
    }
    
    return NULL;
}

VertexNode *Triangle2::vertexNotInEdge(const VertexEdge *edge) const
{
    for (uint i = 0; i < count(); i++)
    {
        VertexNode *currentVertex = vertex(i);
        if (currentVertex != edge->vertex(0) &&
            currentVertex != edge->vertex(1))
            return currentVertex;
    }
    return NULL;
}

TexCoordNode *Triangle2::texCoordNotInEdge(const TexCoordEdge *edge) const
{
    for (uint i = 0; i < count(); i++)
    {
        TexCoordNode *currentTexCoord = texCoord(i);
        if (currentTexCoord != edge->texCoord(0) &&
            currentTexCoord != edge->texCoord(1))
            return currentTexCoord;
    }
    return NULL;
}

void Triangle2::computeNormalsIfNeeded()
{
    // TODO: Make it work for polygon (tri/quad).
    if (normalsAreValid)
        return;
    
    Vector3D u, v;
    
    u = texCoord(0)->data().position - texCoord(1)->data().position;
    v = texCoord(1)->data().position - texCoord(2)->data().position;
    texCoordNormal = u.Cross(v);    
    
    u = vertex(0)->data().position - vertex(1)->data().position;
    v = vertex(1)->data().position - vertex(2)->data().position;
    vertexNormal = u.Cross(v);
    
    normalsAreValid = true;
}

bool Triangle2::rayIntersect(const Vector3D &origin, const Vector3D &direction, float &u, float &v, Vector3D &intersect)
{
    // TODO: Make it work for polygon (tri/quad).
    
    Vector3D v0 = vertex(0)->data().position;
    Vector3D v1 = vertex(1)->data().position;
    Vector3D v2 = vertex(2)->data().position;
    
    Vector3D e1 = v1 - v0;
    Vector3D e2 = v2 - v0;
    Vector3D p = direction.Cross(e2);
    float a = e1.Dot(p);
    
    if (fabsf(a) < FLOAT_EPS)
        return false;
    
    float f = 1.0f / a;
    
    Vector3D s = origin - v0;
    u = f * s.Dot(p);
    if (u < 0.0f || u > 1.0f)
        return false;
    
    Vector3D q = s.Cross(e1);
    v = f * direction.Dot(q);
    if (v < 0.0f || u + v > 1.0f)
        return false;
    
    float t = f * e2.Dot(q);
    if (t >= 0.0f)
    {
        intersect = v0 + e1 * u + e2 * v;
        return true;
    }
    return false;
}

void Triangle2::convertBarycentricToUVs(float &u, float &v)
{
    // TODO: Make it work for polygon (tri/quad).
    
    Vector3D t0 = texCoord(0)->data().position;
    Vector3D t1 = texCoord(1)->data().position;
    Vector3D t2 = texCoord(2)->data().position;
    
    Vector3D e1 = t1 - t0;
    Vector3D e2 = t2 - t0;
   
    Vector3D final = t0 + e1 * u + e2 * v;
    
    u = final.x;
    v = final.y;
}

void TriangleNode::softSelect(const float weights[], uint weightCount)
{
    selectionWeight = 1.0f;
    
    vector<TriangleNode *> currentStepNodes;
    currentStepNodes.push_back(this);
    
    vector<TriangleNode *> nextStepNodes;
    
    for (uint w = 0; w < weightCount; w++)
    {
        float weight = weights[w];
        
        nextStepNodes.clear();
        
        for (uint i = 0; i < currentStepNodes.size(); i++)
        {
            TriangleNode *currentNode = currentStepNodes[i];
            Triangle2 &triangle = currentNode->data();
            
            for (uint j = 0; j < triangle.count(); j++)
            {
                TriangleNode *oppositeNode = triangle.vertexEdge(j)->data().opposite(currentNode);
                
                if (oppositeNode != NULL)
                {
                    nextStepNodes.push_back(oppositeNode);
                    if (oppositeNode->selectionWeight < weight)
                        oppositeNode->selectionWeight = weight;
                }
            }
        }
        
        currentStepNodes = nextStepNodes;
    }
}
