# Welcome to OpenGL Editor

## License

This project is under MIT license. You find it in file "LICENSE.TXT".

## About

OpenGL Editor is very basic modeling tool focused on low poly modeling with triangles.

[Modeling chess tower video](http://www.youtube.com/watch?v=57d63xcT21Y)

### Project is now divided into three repos.

* [opengl-editor-cocoa](http://github.com/filipkunc/opengl-editor-cocoa/) 
this git repo aims to be Mac only project for low poly modeling
* [3d-editor-toolkit](http://code.google.com/p/3d-editor-toolkit/)
this svn repo aims to be Windows/Mac project for others to be able to create similar editor,
it will not contain modeling or Bullet stuff
* [bullet-physics-editor](http://code.google.com/p/bullet-physics-editor/)
this svn repo aims to be Windows/Mac project to enable editing and working with Bullet files

### Compiling

Just open OpenGLEditor.xcodeproj in Xcode and build all. 

### Implemented features

* Vertex, Edge, Triangle manipulation (translate, rotate, scale)
* Edge turning and splitting
* Vertex and mesh merging
* Merge vertex pairs (good for mirrored models)
* Cube, cylinder, sphere primitives
* Save and load
* Solid and wireframe view
* Four views
* Full undo and redo support, currently unlimited
* Basic extrusion (⌘C in triangle mode)

### Future plans

* Issues on GitHub
* Wiki documentation

### Camera manipulation

Similar to Maya, Unity. 

* Rotation - Option + Left Mouse Button
* Pan - Option + Middle Mouse Button
* Zoom - Option + Right Mouse Button

Editor can be used also only with multitouch trackpad (MacBooks) and keyboard.

* Rotation - Option + Two Fingers
* Pan - Control + Option + Two Fingers
* Zoom - Two Fingers Zoom

