//
//  Mesh2.drawing.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  For license see LICENSE.TXT
//

#include "Mesh2.h"

void Mesh2::resetTriangleCache()
{
    _cachedTriangleVertices.setValid(false);
    resetEdgeCache();
}

void Mesh2::resetEdgeCache()
{
    _cachedEdgeVertices.setValid(false);
    _cachedEdgeTexCoords.setValid(false);
}

void Mesh2::fillTriangleCache()
{
    if (_cachedTriangleVertices.isValid())
        return;
    
    _cachedTriangleVertices.resize(_triangles.count() * 6);
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle2 &currentTriangle = node->data();
        currentTriangle.normalsAreValid = false;
        currentTriangle.computeNormalsIfNeeded();
    }
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        node->resetCacheIndices();
        node->computeNormal();
    }
    
    for (TexCoordNode *node = _texCoords.begin(), *end = _texCoords.end(); node != end; node = node->next())
    {
        node->computeNormal();
    }

    const float selectedComponents[] = { 0.7f, 0.0f, 0.0f };
    const uint *twoTriIndices = Triangle2::twoTriIndices;
    uint i = 0;
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle2 &currentTriangle = node->data();

        if (!currentTriangle.visible)
            continue;
        
        const float *c = currentTriangle.selected ? selectedComponents : _colorComponents;
        
        Vector3D fn = _isUnwrapped ? currentTriangle.texCoordNormal : currentTriangle.vertexNormal;
        
        uint vertexCount = currentTriangle.isQuad() ? 6 : 3;
        
        for (uint j = 0; j < vertexCount; j++)
        {
            uint twoTriIndex = twoTriIndices[j];
            VertexNode *vertex = currentTriangle.vertex(twoTriIndex);
            TexCoordNode *texCoord = currentTriangle.texCoord(twoTriIndex);

            const Vector3D &v = vertex->data().position;
            const Vector3D &t = texCoord->data().position;
            
            const Vector3D &sn = _isUnwrapped ? texCoord->algorithmData.normal : vertex->algorithmData.normal;;
            
            GLTriangleVertex &cachedVertex = _cachedTriangleVertices[i];
            vertex->setCacheIndexForTriangleNode(node, i, j < 3 ? 0 : 1);
            
            for (uint k = 0; k < 3; k++)
            {
                cachedVertex.position.coords[k] = v[k];
                cachedVertex.texCoord.coords[k] = t[k];
                cachedVertex.flatNormal.coords[k] = fn[k];
                cachedVertex.smoothNormal.coords[k] = sn[k];
                cachedVertex.color.coords[k] = c[k];
            }
            
            i++;
        }
    }
    
    _cachedTriangleVertices.resize(i);
    _cachedTriangleVertices.setValid(true);

