// complex test - reimplementation of loop subdivision in JavaScript
// not finished yet

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
        
        if (t0 == null || t1 == null)
        {
            eX = (v1.x() + v2.x()) / 2.0;
            eY = (v1.y() + v2.y()) / 2.0;
            eZ = (v1.z() + v2.z()) / 2.0;
        }
        else
        {
            var v3 = t0.vertexNotInEdge(edge);
            var v4 = t1.vertexNotInEdge(edge);
            
            if (v3 == null || v4 == null)
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
    // TODO
}

function makeSubdividedTriangles(item)
{
    // TODO
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