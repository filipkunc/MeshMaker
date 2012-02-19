# Welcome to OpenGL Editor

## License

This project is under MIT license. You find it in file "LICENSE.TXT". OpenGL Editor contains [RapidXml parser](http://rapidxml.sourceforge.net/) from Marcin Kalicinski. RapidXml parser is also under [MIT license](http://rapidxml.sourceforge.net/license.txt).

## About

OpenGL Editor is very basic modeling tool focused on low poly modeling with triangles. 
Currently Mesh class is being re-implemented in Mesh2 for faster modeling operations and easier
implementation of advanced features (subdivision, splitting, extrusion, ...).

## Screenshots

### Chess tower

Modeled with extrude (⌘D shortcut, same as duplicate).

<img src="https://github.com/filipkunc/opengl-editor-cocoa/raw/master/Screenshots/chesstower.png" alt="Chess tower" width="902px" height="597px"></img>

### Loop subdivision

And same chess tower after four or five Loop subdivision iterations.

<img src="https://github.com/filipkunc/opengl-editor-cocoa/raw/master/Screenshots/loopsubdivision.png" alt="Loop subdivision" width="902px" height="597px"></img>

### Texture painting

Texturing is currently work in progress.

<img src="https://github.com/filipkunc/opengl-editor-cocoa/raw/master/Screenshots/texturepainting.png" alt="Texture painting" width="903px" height="667px"></img>


## Videos

 * [Chess tower modeling](http://youtu.be/57d63xcT21Y)
 * [Subdivision and soft selection](http://youtu.be/65whhpqHgO4)
 * [Texture painting](http://youtu.be/nCdlcOvYr-c)

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