#if defined(__APPLE__) || defined(SHADERS)
    if (!_vboGenerated)
    {
        glGenBuffers(1, &_vboID);
        _vboGenerated = true;
    }

    glBindBuffer(GL_ARRAY_BUFFER, _vboID);
    glBufferData(GL_ARRAY_BUFFER, _cachedTriangleVertices.count() * sizeof(GLTriangleVertex), _cachedTriangleVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void Mesh2::fillEdgeCache()
{
    if (_cachedEdgeVertices.isValid() && _cachedEdgeTexCoords.isValid())
        return;
    
    _cachedEdgeVertices.resize(_vertexEdges.count() * 2);
    _cachedEdgeTexCoords.resize(_texCoordEdges.count() * 2);
    
    Vector3D selectedColor(0.8f, 0.0f, 0.0f);
    Vector3D normalColor(_colorComponents[0] - 0.2f, _colorComponents[1] - 0.2f, _colorComponents[2] - 0.2f);
    
    uint i = 0;
    
    for (VertexEdgeNode *node = _vertexEdges.begin(), *end = _vertexEdges.end(); node != end; node = node->next())
    {
        if (!node->data().visible)
            continue;
        
        if (node->data().selected)
        {
            for (uint k = 0; k < 3; k++)
            {
                _cachedEdgeVertices[i].color.coords[k] = selectedColor[k];
                _cachedEdgeVertices[i + 1].color.coords[k] = selectedColor[k];
            }
        }
        else
        {
            for (uint k = 0; k < 3; k++)
            {
                _cachedEdgeVertices[i].color.coords[k] = normalColor[k];
                _cachedEdgeVertices[i + 1].color.coords[k] = normalColor[k];
            }
        }
        
        VertexNode *v0 = node->data().vertex(0);
        VertexNode *v1 = node->data().vertex(1);
        
        v0->setCacheIndexForEdgeNode(node, i);
        v1->setCacheIndexForEdgeNode(node, i + 1);
        
        for (uint k = 0; k < 3; k++)
        {
            _cachedEdgeVertices[i].position.coords[k] = v0->data().position[k];
            _cachedEdgeVertices[i + 1].position.coords[k] = v1->data().position[k];
        }
        
        i += 2;
    }
    
    _cachedEdgeVertices.resize(i); // resize doesn't delete [] internal array, if not needed
    _cachedEdgeVertices.setValid(true);
    
    i = 0;
    
    for (TexCoordEdgeNode *node = _texCoordEdges.begin(), *end = _texCoordEdges.end(); node != end; node = node->next())
    {
        if (!node->data().visible)
            continue;
        
        if (node->data().selected)
        {
            for (uint k = 0; k < 3; k++)
            {
                _cachedEdgeTexCoords[i].color.coords[k] = selectedColor[k];
                _cachedEdgeTexCoords[i + 1].color.coords[k] = selectedColor[k];
            }
        }
        else
        {
            for (uint k = 0; k < 3; k++)
            {
                _cachedEdgeTexCoords[i].color.coords[k] = normalColor[k];
                _cachedEdgeTexCoords[i + 1].color.coords[k] = normalColor[k];
            }
        }
        
        for (uint k = 0; k < 3; k++)
        {
            _cachedEdgeTexCoords[i].position.coords[k] = node->data().texCoord(0)->data().position[k];
            _cachedEdgeTexCoords[i + 1].position.coords[k] = node->data().texCoord(1)->data().position[k];
        }
        
        i += 2;
    }
    
    _cachedEdgeTexCoords.resize(i); // resize doesn't delete [] internal array, if not needed
    _cachedEdgeTexCoords.setValid(true);
}

void Mesh2::updateVertexInTriangleCache(VertexNode *vertexNode, VertexTriangleNode *triangleNode, uint cacheIndexPosition)
{
    int cacheIndex = triangleNode->cacheIndices[cacheIndexPosition];
    if (cacheIndex < 0)
        return;
    
    const Vector3D &v = vertexNode->data().position;
	const Vector3D &sn = vertexNode->algorithmData.normal;
    const Vector3D &fn = triangleNode->data()->data().vertexNormal;
    
    GLTriangleVertex &cachedVertex = _cachedTriangleVertices[cacheIndex];
    
    for (uint k = 0; k < 3; k++)
    {
        cachedVertex.position.coords[k] = v[k];
        cachedVertex.smoothNormal.coords[k] = sn[k];
        cachedVertex.flatNormal.coords[k] = fn[k];
    }
}

void Mesh2::updateVertexInEdgeCache(VertexNode *vertexNode, Vertex2VEdgeNode *edgeNode)
{
    int cacheIndex = edgeNode->cacheIndex;
    if (cacheIndex < 0)
        return;
    
    const Vector3D &v = vertexNode->data().position;
    
    GLEdgeVertex &cachedVertex = _cachedEdgeVertices[cacheIndex];
    
    for (uint k = 0; k < 3; k++)
    {
        cachedVertex.position.coords[k] = v[k];
    }
}

void Mesh2::updateTriangleAndEdgeCache(vector<VertexNode *> &affectedVertices)
{
    uint count = affectedVertices.size();
    
    if (count > vertexCount() / 3)
    {
        resetTriangleCache();
        return;
    }
    
    for (uint i = 0; i < count; i++)
    {
        VertexNode *vertexNode = affectedVertices[i];
        vertexNode->addAffectedVertices(affectedVertices);
    }
    
    count = affectedVertices.size();
    
    for (uint i = 0; i < count; i++)
    {
        VertexNode *vertexNode = affectedVertices[i];
        vertexNode->updateTriangleNormals();
    }
    
    for (uint i = 0; i < count; i++)
    {
        VertexNode *vertexNode = affectedVertices[i];
        vertexNode->computeNormal();
        
        for (VertexTriangleNode
             *triangleNode = vertexNode->_triangles.begin(),
             *triangleEnd = vertexNode->_triangles.end();
             triangleNode != triangleEnd;
             triangleNode = triangleNode->next())
        {
            updateVertexInTriangleCache(vertexNode, triangleNode, 0);
            if (triangleNode->data()->data().isQuad())
                updateVertexInTriangleCache(vertexNode, triangleNode, 1);
        }
        
        for (Vertex2VEdgeNode
             *edgeNode = vertexNode->_edges.begin(),
             *edgeEnd = vertexNode->_edges.end();
             edgeNode != edgeEnd;
             edgeNode = edgeNode->next())
        {
            updateVertexInEdgeCache(vertexNode, edgeNode);
        }
    }
#if defined(__APPLE__) || defined(SHADERS)
    glBindBuffer(GL_ARRAY_BUFFER, _vboID);
    glBufferData(GL_ARRAY_BUFFER, _cachedTriangleVertices.count() * sizeof(GLTriangleVertex), _cachedTriangleVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
}

void Mesh2::drawFill(FillMode fillMode, ViewMode viewMode)
{
    fillTriangleCache();
#if defined(__APPLE__) || defined(SHADERS)
	if (viewMode == ViewMode::MixedWireSolid)
        glEnable(GL_BLEND);
    
    glBindBuffer(GL_ARRAY_BUFFER, _vboID);
    
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

    if (fillMode.colored)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, color));
    }
    
    if (viewMode == ViewMode::SolidSmooth)
        glNormalPointer(GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, smoothNormal));
    else
        glNormalPointer(GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, flatNormal));
    
    if (viewMode == ViewMode::Unwrap)
        glVertexPointer(3, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, texCoord));
    else
        glVertexPointer(3, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, position));

    glDrawArrays(GL_TRIANGLES, 0, (int)_cachedTriangleVertices.count());

    if (fillMode.colored)
        glDisableClientState(GL_COLOR_ARRAY);
    
    
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (viewMode == ViewMode::MixedWireSolid)
        glDisable(GL_BLEND);
