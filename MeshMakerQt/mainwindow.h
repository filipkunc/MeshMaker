#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class ItemCollection;
class OpenGLManipulatingController;
class OpenGLSceneView;
class MyDocument;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void setSelect();
    void setTranslate();
    void setRotate();
    void setScale();

    void addPlane();
    void addCube();
    void addCylinder();
    void addSphere();
    void addIcosahedron();
signals:


private:
    MyDocument *document;
    OpenGLSceneView *perspectiveView;
};

#endif // MAINWINDOW_H
