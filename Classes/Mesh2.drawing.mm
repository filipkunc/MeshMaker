//
//  Mesh2.drawing.mm
//  OpenGLEditor
//
//  Created by Filip Kunc on 19.06.11.
//  Copyright 2011 Filip Kunc. All rights reserved.
//

#import "Mesh2.h"
#import "OpenGLDrawing.h"
#import "ShaderProgram.h"
#import "FPTexture.h"

FPTexture *checkerTexture = nil;

void loadTextureIfNeeded();

void loadTextureIfNeeded()
{
    if (!checkerTexture)
        checkerTexture = [[FPTexture alloc] initWithFile:@"checker.png" convertToAlpha:NO];    
}

Point2D makePoint(float x, float y);

Point2D makePoint(float x, float y)
{
    Point2D point;
    point.coords[0] = x;
    point.coords[1] = y;
    return point;
}

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
}

void Mesh2::fillTriangleCache()
{
    if (_cachedTriangleVertices.isValid())
        return;
    
    _cachedTriangleVertices.resize(_triangles.count() * 3);
    
    int i = 0;
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle2 &currentTriangle = node->data;
        
        currentTriangle.computeNormal();
        Vector3D n = currentTriangle.normal;
        
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                _cachedTriangleVertices[i * 3 + j].position.coords[k] = currentTriangle.vertex(j)->data.position[k];
                _cachedTriangleVertices[i * 3 + j].flatNormal.coords[k] = n[k];   
            }
        }
        
        i++;
    }
    
    for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
    {
        node->computeNormal();
    }
    
    i = 0;
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle2 &currentTriangle = node->data;
        
        for (int j = 0; j < 3; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                const Vector3D &n = currentTriangle.vertex(j)->normal;
                const Vector2D &t = currentTriangle.texCoord(j)->data.position;
                _cachedTriangleVertices[i * 3 + j].smoothNormal.coords[k] = n[k];
                if (k < 2)
                    _cachedTriangleVertices[i * 3 + j].texCoord.coords[k] = t[k];
            }
        }
        
        i++;
    }
    
    float selectedComponents[] = { 0.7f, 0.0f, 0.0f };
	
    i = 0;
	
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
	{
		if (node->data.selected)
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
    if (_cachedEdgeVertices.isValid())
        return;
    
    _cachedEdgeVertices.resize(_edges.count() * 2);
    
    Vector3D selectedColor(0.8f, 0.0f, 0.0f);
    Vector3D normalColor(_colorComponents[0] - 0.2f, _colorComponents[1] - 0.2f, _colorComponents[2] - 0.2f);
    
    int i = 0;
    
    for (VertexEdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
    {
        if (_selectionMode == MeshSelectionModeQuadsTriangles && node->data.isQuadEdge())
            continue;
        
        if (node->data.selected)
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
            _cachedEdgeVertices[i].position.coords[k] = node->data.vertex(0)->data.position[k];
            _cachedEdgeVertices[i + 1].position.coords[k] = node->data.vertex(1)->data.position[k];
            if (k < 2)
            {
                _cachedEdgeVertices[i].texCoord.coords[k] = node->data.texCoord(0)->data.position[k];
                _cachedEdgeVertices[i + 1].texCoord.coords[k] = node->data.texCoord(1)->data.position[k];
            }
        }
        
        i += 2;
    }
    
    _cachedEdgeVertices.resize(i); // resize doesn't delete [] internal array, if not needed
    _cachedEdgeVertices.setValid(true);
}

