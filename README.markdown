# Welcome to MeshMaker

## How to build it

Well project contains hardcoded paths in Mac version, I don't know how to fix it properly to $(SRCROOT) so I give this tutorial, which works for me:

    cd ~
    git clone https://github.com/filipkunc/MeshMaker.git
    cd MeshMaker
    git submodule init
    git submodule update
    cd Submodules/OpenSubdiv
    cmake -G "Xcode"
    cd ../..
    open MeshMaker.xcworkspace

Windows version needs to have build OpenSubdiv first by OpenSubdiv.sln. Then MeshMaker.sln works. If there is also hardcoded paths, use generate it with cmake -G switch.

Linux version is just my learning of Qt Creator and Ubuntu, so I just knew that I depend on glew installed, feel free to improve Linux port, because I am much more experienced in Windows/Mac development than Linux.

## License and submodules

MeshMaker is under [MIT license](http://opensource.org/licenses/mit-license.php). You find it in file "LICENSE.TXT". 

MeshMaker uses these third party libraries:

 * [OpenSubdiv library](http://graphics.pixar.com/opensubdiv) from Pixar Animation Studios under [Ms-PL license](http://www.microsoft.com/en-us/openness/licenses.aspx#MPL). Library is slightly modified for this project, modifications are at [https://github.com/filipkunc/OpenSubdiv](https://github.com/filipkunc/OpenSubdiv).
 * [Fragaria editor](http://www.mugginsoft.com/code/fragaria). Library is slightly modified for this project, modifications are at [https://github.com/filipkunc/Fragaria](https://github.com/filipkunc/Fragaria). It is used just for Mac version of script editor
 * [RapidXml](http://rapidxml.sourceforge.net)
 * [glew](http://glew.sourceforge.net) for Windows and Linux port
 * [FastColoredTextBox](http://www.codeproject.com/Articles/161871/Fast-Colored-TextBox-for-syntax-highlighting) under LGPLv3. It is used as dll just for Windows script editor.
 * [Noesis.Javascript](http://javascriptdotnet.codeplex.com/documentation) under New BSD License. It is used as dll just for Windows script editor as v8 engine wrapper for execution of JavaScript user scripts.
 
For working with submodules I recommend reading [Pro Git chapter about submodules](http://git-scm.com/book/en/Git-Tools-Submodules). But in short to get full repo you need:

    git clone https://github.com/filipkunc/MeshMaker.git
    cd MeshMaker
    git submodule init
    git submodule update

## About

MeshMaker is very basic modeling tool focused on low poly modeling with triangles and quads. I am working on it in my spare time. It is my hobby project, I am writing it because I always wanted to know how is DCC tools made so I decided that I try to build something small myself.

My personal goal is to make it viable for intuitive and fast work, something like 3D equivalent of [Paint.NET](http://www.getpaint.net).

Project started as Objective C project, because I wanted to learn Mac programming, after several years programming on Windows in C# and C++. Later I decided to port it (twice). Current rewrite is mainly C++ spiced with `#if defined(__APPLE__)` or `#if defined(WIN32)` for platform specific tasks. I like to use platform strongest language for GUI so Mac version is Objective C++ with Cocoa and Windows version is C# and C++/CLI with Windows Forms libraries.

For learning Linux development I tried Ubuntu with Qt and Qt Creator IDE, it allows me to share C++ codebase a lot.

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

## Windows port

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/triquads_win.png" alt="Triangles and Quads" width="715px" height="541px"></img>

## Linux port

<img src="https://github.com/filipkunc/MeshMaker/raw/master/Screenshots/LinuxQt.png" alt="Linux Qt" width="744px" height="494px"></img>
 
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
