#include "../Classes/OpenGLSceneView.h"
#include "../Classes/MyDocument.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("MeshMaker"));

    QMenuBar *menuBar = new QMenuBar;
    menuBar->addMenu(tr("File"));
    menuBar->addMenu(tr("Edit"));
    menuBar->addMenu(tr("View"));
    setMenuBar(menuBar);

    QToolBar *manipulatorToolBar = addToolBar(tr("Manipulator"));
    manipulatorToolBar->addAction(QIcon(":/Icons/SelectTemplate.png"), tr("Select"), this, SLOT(setSelect()));
    manipulatorToolBar->addAction(QIcon(":/Icons/TranslateTemplate.png"), tr("Translate"), this, SLOT(setTranslate()));
    manipulatorToolBar->addAction(QIcon(":/Icons/RotateTemplate.png"), tr("Rotate"), this, SLOT(setRotate()));
    manipulatorToolBar->addAction(QIcon(":/Icons/ScaleTemplate.png"), tr("Scale"), this, SLOT(setScale()));
    manipulatorToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    QToolBar *itemsToolBar = addToolBar(tr("Items"));
    itemsToolBar->addAction(QIcon(":/Icons/PlaneTemplate.png"), tr("Add Plane"), this, SLOT(addPlane()));
    itemsToolBar->addAction(QIcon(":/Icons/CubeTemplate.png"), tr("Add Cube"), this, SLOT(addCube()));
    itemsToolBar->addAction(QIcon(":/Icons/CylinderTemplate.png"), tr("Add Cylinder"), this, SLOT(addCylinder()));
    itemsToolBar->addAction(QIcon(":/Icons/SphereTemplate.png"), tr("Add Sphere"), this, SLOT(addSphere()));
    itemsToolBar->addAction(QIcon(":/Icons/IcosahedronTemplate.png"), tr("Add Icosahedron"), this, SLOT(addIcosahedron()));
    itemsToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);

    perspectiveView = new OpenGLSceneView;
    setCentralWidget(perspectiveView);

    document = new MyDocument();
    document->setViews(perspectiveView, perspectiveView, perspectiveView, perspectiveView);
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::setSelect()
{
    document->setCurrentManipulator(ManipulatorType::Default);
}

void MainWindow::setTranslate()
{
    document->setCurrentManipulator(ManipulatorType::Translation);
}

void MainWindow::setRotate()
{
    document->setCurrentManipulator(ManipulatorType::Rotation);
}

void MainWindow::setScale()
{
    document->setCurrentManipulator(ManipulatorType::Scale);
}

void MainWindow::addPlane()
{
    document->addItem(MeshType::Plane, 0);
}

void MainWindow::addCube()
{
    document->addItem(MeshType::Cube, 0);
}

void MainWindow::addCylinder()
{
    document->addItem(MeshType::Cylinder, 20);
}

void MainWindow::addSphere()
{
    document->addItem(MeshType::Sphere, 20);
}

void MainWindow::addIcosahedron()
{
    document->addItem(MeshType::Icosahedron, 0);
}
