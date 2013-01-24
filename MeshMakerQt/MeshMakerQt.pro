#-------------------------------------------------
#
# Project created by QtCreator 2013-01-19T10:16:50
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = MeshMakerQt
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    ../Classes/Vector4D.cpp \
    ../Classes/Vector3D.cpp \
    ../Classes/Vector2D.cpp \
    ../Classes/Quaternion.cpp \
    ../Classes/Matrix4x4.cpp \
    ../Classes/Camera.cpp \
    ../Classes/Item.cpp \
    ../Classes/ItemCollection.cpp \
    ../Classes/Manipulator.cpp \
    ../Classes/ManipulatorWidget.cpp \
    ../Classes/MemoryStream.cpp \
    ../Classes/Mesh2.drawing.cpp \
    ../Classes/Mesh2.make.cpp \
    ../Classes/Mesh2.cpp \
    ../Classes/MeshHelpers.cpp \
    ../Classes/OpenGLDrawing.cpp \
    ../Classes/OpenGLManipulatingController.cpp \
    ../Classes/Shader.cpp \
    ../Classes/ShaderProgram.cpp \
    ../Classes/Triangle.cpp \
    ../Classes/OpenGLSceneViewCore.cpp \
    ../Classes/OpenGLSceneView.cpp \
    ../Classes/MyDocument+archiving.cpp \
    ../Classes/MyDocument.cpp

HEADERS  += mainwindow.h \
    ../Classes/VertexEdge.h \
    ../Classes/Vertex.h \
    ../Classes/Vector4D.h \
    ../Classes/Vector3D.h \
    ../Classes/Vector2D.h \
    ../Classes/Triangle.h \
    ../Classes/SimpleNodeAndList.h \
    ../Classes/ShaderProgram.h \
    ../Classes/Shader.h \
    ../Classes/rapidxml_utils.hpp \
    ../Classes/rapidxml_print.hpp \
    ../Classes/rapidxml_iterators.hpp \
    ../Classes/rapidxml.hpp \
    ../Classes/Quaternion.h \
    ../Classes/OpenGLSelecting.h \
    ../Classes/OpenGLManipulatingModel.h \
    ../Classes/OpenGLManipulatingController.h \
    ../Classes/OpenGLManipulating.h \
    ../Classes/OpenGLDrawing.h \
    ../Classes/MeshHelpers.h \
    ../Classes/MeshForwardDeclaration.h \
    ../Classes/Mesh2.h \
    ../Classes/MemoryStreaming.h \
    ../Classes/MemoryStream.h \
    ../Classes/Matrix4x4.h \
    ../Classes/MathForwardDeclaration.h \
    ../Classes/MathDeclaration.h \
    ../Classes/ManipulatorWidget.h \
    ../Classes/Manipulator.h \
    ../Classes/ItemCollection.h \
    ../Classes/Item.h \
    ../Classes/FPNode.h \
    ../Classes/FPList.h \
    ../Classes/FPArrayCache.h \
    ../Classes/Enums.h \
    ../Classes/Camera.h \
    ../Classes/OpenGLSceneViewCore.h \
    ../Classes/OpenGLSceneView.h \
    ../Classes/MyDocument.h

QMAKE_CXXFLAGS += -std=c++0x

LIBS += -L/usr/local/lib -lGLU -lGLEW

RESOURCES += \
    resources.qrc

OTHER_FILES +=
