//
//  Mesh2.drawing.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  For license see LICENSE.TXT
//

#import "Mesh2.h"

void fillCachedColorsAtIndex(uint index, GLTriangleVertex *cachedColors, float components[3]);

void fillCachedColorsAtIndex(uint index, GLTriangleVertex *cachedColors, float components[3])
{
    for (uint j = 0; j < 3; j++)
    {
        for (uint k = 0; k < 3; k++)
        {
            cachedColors[index * 3 + j].color.coords[k] = components[k];
        }
    }	
}

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
    
    _cachedTriangleVertices.resize(_triangles.count() * 3);
    
    int i = 0;
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle2 &currentTriangle = node->data();
        
        currentTriangle.computeNormal();
        Vector3D n = _isUnwrapped ? currentTriangle.texCoordNormal : currentTriangle.vertexNormal;
        
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                _cachedTriangleVertices[i * 3 + j].position.coords[k] = currentTriangle.vertex(j)->data().position[k];
                _cachedTriangleVertices[i * 3 + j].flatNormal.coords[k] = n[k];   
            }
        }
        
        i++;
    }
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        node->computeNormal();
    }
    
    for (TexCoordNode *node = _texCoords.begin(), *end = _texCoords.end(); node != end; node = node->next())
    {
        node->computeNormal();
    }
    
    i = 0;
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle2 &currentTriangle = node->data();
        
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                const Vector3D &n = _isUnwrapped ? currentTriangle.texCoord(j)->normal : currentTriangle.vertex(j)->normal;
                const Vector3D &t = currentTriangle.texCoord(j)->data().position;
                _cachedTriangleVertices[i * 3 + j].smoothNormal.coords[k] = n[k];
                _cachedTriangleVertices[i * 3 + j].texCoord.coords[k] = t[k];
            }
        }
        
        i++;
    }
    
    float selectedComponents[] = { 0.7f, 0.0f, 0.0f };
	
    i = 0;
	
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
	{
		if (node->data().selected)
            fillCachedColorsAtIndex(i, _cachedTriangleVertices, selectedComponents);
		else
            fillCachedColorsAtIndex(i, _cachedTriangleVertices, _colorComponents);				
        
        i++;
	}
    
    _cachedTriangleVertices.setValid(true);
    
    if (!_vboGenerated)
    {
        glGenBuffers(1, &_vboID);
        _vboGenerated = true;
    }

    glBindBuffer(GL_ARRAY_BUFFER, _vboID);
    glBufferData(GL_ARRAY_BUFFER, _cachedTriangleVertices.count() * sizeof(GLTriangleVertex), _cachedTriangleVertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh2::fillEdgeCache()
{
    if (_cachedEdgeVertices.isValid() && _cachedEdgeTexCoords.isValid())
        return;
    
    _cachedEdgeVertices.resize(_vertexEdges.count() * 2);
    _cachedEdgeTexCoords.resize(_texCoordEdges.count() * 2);
    
    Vector3D selectedColor(0.8f, 0.0f, 0.0f);
    Vector3D normalColor(_colorComponents[0] - 0.2f, _colorComponents[1] - 0.2f, _colorComponents[2] - 0.2f);
    
    int i = 0;
    
    for (VertexEdgeNode *node = _vertexEdges.begin(), *end = _vertexEdges.end(); node != end; node = node->next())
    {
        if (_selectionMode == MeshSelectionModeQuadsTriangles && node->data().isQuadEdge())
            continue;
        
        if (node->data().selected)
        {
            for (int k = 0; k < 3; k++)
            {
                _cachedEdgeVertices[i].color.coords[k] = selectedColor[k];
                _cachedEdgeVertices[i + 1].color.coords[k] = selectedColor[k];
            }
        }
        else
        {
            for (int k = 0; k < 3; k++)
            {
                _cachedEdgeVertices[i].color.coords[k] = normalColor[k];
                _cachedEdgeVertices[i + 1].color.coords[k] = normalColor[k];
            }
        }
        
        for (int k = 0; k < 3; k++)
        {
            _cachedEdgeVertices[i].position.coords[k] = node->data().vertex(0)->data().position[k];
            _cachedEdgeVertices[i + 1].position.coords[k] = node->data().vertex(1)->data().position[k];
        }
        
        i += 2;
    }
    
    _cachedEdgeVertices.resize(i); // resize doesn't delete [] internal array, if not needed
    _cachedEdgeVertices.setValid(true);
    
    i = 0;
    
    for (TexCoordEdgeNode *node = _texCoordEdges.begin(), *end = _texCoordEdges.end(); node != end; node = node->next())
    {
        if (node->data().selected)
        {
            for (int k = 0; k < 3; k++)
            {
                _cachedEdgeTexCoords[i].color.coords[k] = selectedColor[k];
                _cachedEdgeTexCoords[i + 1].color.coords[k] = selectedColor[k];
            }
        }
        else
        {
            for (int k = 0; k < 3; k++)
            {
                _cachedEdgeTexCoords[i].color.coords[k] = normalColor[k];
                _cachedEdgeTexCoords[i + 1].color.coords[k] = normalColor[k];
            }
        }
        
        for (int k = 0; k < 3; k++)
        {
            _cachedEdgeTexCoords[i].position.coords[k] = node->data().texCoord(0)->data().position[k];
            _cachedEdgeTexCoords[i + 1].position.coords[k] = node->data().texCoord(1)->data().position[k];
        }
        
        i += 2;
    }
    
    _cachedEdgeTexCoords.resize(i); // resize doesn't delete [] internal array, if not needed
    _cachedEdgeTexCoords.setValid(true);
}

void Mesh2::initOrUpdateTexture()
{
    if (!_texture)
        _texture = [[FPTexture alloc] initWithFile:@"checker.png" convertToAlpha:NO];
    else if (_texture.needUpdate)
        [_texture updateTexture];
}

void Mesh2::drawFill(FillMode fillMode, ViewMode viewMode)
{
    fillTriangleCache();
    
    if (fillMode.textured)
    {
        glEnable(GL_TEXTURE_2D);
        initOrUpdateTexture();
        glBindTexture(GL_TEXTURE_2D, [_texture textureID]);
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, _vboID);
    
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

    if (fillMode.textured)
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    if (fillMode.colored)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, color));
    }
    
    if (viewMode == ViewModeSolidSmooth)
        glNormalPointer(GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, smoothNormal));
    else
        glNormalPointer(GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, flatNormal));
    
    if (fillMode.textured)
        glTexCoordPointer(2, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, texCoord));
    
    if (viewMode == ViewModeUnwrap)
        glVertexPointer(3, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, texCoord));
    else
        glVertexPointer(3, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, position));

    glDrawArrays(GL_TRIANGLES, 0, _triangles.count() * 3);

    if (fillMode.colored)
        glDisableClientState(GL_COLOR_ARRAY);
    
    if (fillMode.textured)
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (fillMode.textured)
    {
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_TEXTURE_2D); 
    }
}

