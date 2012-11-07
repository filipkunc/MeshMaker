// quadify item by removing longest shared edge in two triangles 

function edgeSqLength(v0, v1)
{
	var x = v1.x() - v0.x();
	var y = v1.y() - v0.y();
	var z = v1.z() - v0.z();
	
	var sqLength = x * x + y * y + z * z;
	return sqLength;
}

function isLongestEdgeInTriangles(edge, t0, t1)
{
	var ev0 = edge.vertex(0);
	var ev1 = edge.vertex(1);
	var ov0 = t0.vertexNotInEdge(edge);
	var ov1 = t1.vertexNotInEdge(edge);
	var currentEdgeSqLength = edgeSqLength(ev0, ev1);
	
	if (currentEdgeSqLength < edgeSqLength(ev0, ov0))
		return false;
	if (currentEdgeSqLength < edgeSqLength(ev0, ov1))
		return false;
		
	if (currentEdgeSqLength < edgeSqLength(ev1, ov0))
		return false;
	if (currentEdgeSqLength < edgeSqLength(ev1, ov1))
		return false;
		
	return true;
}

function quadify(item)
{
	item.setSelectionModeEdges();
	
	var edge = item.edgeIterator();
    for (edge; !edge.finished(); edge.moveNext())
    {
    	if (edge.selected())
    		continue;
    	
    	var t0 = edge.triangle(0);
    	var t1 = edge.triangle(1);
    	
        if (t0 && t1 && !t0.isQuad() && !t1.isQuad())
        {
        	if (isLongestEdgeInTriangles(edge, t0, t1))
        		edge.setSelected(true);
        }       
    }
    
    item.updateSelection();
    item.removeSelected();
}

for (var i = 0; i < items.count(); i++)
{
    var item = items.at(i);
    if (item.selected)
        quadify(item);
}