#else
	if (!fillMode.colored && !fillMode.textured)
	{
		glBegin(GL_TRIANGLES);
		for (uint i = 0; i < _cachedTriangleVertices.count(); i++)
		{
			glVertex3fv(_cachedTriangleVertices[i].position.coords);
		}
		glEnd();
	}
	else
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		glBegin(GL_TRIANGLES);
		for (uint i = 0; i < _cachedTriangleVertices.count(); i++)
		{
			glColor3fv(_cachedTriangleVertices[i].color.coords);
			glNormal3fv(_cachedTriangleVertices[i].smoothNormal.coords);
			glVertex3fv(_cachedTriangleVertices[i].position.coords);
		}
		glEnd();

		glDisable(GL_LIGHTING);
	}
#endif
}

void Mesh2::draw(ViewMode viewMode, const Vector3D &scale, bool selected, bool forSelection)
{
    bool flipped = scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f;
#if defined(__APPLE__) || defined(SHADERS)
    ShaderProgram *shader = ShaderProgram::normalShader();
#endif    
    if (viewMode == ViewMode::MixedWireSolid)
    {
        selected = true;
        glEnable(GL_CULL_FACE);
        if (flipped)
            glCullFace(GL_BACK);
        else
            glCullFace(GL_FRONT);
    }   
	
    FillMode fillMode;
	
	glPushMatrix();
	glScalef(scale.x, scale.y, scale.z);
	if (viewMode == ViewMode::Wireframe)
	{
        drawAllEdges(viewMode, forSelection);
	}
	else if (forSelection)
	{
        fillMode.textured = false;
        fillMode.colored = false;
        drawFill(fillMode, viewMode);
    }
    else
    {
        if (selected)
        {
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(1.0f, 1.0f);

        }
#if defined(__APPLE__) || defined(SHADERS)
        shader->useProgram();
#endif        
        if (_selectionMode != MeshSelectionMode::Triangles)
        {
            glColor3fv(_colorComponents);
            fillMode.textured = true;
            fillMode.colored = false;
            drawFill(fillMode, viewMode);
        }
        else
        {
            fillMode.textured = true;
            fillMode.colored = true;
            drawFill(fillMode, viewMode);
        }
#if defined(__APPLE__) || defined(SHADERS)
        ShaderProgram::resetProgram();
#endif        
        if (selected)
        {
            glDisable(GL_POLYGON_OFFSET_FILL);
            drawAllEdges(viewMode, forSelection);
        }        
	}
	glPopMatrix();
    
    if (viewMode == ViewMode::MixedWireSolid)
    {
        glDisable(GL_CULL_FACE);
    }
}

