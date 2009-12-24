# Welcome to OpenGL Editor

## License

This project is under MIT license. You find it in file "LICENSE.TXT".

## About

OpenGL Editor is my hobby project. Goal is to create fast and powerful tool for basic low poly modeling on Mac OS X and Windows. Project is currently in very early stages and many critical pieces are missing. Any ideas, help or wishes is welcome!

[Modeling chess tower video on Objective-C++](http://www.youtube.com/watch?v=57d63xcT21Y)

[Modeling chess tower video on C# + C++/CLI](http://www.youtube.com/watch?v=7iddjSQ3Uj8) 

Project is now also on [Google Code](http://code.google.com/p/3d-editor-toolkit) as Subversion repository. Another goal is to make code more re-usable for other people to let them write their own editors with some basic framework.

### Compiling on Windows

If you have Visual Studio 2008 Standard, Professional or higher you can use CombinedEditorWindows.sln, thanks to Erwin Coumans.

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

Just open OpenGLEditor.xcodeproj in Xcode and build all. It builds as universal binary x86_64 for Leopard 10.5 SDK. Works in Snow Leopard and Leopard. Unit tests needs Snow Leopard for Objective-C 2.1.

### Implemented features

* Vertex, Edge, Triangle manipulation (translate, rotate, scale)
* Edge turning and splitting
* Vertex and mesh merging
* Merge vertex pairs (good for mirrored models)
* Cube, cylinder, sphere primitives
* Save and load (universal format for 32/64 bit versions across Mac and Windows)
* Solid and wireframe view
* Four views
* Ported to Windows (C++/CLI + C#)
* Full undo and redo support, currently unlimited
* Basic extrusion (⌘C or Ctrl+C in triangle mode)

### Future plans

* Issues on GitHub and Google Code
* Wiki documentation

### Camera manipulation

I will change this to Maya like manipulation, but for now:

* ⌥ or Alt + middle mouse => Rotate world
* Middle mouse => Pan world
* Mouse wheel => Zoom world
