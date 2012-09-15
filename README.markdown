# Welcome to MeshMaker

## License

MeshMaker is under [MIT license](http://opensource.org/licenses/mit-license.php). You find it in file "LICENSE.TXT". 

MeshMaker uses:

 * [OpenSubdiv library](http://graphics.pixar.com/opensubdiv) from Pixar Animation Studios under [Ms-PL license](http://www.microsoft.com/en-us/openness/licenses.aspx#MPL). Library is slightly modified for this project, modifications are at [https://github.com/filipkunc/OpenSubdiv](https://github.com/filipkunc/OpenSubdiv).
 * [Open Asset Import Library](http://assimp.sourceforge.net/) from assimp team under [3-clause BSD license](http://assimp.sourceforge.net/main_license.html). Library is slightly modified for this project, modifications are at [https://github.com/filipkunc/assimp](https://github.com/filipkunc/assimp)
 

## About

MeshMaker is very basic modeling tool focused on low poly modeling with triangles and now also quads.

## Screenshots

### Chess tower

Modeled with extrusion.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/chesstower.png" alt="Chess tower" width="902px" height="597px"></img>

### Loop subdivision

And same chess tower after four or five Loop subdivision iterations. This is before OpenSubdiv.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/loopsubdivision.png" alt="Loop subdivision" width="902px" height="597px"></img>

### Texture painting

Texturing needs a lot of work.

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/texturepainting.png" alt="Texture painting" width="785px" height="577px"></img>

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