void Mesh2::drawColoredFill(bool colored, ViewMode mode)
{
    fillTriangleCache();
    
    glEnable(GL_TEXTURE_2D);
    loadTextureIfNeeded();
    glBindTexture(GL_TEXTURE_2D, [checkerTexture textureID]);
    
    glBindBuffer(GL_ARRAY_BUFFER, _vboID);
    
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    if (colored)
    {
        glEnableClientState(GL_COLOR_ARRAY);
        glColorPointer(3, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, color));
    }
    
    if (mode == ViewModeSolidSmooth)
        glNormalPointer(GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, smoothNormal));
    else
        glNormalPointer(GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, flatNormal));
    
    glTexCoordPointer(2, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, texCoord));
    
    if (mode == ViewModeUnwrap)
        glVertexPointer(2, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, texCoord));
    else
        glVertexPointer(3, GL_FLOAT, sizeof(GLTriangleVertex), (void *)offsetof(GLTriangleVertex, position));

    glDrawArrays(GL_TRIANGLES, 0, _triangles.count() * 3);

    if (colored)
        glDisableClientState(GL_COLOR_ARRAY);
    
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisable(GL_TEXTURE_2D);
    
	/*glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (colored)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		float *colorPtr = (float *)&_cachedTriangleVertices[0].color;
		glColorPointer(3, GL_FLOAT, sizeof(GLVertex), colorPtr);
	}
	
	float *vertexPtr = (float *)&_cachedTriangleVertices[0].position;
	float *normalPtr;
    
    if (useVertexNormals)
        normalPtr = (float *)&_cachedTriangleVertices[0].smoothNormal;
    else
        normalPtr = (float *)&_cachedTriangleVertices[0].flatNormal;
	
	glNormalPointer(GL_FLOAT, sizeof(GLVertex), normalPtr);
	glVertexPointer(3, GL_FLOAT, sizeof(GLVertex), vertexPtr);
	
	glDrawArrays(GL_TRIANGLES, 0, _triangles.count() * 3);
	
	if (colored)
		glDisableClientState(GL_COLOR_ARRAY);
    
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);*/
}

void Mesh2::draw(ViewMode mode, const Vector3D &scale, bool selected, bool forSelection)
{
	bool flipped = scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f;
    ShaderProgram *shader = flipped ? [ShaderProgram flippedShader] : [ShaderProgram normalShader];
	
	glPushMatrix();
	glScalef(scale.x, scale.y, scale.z);
	if (mode == ViewModeWireframe)
	{
        drawAllEdges(mode, forSelection);
	}
	else if (forSelection)
	{
        drawColoredFill(false, mode);
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
            drawColoredFill(false, mode);
        }
        else
        {
            drawColoredFill(true, mode);
        }
        [ShaderProgram resetProgram];
        
        if (selected)
        {
            glDisable(GL_POLYGON_OFFSET_FILL);
            drawAllEdges(mode, forSelection);
        }
        
	}
	glPopMatrix();
}

void Mesh2::drawAtIndex(uint index, bool forSelection, ViewMode mode)
{
    switch (_selectionMode) 
	{
		case MeshSelectionModeVertices:
		{
            bool selected;
            Vector3D v;
            
            if (_isUnwrapped)
            {
                const TextureCoordinate &texCoord = _cachedTextureCoordinateSelection.at(index)->data;
                selected = texCoord.selected;
                v = Vector3D(texCoord.position.x, texCoord.position.y, 0.0f);
            }
            else
            {
                const Vertex2 &vertex = _cachedVertexSelection.at(index)->data;
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
				const Triangle2 &triangle = _cachedTriangleSelection.at(index)->data;
				glBegin(GL_TRIANGLES);
				for (uint i = 0; i < 3; i++)
				{
					Vector3D v = triangle.vertex(i)->data.position;
					glVertex3f(v.x, v.y, v.z);
				}
				glEnd();
			}
		} break;
        case MeshSelectionModeEdges:
        {
            const VertexEdge &edge = _cachedEdgeSelection.at(index)->data;
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
                Vector3D v = edge.vertex(i)->data.position;
                glVertex3f(v.x, v.y, v.z);
            }
            glEnd();
        } break;
        case MeshSelectionModeQuadsTriangles:
        {
            
        }break;
	}
}