void Mesh2::drawAtIndex(uint index, bool forSelection, ViewMode viewMode)
{
    switch (_selectionMode) 
	{
		case MeshSelectionMode::Vertices:
		{
            bool selected;
            Vector3D v;
            
            if (_isUnwrapped)
            {
                const TexCoord &texCoord = _cachedTexCoordSelection.at(index)->data();
                
                if (!texCoord.visible)
                    return;
                
                selected = texCoord.selected;
                v = texCoord.position;
            }
            else
            {
                const Vertex2 &vertex = _cachedVertexSelection.at(index)->data();
                
                if (!vertex.visible)
                    return;
                
                selected = vertex.selected;
                v = vertex.position;
            }
            
			if (!forSelection)
			{
				glPointSize(4.0f);
				if (selected)
					glColor3f(1.0f, 0.0f, 0.0f);
				else
					glColor3f(0.0f, 0.0f, 1.0f);
			}
			glBegin(GL_POINTS);
			glVertex3f(v.x, v.y, v.z);
			glEnd();
		} break;
		case MeshSelectionMode::Triangles:
		{
			if (forSelection)
			{
				const Triangle2 &triangle = _cachedTriangleSelection.at(index)->data();
                
                if (!triangle.visible)
                    return;
                
				glBegin(triangle.isQuad() ? GL_QUADS : GL_TRIANGLES);
                if (_isUnwrapped)
                {
                    for (uint i = 0; i < triangle.count(); i++)
                    {
                        Vector3D v = triangle.texCoord(i)->data().position;
                        glVertex3f(v.x, v.y, v.z);
                    }
                }
                else 
                {
                    for (uint i = 0; i < triangle.count(); i++)
                    {
                        Vector3D v = triangle.vertex(i)->data().position;
                        glVertex3f(v.x, v.y, v.z);
                    }
                }
				glEnd();
			}
		} break;
        case MeshSelectionMode::Edges:
        {
            if (_isUnwrapped)
            {
                const TexCoordEdge &edge = _cachedTexCoordEdgeSelection.at(index)->data();
                
                if (!edge.visible)
                    return;
                
                if (!forSelection)
                {
                    bool isSelected = edge.selected;
                    if (isSelected)
                        glColor3f(0.8f, 0.0f, 0.0f);
                    else
                        glColor3f(_colorComponents[0] - 0.2f, _colorComponents[1] - 0.2f, _colorComponents[2] - 0.2f);
                }
                glBegin(GL_LINES);
                for (uint i = 0; i < 2; i++)
                {
                    Vector3D v = edge.texCoord(i)->data().position;
                    glVertex3f(v.x, v.y, v.z);
                }
                glEnd();
            }
            else 
            {    
                const VertexEdge &edge = _cachedVertexEdgeSelection.at(index)->data();
                
                if (!edge.visible)
                    return;
                
                if (!forSelection)
                {
                    bool isSelected = edge.selected;
                    if (isSelected)
                        glColor3f(0.8f, 0.0f, 0.0f);
                    else
                        glColor3f(_colorComponents[0] - 0.2f, _colorComponents[1] - 0.2f, _colorComponents[2] - 0.2f);
                }
                glBegin(GL_LINES);
                for (uint i = 0; i < 2; i++)
                {
                    Vector3D v = edge.vertex(i)->data().position;
                    glVertex3f(v.x, v.y, v.z);
                }
                glEnd();
            }
        } break;
	}
}

