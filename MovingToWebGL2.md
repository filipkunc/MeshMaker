# Plan to move to WebGL2

## Current state in the active branch

Current codebase is currently working on Windows and contains C++/CLI code and C# code.
The C++/CLI code is in MeshMakerCppCLI project with Classes using confusingly the `.mm` extension from Objective-C++, but actually being compiled only for Windows C++/CLI so standard `.cpp` would be easier to follow.

## Road to WebGL2

Easiest would be to switch the code to pure C++ and OpenGL profile closest to WebGL2 with CMake project. This would allow to do a separate second step using Emscripten as a compiler target which would finally be using the WebGL2.
The steps would need to be incremental to reduce the complexity of the port and also to verify each stage to make sure it works.

My recommendation would be to make a separate folder with the CMake setup and clean room setup code using the OpenGL/WebGL2 and then incrementally port the code there.