void Mesh2::drawAllVertices(ViewMode mode, bool forSelection)
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
            drawColoredFill(false, mode);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
        
        if (_isUnwrapped)
        {
            for (TextureCoordinateNode *node = _textureCoordinates.begin(), *end = _textureCoordinates.end(); node != end; node = node->next())
            {
                colorIndex++;
                tempColors.push_back(colorIndex);
                tempVertices.push_back(Vector3D(node->data.position.x, node->data.position.y, 0.0f));
            }
        }
        else
        {
            for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
            {
                colorIndex++;
                tempColors.push_back(colorIndex);            
                tempVertices.push_back(node->data.position);            
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
            for (TextureCoordinateNode *node = _textureCoordinates.begin(), *end = _textureCoordinates.end(); node != end; node = node->next())
            {
                if (node->data.selected)
                    tempColors.push_back(selectedColor);
                else
                    tempColors.push_back(normalColor);
                
                tempVertices.push_back(Vector3D(node->data.position.x, node->data.position.y, 0.0f));
            }
        }
        else
        {
            for (VertexNode *node = _vertices.begin(), *end = _vertices.end(); node != end; node = node->next())
            {
                if (node->data.selected)
                    tempColors.push_back(selectedColor); 
                else if (_useSoftSelection && node->selectionWeight > 0.0f)
                    tempColors.push_back(Vector3D(1.0f, 1.0f - node->selectionWeight, 0.0f));
                else
                    tempColors.push_back(normalColor);
                
                tempVertices.push_back(node->data.position);            
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

void Mesh2::drawAllTriangles(ViewMode mode, bool forSelection)
{
    for (int i = 0; i < _triangles.count(); i++)
    {
        uint colorIndex = i + 1;
        glColor4ubv((GLubyte *)&colorIndex);
        drawAtIndex(i, forSelection, mode);
    }
}

void Mesh2::drawAllEdges(ViewMode mode, bool forSelection)
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
            drawColoredFill(false, mode);
            glDisable(GL_POLYGON_OFFSET_FILL);
        }
        
        for (VertexEdgeNode *node = _edges.begin(), *end = _edges.end(); node != end; node = node->next())
        {
            colorIndex++;
            tempColors.push_back(colorIndex);
            tempColors.push_back(colorIndex);
        }
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        GLubyte *colorPtr = (GLubyte *)&tempColors[0];
        glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorPtr);
        
        if (_isUnwrapped)
        {
            float *vertexPtr = (float *)&_cachedEdgeVertices[0].texCoord;
            glVertexPointer(2, GL_FLOAT, sizeof(GLEdgeVertex), vertexPtr);
        }
        else
        {
            float *vertexPtr = (float *)&_cachedEdgeVertices[0].position;
            glVertexPointer(3, GL_FLOAT, sizeof(GLEdgeVertex), vertexPtr);
        }
        
        glDrawArrays(GL_LINES, 0, _cachedEdgeVertices.count());
        
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
    else
    {
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        float *colorPtr = (float *)&_cachedEdgeVertices[0].color;
        glColorPointer(3, GL_FLOAT, sizeof(GLEdgeVertex), colorPtr);
        
        if (_isUnwrapped)
        {
            float *vertexPtr = (float *)&_cachedEdgeVertices[0].texCoord;
            glVertexPointer(2, GL_FLOAT, sizeof(GLEdgeVertex), vertexPtr);
        }
        else
        {
            float *vertexPtr = (float *)&_cachedEdgeVertices[0].position;
            glVertexPointer(3, GL_FLOAT, sizeof(GLEdgeVertex), vertexPtr);
        }
        
        glDrawArrays(GL_LINES, 0, _cachedEdgeVertices.count());
        
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
    }
}

void Mesh2::drawAll(ViewMode mode, bool forSelection)
{
    switch (_selectionMode) 
	{
        case MeshSelectionModeVertices:
            drawAllVertices(mode, forSelection);
            break;
        case MeshSelectionModeTriangles:
            drawAllTriangles(mode, forSelection);
            break;
        case MeshSelectionModeEdges:
            drawAllEdges(mode, forSelection);
            break;
        case MeshSelectionModeQuadsTriangles:
            //drawAllTriangles(mode, forSelection);
            break;
    }
}
