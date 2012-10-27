// complex test - reimplementation of loop subdivision in JavaScript
// this version ignores texture coordinates

function triangulate(item)
{
    var twoTriIndices = [ 0, 1, 2, 0, 2, 3 ];
    var triangles = [];
    triangles[0] = [];
    triangles[1] = [];

    var quad = item.triQuadIterator();
    for (quad; !quad.finished(); quad.moveNext())
    {
        if (quad.isQuad())
        {
            var v = 0;
            for (var i = 0; i < 2; i++)
            {
                for (var j = 0; j < 3; j++)
                {
                    var index = twoTriIndices[v];
                    v++;
                    triangles[i][j] = quad.vertex(index);                    
                }
            
                item.addTriangle(triangles[i][0], 
                                 triangles[i][1], 
                                 triangles[i][2]);
            }
            item.removeTriQuad(quad);
        }
    }
    
    item.makeEdges();
    item.updateSelection();
}

function halfEdges(item)
{
    var edge = item.edgeIterator();
    for (edge; !edge.finished(); edge.moveNext())
    {
        var v1 = edge.vertex(0);
        var v2 = edge.vertex(1);
        
        var eX, eY, eZ;
        
        var t0 = edge.triangle(0);
        var t1 = edge.triangle(1);
        
        if (t0 === null || t1 === null)
        {
            eX = (v1.x() + v2.x()) / 2.0;
            eY = (v1.y() + v2.y()) / 2.0;
            eZ = (v1.z() + v2.z()) / 2.0;
        }
        else
        {
            var v3 = t0.vertexNotInEdge(edge);
            var v4 = t1.vertexNotInEdge(edge);
            
            if (v3 === null || v4 === null)
            {
                eX = (v1.x() + v2.x()) / 2.0;
                eY = (v1.y() + v2.y()) / 2.0;
                eZ = (v1.z() + v2.z()) / 2.0;
            }
            else
            {
                eX = 3.0 * (v1.x() + v2.x()) / 8.0 + 1.0 * (v3.x() + v4.x()) / 8.0;
                eY = 3.0 * (v1.y() + v2.y()) / 8.0 + 1.0 * (v3.y() + v4.y()) / 8.0;
                eZ = 3.0 * (v1.z() + v2.z()) / 8.0 + 1.0 * (v3.z() + v4.z()) / 8.0;
            }
        }
        
        var edgeVertex = item.addVertex(eX, eY, eZ);
        edge.setHalf(edgeVertex);
    }
}

function repositionVertices(item, vertexCount)
{
    var tempVerticesXYZ = [];
    
    var index = 0;
    
    var vertex = item.vertexIterator();
    for (vertex; index < vertexCount; vertex.moveNext())
    {
        vertex.setIndex(index);
        index++;
        
        var beta, n;
        n = vertex.edgeCount();
        beta = 3.0 + 2.0 * Math.cos(Math.PI * 2.0 / n);
        beta = 5.0 / 8.0 - (beta * beta) / 64.0;
        
        var bon = beta / n;        
        beta = 1.0 - beta;
        
        var finalX = beta * vertex.x();
        var finalY = beta * vertex.y();
        var finalZ = beta * vertex.z();
        
        var edge = vertex.edgeIterator();
        
        for (edge; !edge.finished(); edge.moveNext())
        {
            var oppositeVertex = edge.oppositeVertex(vertex);
            
            finalX += oppositeVertex.x() * bon;
            finalY += oppositeVertex.y() * bon;
            finalZ += oppositeVertex.z() * bon;
        }      
        
        tempVerticesXYZ.push(finalX);
        tempVerticesXYZ.push(finalY);
        tempVerticesXYZ.push(finalZ);
    }
    
    index = 0;
    vertex.moveStart();
    for (vertex; index < vertexCount; vertex.moveNext())
    {
        vertex.setX(tempVerticesXYZ[vertex.index() * 3]);
        vertex.setY(tempVerticesXYZ[vertex.index() * 3 + 1]);
        vertex.setZ(tempVerticesXYZ[vertex.index() * 3 + 2]);
        index++;
    }
}

function makeSubdividedTriangles(item)
{
    var triangleCount = item.triQuadCount();
    var triangle = item.triQuadIterator();
    
    var index = 0;
    
    var vertices = [];
    
    for (triangle; index < triangleCount; triangle.moveNext())
    {
        index++;
        
        for (var i = 0; i < 3; i++)
        {
            vertices[i] = triangle.vertex(i);
            vertices[i + 3]  = triangle.edge(i).half();
        }
        
        item.addTriangle(vertices[0], vertices[3], vertices[5]);
        item.addTriangle(vertices[3], vertices[1], vertices[4]);
        item.addTriangle(vertices[5], vertices[4], vertices[2]);
        item.addTriangle(vertices[3], vertices[4], vertices[5]);
        
        item.removeTriQuad(triangle);
    } 
}

function loopSubdivision(item)
{
    triangulate(item);
    
    var vertexCount = item.vertexCount();
    
    halfEdges(item);
    repositionVertices(item, vertexCount);
    makeSubdividedTriangles(item);
}

for (var i = 0; i < items.count(); i++)
{
    var item = items.at(i);
    loopSubdivision(item);
}