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
	resetColorCache();
}

void Mesh2::resetColorCache()
{
    if (_cachedColors)
	{
		delete [] _cachedColors;
		_cachedColors = NULL;
	}
}

void Mesh2::fillCache()
{
    if (_cachedVertices)
        return;
    
    _cachedVertices = new Vector3D[_triangles.count() * 3];
    _cachedNormals = new Vector3D[_triangles.count() * 3];
    Vector3D triangleVertices[3];
    
    uint i = 0;
    
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
    {
        Triangle2 currentTriangle = node->data;
        currentTriangle.getVertexPositions(triangleVertices);
        
        Vector3D n = NormalFromTriangleVertices(triangleVertices);
        
        for (uint j = 0; j < 3; j++)
        {
            _cachedVertices[i * 3 + j] = triangleVertices[j];
            _cachedNormals[i * 3 + j] = n;
        }
        
        i++;
    }
}

void Mesh2::fillColorCache()
{
    if (_cachedColors)
        return;
    
    _cachedColors = new Vector3D[_triangles.count() * 3];    
    
    float selectedComponents[] = { 0.7f, 0.0f, 0.0f };
	
    uint i = 0;
	
    for (TriangleNode *node = _triangles.begin(), *end = _triangles.end(); node != end; node = node->next())
	{
		if (node->data.selected)
		{
            fillCachedColorsAtIndex(i, _cachedColors, selectedComponents);
		}
		else
		{
            fillCachedColorsAtIndex(i, _cachedColors, _colorComponents);				
		}
        
        i++;
	}
}

void Mesh2::drawColoredFill(bool colored)
{
    fillCache();
	if (colored)
        fillColorCache();
    
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	if (colored)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		float *colorPtr = (float *)_cachedColors;
		glColorPointer(3, GL_FLOAT, 0, colorPtr);
	}
	
	float *vertexPtr = (float *)_cachedVertices;
	float *normalPtr = (float *)_cachedNormals;
	
	glNormalPointer(GL_FLOAT, 0, normalPtr);
	glVertexPointer(3, GL_FLOAT, 0, vertexPtr);
	
	glDrawArrays(GL_TRIANGLES, 0, _triangles.count() * 3);
	
	if (colored)
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
        drawColoredFill(false);
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
        drawColoredFill(false);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	else if (forSelection)
	{
        drawColoredFill(false);
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
				glColor3fv(_colorComponents);
                drawColoredFill(false);
			}
            else
            {
                drawColoredFill(true);
            }
			[ShaderProgram resetProgram];
			glDisable(GL_POLYGON_OFFSET_FILL);
            drawWire();
		}
		else
		{
            [shader useProgram];
            if (_selectionMode != MeshSelectionModeTriangles)
			{
				glColor3fv(_colorComponents);
                drawColoredFill(false);
			}
            else
            {
                drawColoredFill(true);
            }
            [ShaderProgram resetProgram];
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
            const Vertex2 &vertex = _cachedVertexSelection.at(index)->data;
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
				const Triangle2 &triangle = _cachedTriangleSelection.at(index)->data;
                Vector3D triangleVertices[3];
                triangle.getVertexPositions(triangleVertices);
				glBegin(GL_TRIANGLES);
				for (uint i = 0; i < 3; i++)
				{
					Vector3D v = triangleVertices[i];
					glVertex3f(v.x, v.y, v.z);
				}
				glEnd();
			}
		} break;
        case MeshSelectionModeEdges:
        {
            const Edge2 &edge = _cachedEdgeSelection.at(index)->data;
            if (!forSelection)
            {
                BOOL isSelected = edge.selected;
                if (isSelected)
                    glColor3f(0.8f, 0.0f, 0.0f);
                else
                    glColor3f(_colorComponents[0] - 0.2f, _colorComponents[1] - 0.2f, _colorComponents[2] - 0.2f);
                glDisable(GL_LIGHTING);
            }
            glBegin(GL_LINES);
            for (int i = 0; i < 2; i++)
            {
                Vector3D v = edge.vertex(i)->data.position;
                glVertex3f(v.x, v.y, v.z);
            }
            glEnd();
        } break;
        default:
            break;
	}
}

void Mesh2::drawAllVertices(ViewMode viewMode, bool forSelection)
{
    for (int i = 0; i < _vertices.count(); i++)
    {
        uint colorIndex = i + 1;
        glColor4ubv((GLubyte *)&colorIndex);
        drawAtIndex(i, forSelection, viewMode);
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
    for (int i = 0; i < _edges.count(); i++)
    {
        uint colorIndex = i + 1;
        glColor4ubv((GLubyte *)&colorIndex);
        drawAtIndex(i, forSelection, viewMode);
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
    }
}