void Mesh2::draw(ViewMode viewMode, const Vector3D &scale, bool selected, bool forSelection)
{
	bool flipped = scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f;
    ShaderProgram *shader = flipped ? [ShaderProgram flippedShader] : [ShaderProgram normalShader];
    
    FillMode fillMode;
	
	glPushMatrix();
	glScalef(scale.x, scale.y, scale.z);
	if (viewMode == ViewModeWireframe)
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
        
        [shader useProgram];
        if (_selectionMode != MeshSelectionModeTriangles)
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
        [ShaderProgram resetProgram];
        
        if (selected)
        {
            glDisable(GL_POLYGON_OFFSET_FILL);
            drawAllEdges(viewMode, forSelection);
        }        
        
        if (_isUnwrapped)
        {
            glColor3f(1.0f, 1.0f, 1.0f);
            [_texture drawForUnwrap];
        }
	}
	glPopMatrix();
}

void Mesh2::drawAtIndex(uint index, bool forSelection, ViewMode viewMode)
{
    switch (_selectionMode) 
	{
		case MeshSelectionModeVertices:
		{
            bool selected;
            Vector3D v;
            
            if (_isUnwrapped)
            {
                const TexCoord &texCoord = _cachedTexCoordSelection.at(index)->data();
                selected = texCoord.selected;
                v = texCoord.position;
            }
            else
            {
                const Vertex2 &vertex = _cachedVertexSelection.at(index)->data();
                selected = vertex.selected;
                v = vertex.position;
            }
            
			if (!forSelection)
			{
				glPointSize(5.0f);
				if (selected)
					glColor3f(1.0f, 0.0f, 0.0f);
				else
					glColor3f(0.0f, 0.0f, 1.0f);
			}
			glBegin(GL_POINTS);
			glVertex3f(v.x, v.y, v.z);
			glEnd();
		} break;
		case MeshSelectionModeTriangles:
		{
			if (forSelection)
			{
				const Triangle2 &triangle = _cachedTriangleSelection.at(index)->data();
				glBegin(GL_TRIANGLES);
                if (_isUnwrapped)
                {
                    for (uint i = 0; i < 3; i++)
                    {
                        Vector3D v = triangle.texCoord(i)->data().position;
                        glVertex3f(v.x, v.y, v.z);
                    }
                }
                else 
                {
                    for (uint i = 0; i < 3; i++)
                    {
                        Vector3D v = triangle.vertex(i)->data().position;
                        glVertex3f(v.x, v.y, v.z);
                    }
                }
				glEnd();
			}
		} break;
        case MeshSelectionModeEdges:
        {
            if (_isUnwrapped)
            {
                const TexCoordEdge &edge = _cachedTexCoordEdgeSelection.at(index)->data();
                if (!forSelection)
                {
                    BOOL isSelected = edge.selected;
                    if (isSelected)
                        glColor3f(0.8f, 0.0f, 0.0f);
                    else
                        glColor3f(_colorComponents[0] - 0.2f, _colorComponents[1] - 0.2f, _colorComponents[2] - 0.2f);
                }
                glBegin(GL_LINES);
                for (int i = 0; i < 2; i++)
                {
                    Vector3D v = edge.texCoord(i)->data().position;
                    glVertex3f(v.x, v.y, v.z);
                }
                glEnd();
            }
            else 
            {    
                const VertexEdge &edge = _cachedVertexEdgeSelection.at(index)->data();
                if (!forSelection)
                {
                    BOOL isSelected = edge.selected;
                    if (isSelected)
                        glColor3f(0.8f, 0.0f, 0.0f);
                    else
                        glColor3f(_colorComponents[0] - 0.2f, _colorComponents[1] - 0.2f, _colorComponents[2] - 0.2f);
                }
                glBegin(GL_LINES);
                for (int i = 0; i < 2; i++)
                {
                    Vector3D v = edge.vertex(i)->data().position;
                    glVertex3f(v.x, v.y, v.z);
                }
                glEnd();
            }
        } break;
        case MeshSelectionModeQuadsTriangles:
        {
            
        }break;
	}
}