void Mesh2::drawAllVertices(ViewMode viewMode, bool forSelection)
{
    glPointSize(4.0f);
    
    if (forSelection)
    {
        vector<Vector3D> tempVertices;
        vector<uint> tempColors;
        
        uint colorIndex = 0;
        
        if (!_selectThrough && !_isUnwrapped)
        {
            glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);
            glColor4ubv((GLubyte *)&colorIndex);
            FillMode fillMode;
            fillMode.textured = false;
            fillMode.colored = false;
            drawFill(fillMode, viewMode);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
        
        if (_isUnwrapped)
        {
            for (TexCoordNode *node = _texCoords.begin(), *end = _texCoords.end(); node != end; node = node->next())
            {
                colorIndex++;
                
                if (!node->data().visible)
                    continue;
                
                tempColors.push_back(colorIndex);
                tempVertices.push_back(node->data().position);
            }
        }
        else
        {
            for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
            {
                colorIndex++;
                
                if (!node->data().visible)
                    continue;
                
                tempColors.push_back(colorIndex);            
                tempVertices.push_back(node->data().position);            
            }
        }
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        GLubyte *colorPtr = (GLubyte *)&tempColors[0];
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorPtr);
        
        float *vertexPtr = (float *)&tempVertices[0];        
        glVertexPointer(3, GL_FLOAT, 0, vertexPtr);
        
        glDrawArrays(GL_POINTS, 0, tempVertices.size());
        
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
        vector<Vector3D> tempVertices;
        vector<Vector3D> tempColors;
        
        Vector3D selectedColor(1.0f, 0.0f, 0.0f);
        Vector3D normalColor(0.0f, 0.0f, 1.0f);
        
        if (_isUnwrapped)
        {
            for (TexCoordNode *node = _texCoords.begin(), *end = _texCoords.end(); node != end; node = node->next())
            {
                if (!node->data().visible)
                    continue;
                
                if (node->data().selected)
                    tempColors.push_back(selectedColor);
                else
                    tempColors.push_back(normalColor);
                
                tempVertices.push_back(node->data().position);
            }
        }
        else
        {
            for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
            {
                if (!node->data().visible)
                    continue;
                
                if (node->data().selected)
                    tempColors.push_back(selectedColor); 
                else if (_useSoftSelection && node->selectionWeight > 0.0f)
                    tempColors.push_back(Vector3D(1.0f, 1.0f - node->selectionWeight, 0.0f));
                else
                    tempColors.push_back(normalColor);
                
                tempVertices.push_back(node->data().position);            
            }
        }
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        float *colorPtr = (float *)&tempColors[0];
        glColorPointer(3, GL_FLOAT, 0, colorPtr);
        
        float *vertexPtr = (float *)&tempVertices[0];        
        glVertexPointer(3, GL_FLOAT, 0, vertexPtr);
        
        glDrawArrays(GL_POINTS, 0, tempVertices.size());
        
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void Mesh2::glProjectSelect(int x, int y, int width, int height, const Matrix4x4 &transform, OpenGLSelectionMode selectionMode)
{
    int viewport[4];
    double modelview[16];
    double projection[16];
    double winX, winY, winZ;
    double posX, posY, posZ;
	
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        if (!node->data().visible)
            continue;

        Vector3D position = transform.Transform(node->data().position);

        posX = position.x;
        posY = position.y;
        posZ = position.z;
        
        gluProject(posX, posY, posZ, modelview, projection, viewport, &winX, &winY, &winZ);
        
        if (winX > x && winX < x + width &&
            winY > y && winY < y + height)
        {
            switch (selectionMode) 
            {
                case OpenGLSelectionMode::Add:
                    node->data().selected = true;
                    break;
                case OpenGLSelectionMode::Subtract:
                    node->data().selected = false;
                    break;
                case OpenGLSelectionMode::Invert:
                    node->data().selected = !node->data().selected;
                    break;
                default:
                    break;
            }
        }
    }
}

