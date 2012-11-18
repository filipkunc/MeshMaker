// make same edge size from first selected edge

function edgeLength(edge)
{
	var v0 = edge.vertex(0);
	var v1 = edge.vertex(1);
	
	var x = v1.x() - v0.x();
	var y = v1.y() - v0.y();
	var z = v1.z() - v0.z();
	
	var length = Math.sqrt(x * x + y * y + z * z);
	return length;
}

function setEdgeLength(edge, length)
{
	var currentLength = edgeLength(edge);
	var mul = length / currentLength;
	
	var v0 = edge.vertex(0);
	var v1 = edge.vertex(1);
	
	var cx = (v1.x() + v0.x()) / 2.0;
	var cy = (v1.y() + v0.y()) / 2.0;
	var cz = (v1.z() + v0.z()) / 2.0;
	
	v0.setX(cx + (v0.x() - cx) * mul);
	v0.setY(cy + (v0.y() - cy) * mul);
	v0.setZ(cz + (v0.z() - cz) * mul);
	
	v1.setX(cx + (v1.x() - cx) * mul);
	v1.setY(cy + (v1.y() - cy) * mul);
	v1.setZ(cz + (v1.z() - cz) * mul);
}

function sameSize(item)
{
	var edge = item.edgeIterator();
	var firstEdgeLength = 0;
	
    for (edge; !edge.finished(); edge.moveNext())
    {
    	if (edge.selected())
    	{
    		if (firstEdgeLength == 0)
    		{
	    		firstEdgeLength = edgeLength(edge);
    		}
	    	else
	    	{
	 			setEdgeLength(edge, firstEdgeLength);	   		
	    	}
    	}
    }
}

for (var i = 0; i < items.count(); i++)
{
    var item = items.at(i);
    if (item.selected)
        sameSize(item);
}
