# Welcome to OpenGL Editor

## License

This project is under MIT license. You find it in file "LICENSE.TXT".

## About

OpenGL Editor is my hobby project. Goal is to create fast and powerful tool for basic low poly modeling on Mac OS X and Windows. Project is currently in very early stages and many critical pieces are missing. Any ideas, help or wishes is welcome!

[Modeling chess tower video on Objective-C++](http://www.youtube.com/watch?v=57d63xcT21Y)

[Modeling chess tower video on C# + C++/CLI](http://www.youtube.com/watch?v=7iddjSQ3Uj8) 

### Compiling on Windows

Microsoft doesn't allow to mix C# and C++ projects in same solution in free versions of Visual Studio 2008. So I can't set project references right. To workaround this problem I have two solutions ManagedCpp.sln and OpenGLEditorWindows.sln. To keep things more complicated than it needs to be I have this build order:

* **HotChocolate.csproj** *(C# dll - it is like Cocoa for .NET)*
* **HotChocolateTest.csproj** *(C# exe - showcase of HotChocolate)*
* **ManagedCpp.vcproj** *(C++/CLI exe, depends on HotChocolate, most code is here)*
* **OpenGLEditorWindows.csproj** *(C# project - executable, depends on ManagedCpp and HotChocolate)*

So the steps are:

1. Open both ManagedCpp.sln and OpenGLEditorWindows.sln
2. Build All in OpenGLEditorWindows.sln
3. Build All in ManagedCpp.sln
4. Build All in OpenGLEditorWindows.sln

Now everything should be fine.

### Compiling on Mac

Just open OpenGLEditor.xcodeproj in Xcode and build all. I currently build only for
Intel 64 bit on Snow Leopard.

### Implemented features

* Vertex, Edge, Triangle manipulation (move, rotate, scale)
* Edge turning and splitting
* Vertex and mesh merging
* Merge vertex pairs (good for mirrored models)
* Cube, cylinder, sphere primitives
* Save and load
* Solid and wireframe view
* Perspective, top, left and other cameras
* Main part of porting to Windows (C++/CLI + C#)
* Undo and redo support (only merging isn't correct now)
* Basic extrusion (⌘C or Ctrl+C in triangle mode)

### Future plans

* Rendering optimizations
* Selection optimizations
* Freeze and hide selection
* Vertex and mesh splitting
* Texturing
* Background image for each view different
* More advanced tubing and extrusion
* Import and export for various formats like: OBJ, 3DS, ASE, COLLADA, Blender, MilkShape 3D, and many more...
* More features related to game development and level editing
* Porting to Linux (if I find some good distro and IDE, I am Unix/Linux noob)
* And more...

### World manipulation

* ⌥ or Alt + middle mouse => Rotate world
* Middle mouse => Pan world
* Mouse wheel => Zoom world

Should I change that for new Magic Mouse? I don't have it now, but it is possible to completely remove need for middle mouse and use Alt + left mouse or Alt + right mouse.