void Mesh2::drawAllTriangles(ViewMode viewMode, bool forSelection)
{
    for (uint i = 0; i < _triangles.count(); i++)
    {
        uint colorIndex = i + 1;
        glColor4ubv((GLubyte *)&colorIndex);
        drawAtIndex(i, forSelection, viewMode);
    }
}

void Mesh2::drawAllEdges(ViewMode viewMode, bool forSelection)
{
    fillEdgeCache();
    
    if (forSelection)
    {
        vector<uint> tempColors;
        
        uint colorIndex = 0;
        
        if (!_selectThrough && !_isUnwrapped)
        {
            glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);
            glColor4ubv((GLubyte *)&colorIndex);
            FillMode fillMode;
            fillMode.textured = false;
            fillMode.colored = false;
            drawFill(fillMode, viewMode);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
        
        if (_isUnwrapped)
        {
            for (TexCoordEdgeNode *node = _texCoordEdges.begin(), *end = _texCoordEdges.end(); node != end; node = node->next())
            {
                if (!node->data().visible)
                {
                    colorIndex++;
                    continue;
                }
                
                colorIndex++;
                tempColors.push_back(colorIndex);
                tempColors.push_back(colorIndex);
            }
        }
        else
        {
            for (VertexEdgeNode *node = _vertexEdges.begin(), *end = _vertexEdges.end(); node != end; node = node->next())
            {
                if (!node->data().visible)
                {
                    colorIndex++;
                    continue;
                }
                
                colorIndex++;
                tempColors.push_back(colorIndex);
                tempColors.push_back(colorIndex);
            }
        }
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        GLubyte *colorPtr = (GLubyte *)&tempColors[0];
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorPtr);
        
        if (_isUnwrapped)
        {
            float *vertexPtr = (float *)&_cachedEdgeTexCoords[0].position;
            glVertexPointer(3, GL_FLOAT, sizeof(GLEdgeTexCoord), vertexPtr);
            
            glDrawArrays(GL_LINES, 0, (int)_cachedEdgeTexCoords.count());
        }
        else
        {
            float *vertexPtr = (float *)&_cachedEdgeVertices[0].position;
            glVertexPointer(3, GL_FLOAT, sizeof(GLEdgeVertex), vertexPtr);
            
            glDrawArrays(GL_LINES, 0, (int)_cachedEdgeVertices.count());
        }
        
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        if (_isUnwrapped)
        {
            float *colorPtr = (float *)&_cachedEdgeTexCoords[0].color;
            glColorPointer(3, GL_FLOAT, sizeof(GLEdgeTexCoord), colorPtr);
            
            float *vertexPtr = (float *)&_cachedEdgeTexCoords[0].position;
            glVertexPointer(3, GL_FLOAT, sizeof(GLEdgeTexCoord), vertexPtr);
            
            glDrawArrays(GL_LINES, 0, (int)_cachedEdgeTexCoords.count());
        }
        else
        {
            float *colorPtr = (float *)&_cachedEdgeVertices[0].color;
            glColorPointer(3, GL_FLOAT, sizeof(GLEdgeVertex), colorPtr);
            
            float *vertexPtr = (float *)&_cachedEdgeVertices[0].position;
            glVertexPointer(3, GL_FLOAT, sizeof(GLEdgeVertex), vertexPtr);
            
            glDrawArrays(GL_LINES, 0, (int)_cachedEdgeVertices.count());
        }
        
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void Mesh2::drawAll(ViewMode viewMode, bool forSelection)
{
    switch (_selectionMode) 
	{
        case MeshSelectionMode::Vertices:
            drawAllVertices(viewMode, forSelection);
            break;
        case MeshSelectionMode::Triangles:
            drawAllTriangles(viewMode, forSelection);
            break;
        case MeshSelectionMode::Edges:
            drawAllEdges(viewMode, forSelection);
            break;
    }
}

void Mesh2::uvToPixels(float &u, float &v)
{
//    u *= (float)_texture.width;
//    v *= (float)_texture.height;
//    
//    v = (float)_texture.height - v;
}

TriangleNode *Mesh2::rayToUV(const Vector3D &origin, const Vector3D &direction, float &u, float &v)
{
    Vector3D intersect = Vector3D();
    TriangleNode *nearest = NULL;
    u = 0.0f;
    v = 0.0f;
    
    float lastSqDistance = 0.0f;
    float tempSqDistance = 0.0f;
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        float tempU = 0.0f;
        float tempV = 0.0f;
        Vector3D tempIntersect;
        
        if (node->data().rayIntersect(origin, direction, tempU, tempV, tempIntersect))
        {
            tempSqDistance = tempIntersect.SqDistance(origin);
            if (nearest == NULL || lastSqDistance > tempSqDistance)
            {
                nearest = node;
                u = tempU;
                v = tempV;
                intersect = tempIntersect;
                lastSqDistance = tempSqDistance;
            }            
        }
    }

    if (nearest)
    {
        nearest->data().convertBarycentricToUVs(u, v);
        uvToPixels(u, v);
    }
    return nearest;
}

