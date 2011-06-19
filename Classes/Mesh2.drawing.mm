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

void fillCachedColorsAtIndex(uint index, Vector3D *cachedColors, float components[3]);

void fillCachedColorsAtIndex(uint index, Vector3D *cachedColors, float components[3])
{
    for (uint j = 0; j < 3; j++)
    {
        for (uint k = 0; k < 3; k++)
        {
            cachedColors[index * 3 + j][k] = components[k];
        }
    }	
}

void Mesh2::resetCache()
{
    if (_cachedVertices)
	{
		delete [] _cachedVertices;
		_cachedVertices = NULL;
	}
	if (_cachedNormals)
	{
		delete [] _cachedNormals;
		_cachedNormals = NULL;
	}
	if (_cachedColors)
	{
		delete [] _cachedColors;
		_cachedColors = NULL;
	}
}

void Mesh2::fillCache()
{
    if (!_cachedVertices)
	{
		_cachedVertices = new Vector3D[_triangles->count() * 3];
		_cachedNormals = new Vector3D[_triangles->count() * 3];
		_cachedColors = new Vector3D[_triangles->count() * 3];
		Vector3D triangleVertices[3];
        
        uint i = 0;
		
		for (TriangleNode *node = _triangles->begin(), *end = _triangles->end(); node != end; node = node->next())
		{
			Triangle2 currentTriangle = node->data;
            currentTriangle.GetVertexPositions(triangleVertices);
			
			Vector3D n = NormalFromTriangleVertices(triangleVertices);
			
			for (uint j = 0; j < 3; j++)
			{
				_cachedVertices[i * 3 + j] = triangleVertices[j];
				_cachedNormals[i * 3 + j] = n;
			}
            
            i++;
		}
	}
}

void Mesh2::fillColorCache(bool darker)
{
    float darkerComponents[3];
    float selectedComponents[] = { 0.7f, 0.0f, 0.0f };
	
	if (darker)
	{
		for (uint k = 0; k < 3; k++)
			darkerComponents[k] = _colorComponents[k] -  0.2f;
		
		selectedComponents[0] -= 0.2f;
	}
    else
    {
        for (uint k = 0; k < 3; k++)
			darkerComponents[k] = _colorComponents[k];
    }
    
    uint i = 0;
	
    for (TriangleNode *node = _triangles->begin(), *end = _triangles->end(); node != end; node = node->next())
	{
		if (node->data.selected)
		{
            fillCachedColorsAtIndex(i, _cachedColors, selectedComponents);
		}
		else
		{
            fillCachedColorsAtIndex(i, _cachedColors, darkerComponents);				
		}
        
        i++;
	}
}

void Mesh2::drawFill(bool darker, bool forSelection)
{
    fillCache();
	if (_selectionMode == MeshSelectionModeTriangles && !forSelection)
    {
        fillColorCache(darker);
    }
    
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (_selectionMode == MeshSelectionModeTriangles && !forSelection)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		float *colorPtr = (float *)_cachedColors;
		glColorPointer(3, GL_FLOAT, 0, colorPtr);
	}
	
	float *vertexPtr = (float *)_cachedVertices;
	float *normalPtr = (float *)_cachedNormals;
	
	glNormalPointer(GL_FLOAT, 0, normalPtr);
	glVertexPointer(3, GL_FLOAT, 0, vertexPtr);
	
	glDrawArrays(GL_TRIANGLES, 0, _triangles->count() * 3);
	
	if (_selectionMode == MeshSelectionModeTriangles && !forSelection)
		glDisableClientState(GL_COLOR_ARRAY);
    
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void Mesh2::drawWire()
{
    if (_selectionMode != MeshSelectionModeEdges)
    {
        glColor3f(_colorComponents[0] - 0.2f, _colorComponents[1] - 0.2f, _colorComponents[2] - 0.2f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawFill(true, false);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Mesh2::draw(ViewMode mode, const Vector3D &scale, bool selected, bool forSelection)
{
	bool flipped = scale.x < 0.0f || scale.y < 0.0f || scale.z < 0.0f;
    ShaderProgram *shader = flipped ? [ShaderProgram flippedShader] : [ShaderProgram normalShader];
	
	glPushMatrix();
	glScalef(scale.x, scale.y, scale.z);
	if (mode == ViewModeWireframe)
	{
        if (!forSelection)
            glColor3f(_colorComponents[0] + 0.2f, _colorComponents[1] + 0.2f, _colorComponents[2] + 0.2f);
    
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        drawFill(true, forSelection);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (forSelection)
	{
        drawFill(true, forSelection);
    }
    else
    {
		if (selected)
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0f, 1.0f);
            [shader useProgram];
            if (_selectionMode != MeshSelectionModeTriangles)
			{
				glColor3f(_colorComponents[0], _colorComponents[1], _colorComponents[2]);
			}
            drawFill(false, forSelection);
			[ShaderProgram resetProgram];
			glDisable(GL_POLYGON_OFFSET_FILL);
            drawWire();
		}
		else
		{
            [shader useProgram];
            if (_selectionMode != MeshSelectionModeTriangles)
            {
                glColor3f(_colorComponents[0], _colorComponents[1], _colorComponents[2]);
            }
            drawFill(false, false);
            [ShaderProgram resetProgram];
		}
	}
	glPopMatrix();
}

void Mesh2::drawAtIndex(uint index, bool forSelection, ViewMode mode)
{
    /*if (!selected->at(index).visible)
     return;*/
    
    switch (_selectionMode) 
	{
		case MeshSelectionModeVertices:
		{
            const Vertex2 &vertex = _cachedVertexSelection->at(index)->data;
			Vector3D v = vertex.position;
			if (!forSelection)
			{
				BOOL isSelected = vertex.selected;
				glPointSize(5.0f);
				if (isSelected)
					glColor3f(1.0f, 0.0f, 0.0f);
				else
					glColor3f(0.0f, 0.0f, 1.0f);
				glDisable(GL_LIGHTING);
			}
			glBegin(GL_POINTS);
			glVertex3f(v.x, v.y, v.z);
			glEnd();
		} break;
		case MeshSelectionModeTriangles:
		{
			if (forSelection)
			{
				Triangle2 triangle = _cachedTriangleSelection->at(index)->data;
                Vector3D triangleVertices[3];
                triangle.GetVertexPositions(triangleVertices);
				glBegin(GL_TRIANGLES);
				for (uint i = 0; i < 3; i++)
				{
					Vector3D v = triangleVertices[i];
					glVertex3f(v.x, v.y, v.z);
				}
				glEnd();
			}
		} break;
        default:
            break;
            /*case MeshSelectionModeEdges:
             {
             Edge currentEdge = [self edgeAtIndex:index];
             if (!forSelection)
             {
             BOOL isSelected = selected->at(index).selected;
             if (isSelected)
             glColor3f(0.8f, 0.0f, 0.0f);
             else
             glColor3f([color redComponent] - 0.2f, [color greenComponent] - 0.2f, [color blueComponent] - 0.2f);
             glDisable(GL_LIGHTING);
             }
             glBegin(GL_LINES);
             for (uint i = 0; i < 2; i++)
             {
             Vector3D v = [self vertexAtIndex:currentEdge.vertexIndices[i]];
             glVertex3f(v.x, v.y, v.z);
             }
             glEnd();
             } break;*/
	}
}