void Mesh2::drawAllVertices(ViewMode viewMode, bool forSelection)
{
    glPointSize(5.0f);
    
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
                tempColors.push_back(colorIndex);
                tempVertices.push_back(node->data().position);
            }
        }
        else
        {
            for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
            {
                colorIndex++;
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

void Mesh2::drawAllTriangles(ViewMode viewMode, bool forSelection)
{
    for (int i = 0; i < _triangles.count(); i++)
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
                colorIndex++;
                tempColors.push_back(colorIndex);
                tempColors.push_back(colorIndex);
            }
        }
        else
        {
            for (VertexEdgeNode *node = _vertexEdges.begin(), *end = _vertexEdges.end(); node != end; node = node->next())
            {
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
            
            glDrawArrays(GL_LINES, 0, _cachedEdgeTexCoords.count());
        }
        else
        {
            float *vertexPtr = (float *)&_cachedEdgeVertices[0].position;
            glVertexPointer(3, GL_FLOAT, sizeof(GLEdgeVertex), vertexPtr);
            
            glDrawArrays(GL_LINES, 0, _cachedEdgeVertices.count());
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
            
            glDrawArrays(GL_LINES, 0, _cachedEdgeTexCoords.count());
        }
        else
        {
            float *colorPtr = (float *)&_cachedEdgeVertices[0].color;
            glColorPointer(3, GL_FLOAT, sizeof(GLEdgeVertex), colorPtr);
            
            float *vertexPtr = (float *)&_cachedEdgeVertices[0].position;
            glVertexPointer(3, GL_FLOAT, sizeof(GLEdgeVertex), vertexPtr);
            
            glDrawArrays(GL_LINES, 0, _cachedEdgeVertices.count());
        }
        
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void Mesh2::drawAll(ViewMode viewMode, bool forSelection)
{
    switch (_selectionMode) 
	{
        case MeshSelectionModeVertices:
            drawAllVertices(viewMode, forSelection);
            break;
        case MeshSelectionModeTriangles:
            drawAllTriangles(viewMode, forSelection);
            break;
        case MeshSelectionModeEdges:
            drawAllEdges(viewMode, forSelection);
            break;
        case MeshSelectionModeQuadsTriangles:
            //drawAllTriangles(mode, forSelection);
            break;
    }
}

void Mesh2::uvToPixels(float &u, float &v)
{
    u *= (float)_texture.width;
    v *= (float)_texture.height;
    
    v = (float)_texture.height - v;
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

void Mesh2::cleanTexture()
{   
    [[_texture canvas] lockFocus];
    
    [[NSColor whiteColor] setFill];
    NSRectFill(NSMakeRect(0, 0, [_texture width], [_texture height]));
    
    [[_texture canvas] unlockFocus];
    [_texture updateTexture]; 
}
