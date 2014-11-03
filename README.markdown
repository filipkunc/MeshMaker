# Welcome to MeshMaker

## How to build it

Just clone and build in XCode.

## License and submodules

MeshMaker is under [MIT license](http://opensource.org/licenses/mit-license.php). You find it in file "LICENSE.TXT". 

MeshMaker uses these third party libraries:

 * [RapidXml](http://rapidxml.sourceforge.net)
 
## About

MeshMaker is very basic modeling tool focused on low poly modeling with triangles and quads. I am working on it in my spare time. It is my hobby project, I am writing it because I always wanted to know how is DCC tools made so I decided that I try to build something small myself.

## Triangles and Quads

MeshMaker supports triangles and quads in same mesh. There is no support for arbitrary polygons.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/triquads.png" alt="Triangles and Quads" width="745px" height="569px"></img>

## Edge loops

MeshMaker enables edge loop selection with Double Click and edge expand selection with Command Double Click. Combinations of triangle/quad extrusion and expanded edge splitting enables simple and fast  modeling.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/edgeloops.png" alt="Edge loops" width="832px" height="653px"></img>

## Camera manipulation

Similar to Maya, Unity. 

* Rotation - Alt + Left Mouse Button
* Pan - Alt + Middle Mouse Button
* Zoom - Alt + Right Mouse Button

Editor can be used also only with multitouch trackpad (MacBooks) and keyboard.

* Rotation - Alt + Two Fingers
* Pan - Control + Alt + Two Fingers
* Zoom - Two Fingers Zoom

## Selection

 * Normal selection - Left Mouse Button
 * Select through (selects back faces for example) - Control + Left Mouse Button
 * Adding to selection - Shift + Left Mouse Button
 * Inverting selection - Command + Left Mouse Button
 * Soft selection - global mode in Edit menu