void Mesh2::hideSelected()
{
    resetTriangleCache();
    
    switch (_selectionMode)
    {
        case MeshSelectionMode::Triangles:
            
            for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
            {
                if (!node->data().visible)
                    node->data().selected = true;
            }            
            
            for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
            {
                Triangle2 &tri = node->data();
                
                if (tri.selected)
                {
                    tri.visible = false;
                    
                    for (uint i = 0; i < tri.count(); i++)
                    {
                        if (!tri.vertexEdge(i)->data().isNotShared())
                            tri.vertexEdge(i)->data().visible = false;
                        
                        if (!tri.texCoordEdge(i)->data().isNotShared())
                            tri.texCoordEdge(i)->data().visible = false;
                    }
                    
                    tri.selected = false;
                }
            }
            
            break;
        case MeshSelectionMode::Edges:
            if (_isUnwrapped)
            {
                for (TexCoordEdgeNode *node = _texCoordEdges.begin(), *end = _texCoordEdges.end(); node != end; node = node->next())
                {
                    if (node->data().selected)
                    {
                        node->data().visible = false;
                        node->data().selected = false;
                    }
                }
            }
            else
            {
                for (VertexEdgeNode *node = _vertexEdges.begin(), *end = _vertexEdges.end(); node != end; node = node->next())
                {
                    if (node->data().selected)
                    {
                        node->data().visible = false;
                        node->data().selected = false;
                    }
                }
            }
            break;
        case MeshSelectionMode::Vertices:
            if (_isUnwrapped)
            {
                for (TexCoordNode *node = _texCoords.begin(), *end = _texCoords.end(); node != end; node = node->next())
                {
                    if (node->data().selected)
                    {
                        node->data().visible = false;
                        node->data().selected = false;
                    }
                } 
            }
            else
            {
                for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
                {
                    if (node->data().selected)
                    {
                        node->data().visible = false;
                        node->data().selected = false;
                    }
                } 
            }
            break;            
        default:
            break;
    }
    
    setSelectionMode(_selectionMode);
}

void Mesh2::unhideAll()
{
    resetTriangleCache();
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
        node->data().visible = true;

    for (TexCoordEdgeNode *node = _texCoordEdges.begin(), *end = _texCoordEdges.end(); node != end; node = node->next())
        node->data().visible = true;
    
    for (VertexEdgeNode *node = _vertexEdges.begin(), *end = _vertexEdges.end(); node != end; node = node->next())
        node->data().visible = true;
    
    for (TexCoordNode *node = _texCoords.begin(), *end = _texCoords.end(); node != end; node = node->next())
        node->data().visible = true;
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
        node->data().visible = true;
    
    setSelectionMode(_selectionMode);
}

