#include "../Classes/ItemCollection.h"
#include "../Classes/OpenGLManipulatingController.h"
#include "../Classes/OpenGLSceneView.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("MeshMaker"));

    perspectiveView = new OpenGLSceneView;
    setCentralWidget(perspectiveView);

    items = new ItemCollection();
    itemsController = new OpenGLManipulatingController();

    itemsController->setModel(items);

    Mesh2 *mesh = new Mesh2();
    mesh->makeCube();
    mesh->loopSubdivision();

    items->addItem(new Item(mesh));
    items->itemAtIndex(0)->selected = true;

    itemsController->updateSelection();

    perspectiveView->coreView()->setManipulated(itemsController);
    perspectiveView->coreView()->setDisplayed(itemsController);
    perspectiveView->coreView()->setCurrentManipulator(ManipulatorType::Translation);
}

MainWindow::~MainWindow()
{
    
}
