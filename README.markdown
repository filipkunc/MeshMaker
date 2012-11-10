# Welcome to MeshMaker

## License and submodules

MeshMaker is under [MIT license](http://opensource.org/licenses/mit-license.php). You find it in file "LICENSE.TXT". 

MeshMaker uses:

 * [OpenSubdiv library](http://graphics.pixar.com/opensubdiv) from Pixar Animation Studios under [Ms-PL license](http://www.microsoft.com/en-us/openness/licenses.aspx#MPL). Library is slightly modified for this project, modifications are at [https://github.com/filipkunc/OpenSubdiv](https://github.com/filipkunc/OpenSubdiv).
 * [Fragaria editor](http://www.mugginsoft.com/code/fragaria) Library is slightly modified for this project, modifications are at [https://github.com/filipkunc/Fragaria](https://github.com/filipkunc/Fragaria)
 
For working with submodules I recommend reading [Pro Git chapter about submodules](http://git-scm.com/book/en/Git-Tools-Submodules).

## About

MeshMaker is very basic modeling tool focused on low poly modeling with triangles and quads.

## Triangles and Quads

MeshMaker supports triangles and quads in same mesh. There is no support for arbitrary polygons.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/triquads.png" alt="Triangles and Quads" width="745px" height="569px"></img>

## Edge loops

MeshMaker enables edge loop selection with Double Click and edge expand selection with Command Double Click. Combinations of triangle/quad extrusion and expanded edge splitting enables simple and fast  modeling.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/edgeloops.png" alt="Edge loops" width="832px" height="653px"></img>

## Scripting

For scripting is used JavaScript via WebScriptObject and Fragaria editor for editing code. 
Example scripts are in Scripts folder.

All script actions are undoable.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/scripting.png" alt="Scripting" width="833px" height="652px"></img>
 
## Camera manipulation

Similar to Maya, Unity. 

* Rotation - Option + Left Mouse Button
* Pan - Option + Middle Mouse Button
* Zoom - Option + Right Mouse Button

Editor can be used also only with multitouch trackpad (MacBooks) and keyboard.

* Rotation - Option + Two Fingers
* Pan - Control + Option + Two Fingers
* Zoom - Two Fingers Zoom

## Selection

 * Normal selection - Left Mouse Button
 * Select through (selects back faces for example) - Control + Left Mouse Button
 * Adding to selection - Shift + Left Mouse Button
 * Inverting selection - Command + Left Mouse Button
 * Soft selection - global mode in Edit menu
