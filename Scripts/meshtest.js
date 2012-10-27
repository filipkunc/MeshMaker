// basic test - needs selected vertices in vertex mode

for (var i = 0; i < items.count(); i++)
{
    var item = items.at(i);
    var it = item.vertexIterator();
    for (it; !it.finished(); it.moveNext())
    {
        if (it.selected())
        {
            //alert(it.positionX().toString());
            it.setX(it.x() + 2.0);
        }
    }
}
