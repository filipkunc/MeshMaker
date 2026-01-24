# External Dependencies

This folder contains external libraries needed to build MeshMaker WebGL2.

## Setup Instructions

### 1. GLAD (OpenGL Loader) - Desktop only

Generate GLAD loader from https://glad.dav1d.de/ with these settings:
- Language: C/C++
- Specification: OpenGL
- Profile: Core
- API gl: Version 3.3
- Extensions: None needed

Download and extract to `external/glad/`:
```
glad/
├── include/
│   ├── glad/
│   │   └── glad.h
│   └── KHR/
│       └── khrplatform.h
└── src/
    └── glad.c
```

### 2. GLM (OpenGL Mathematics)

Download from https://github.com/g-truc/glm/releases

Extract the `glm` folder to `external/glm/`:
```
glm/
└── glm/
    ├── glm.hpp
    ├── common.hpp
    ├── vec3.hpp
    └── ... (other headers)
```

### 3. Dear ImGui (Docking Branch)

Clone or download from https://github.com/ocornut/imgui/tree/docking

Copy the following files to `external/imgui/`:
```
imgui/
├── imgui.h
├── imgui.cpp
├── imgui_demo.cpp
├── imgui_draw.cpp
├── imgui_internal.h
├── imgui_tables.cpp
├── imgui_widgets.cpp
├── imconfig.h
├── imstb_rectpack.h
├── imstb_textedit.h
├── imstb_truetype.h
└── backends/
    ├── imgui_impl_glfw.h
    ├── imgui_impl_glfw.cpp
    ├── imgui_impl_opengl3.h
    ├── imgui_impl_opengl3.cpp
    └── imgui_impl_opengl3_loader.h
```

## Quick Setup Script (PowerShell)

```powershell
# Create directories
New-Item -ItemType Directory -Force -Path "glad/include/glad", "glad/include/KHR", "glad/src"
New-Item -ItemType Directory -Force -Path "glm"
New-Item -ItemType Directory -Force -Path "imgui/backends"

# Download GLM (header-only)
Invoke-WebRequest -Uri "https://github.com/g-truc/glm/releases/download/1.0.1/glm-1.0.1-light.zip" -OutFile "glm.zip"
Expand-Archive -Path "glm.zip" -DestinationPath "." -Force
Remove-Item "glm.zip"

# For GLAD and ImGui, manual download is recommended
Write-Host "Please download GLAD from https://glad.dav1d.de/"
Write-Host "Please download ImGui docking branch from https://github.com/ocornut/imgui/tree/docking"
```

## Verification

After setup, your directory structure should look like:
```
external/
├── glad/
│   ├── include/
│   │   ├── glad/glad.h
│   │   └── KHR/khrplatform.h
│   └── src/glad.c
├── glm/
│   └── glm/
│       └── glm.hpp
├── imgui/
│   ├── imgui.h
│   ├── imgui.cpp
│   └── backends/
│       ├── imgui_impl_glfw.h
│       └── imgui_impl_opengl3.h
└── README.md
```
