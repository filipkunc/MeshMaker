//
//  OpenGLSceneView.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 6/29/09.
//  For license see LICENSE.TXT
//

#pragma once

#include "OpenGLSceneViewCore.h"

#if defined(__APPLE__)

@class OpenGLSceneView;

@protocol OpenGLSceneViewDelegate

- (void)manipulationStartedInView:(OpenGLSceneView *)view;
- (void)manipulationEndedInView:(OpenGLSceneView *)view;
- (void)selectionChangedInView:(OpenGLSceneView *)view;

@property (readonly) NSColor *brushColor;
@property (readonly) float brushSize;
@property (readonly) BOOL texturePaintEnabled;
@property (readonly) BOOL addVertexEnabled;

- (void)addVertex:(Vector3D)position fromCamera:(Camera *)camera;

@end

@interface OpenGLSceneView : NSOpenGLView
{
    OpenGLSceneViewCore *_coreView;
    IOpenGLSceneViewCoreDelegate *_coreDelegate;
    id<OpenGLSceneViewDelegate> __weak delegate;
}

@property (readwrite, assign) IOpenGLManipulating *displayed;
@property (readwrite, assign) IOpenGLManipulating *manipulated;
@property (readwrite, weak) id<OpenGLSceneViewDelegate> delegate;
@property (readwrite, assign) enum ManipulatorType currentManipulator;
@property (readwrite, assign) enum CameraMode cameraMode;
@property (readwrite, assign) Camera camera;

@end

#elif defined(WIN32)

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Diagnostics;
using namespace Chocolate;

namespace MeshMakerCppCLI
{
	ref class OpenGLSceneView;

	public interface class IOpenGLSceneViewDelegate
	{
		void manipulationStartedInView(OpenGLSceneView ^view);
		void manipulationEndedInView(OpenGLSceneView ^view);
		void selectionChangedInView(OpenGLSceneView ^view);
	};

	/// <summary>
	/// Summary for OpenGLSceneView
	/// </summary>
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	public ref class OpenGLSceneView : public System::Windows::Forms::UserControl
	{
	private:
		System::ComponentModel::Container^ components;

		HDC _deviceContext;
		HGLRC _glRenderingContext;

		OpenGLSceneViewCore *_coreView;
		IOpenGLSceneViewCoreDelegate *_coreDelegate;

	public:
		OpenGLSceneView();

		void BeginGL();
		void EndGL();

		OpenGLSceneViewCore *coreView();

 		property CameraMode CurrentCameraMode 
		{
			CameraMode get() { return _coreView->cameraMode(); }
			void set(CameraMode value) { _coreView->setCameraMode(value); }
		}
		
		property IOpenGLSceneViewDelegate ^Delegate;
		property OpenGLSceneView ^SharedContextView;

	protected:
		~OpenGLSceneView();

		void InitializeGL();
		NSPoint LocationFromEvent(MouseEventArgs ^e);
		
		virtual void OnLoad(EventArgs ^e) override;
		virtual void OnSizeChanged(EventArgs ^e) override;
		virtual void OnPaintBackground(PaintEventArgs ^e) override;
		virtual void OnPaint(PaintEventArgs ^e) override;
		virtual void OnMouseMove(MouseEventArgs ^e) override;
		virtual void OnMouseDown(MouseEventArgs ^e) override;
		virtual void OnMouseDoubleClick(MouseEventArgs ^e) override;
		virtual void OnMouseUp(MouseEventArgs ^e) override;
		virtual void OnMouseWheel(MouseEventArgs ^e) override;
	private:
#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};
}

#elif defined(__linux__)

#include <QtGui>
#include <QtOpenGL>
#include <QGLWidget>

class OpenGLSceneView : public QGLWidget, public IOpenGLSceneViewCoreDelegate
{
    //Q_OBJECT

public:
    OpenGLSceneView(QWidget *parent = 0);
    ~OpenGLSceneView();

    OpenGLSceneViewCore *coreView();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    virtual NSRect bounds();
    virtual void setNeedsDisplay();
    virtual void manipulationStarted();
    virtual void manipulationEnded();
    virtual void selectionChanged();
    virtual bool texturePaintEnabled();
    virtual void makeCurrentContext();   

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    NSPoint locationFromMouseEvent(QMouseEvent *event);

private:
    OpenGLSceneViewCore *_coreView;
};

#endif
