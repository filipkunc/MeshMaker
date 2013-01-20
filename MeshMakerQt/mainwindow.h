#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>

class ItemCollection;
class OpenGLManipulatingController;
class OpenGLSceneView;

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    ItemCollection *items;
    OpenGLManipulatingController *itemsController;
    OpenGLSceneView *perspectiveView;
};

#endif // MAINWINDOW_H
