#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../PureCpp/Enums.h"
#include "../PureCpp/MathCore/Camera.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Diagnostics;

#include "Mesh.h"

namespace ManagedCpp
{
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

		HDC deviceContext;
		HGLRC glRenderingContext;
		
		/*id<OpenGLManipulating> displayed;
		id<OpenGLManipulating> manipulated;
		id<OpenGLSceneViewDelegate> delegate;*/
		
		Vector3D *selectionOffset;
		Camera *camera;
		Vector3D *perspectiveRadians;
		PointF lastPoint;
		PointF currentPoint;
		CocoaBool isManipulating;
		CocoaBool isSelecting;
		/*Manipulator *defaultManipulator;
		Manipulator *translationManipulator;
		Manipulator *rotationManipulator;
		Manipulator *scaleManipulator;
		Manipulator *currentManipulator;*/
		
		enum CameraMode cameraMode;
		enum ViewMode viewMode;

		Mesh ^testMesh;
	public:
		OpenGLSceneView();
	protected:
		~OpenGLSceneView();

		virtual void OnLoad(EventArgs ^e) override;
		virtual void OnSizeChanged(EventArgs ^e) override;
		virtual void OnPaintBackground(PaintEventArgs ^e) override;
		virtual void OnPaint(PaintEventArgs ^e) override;
		virtual void OnMouseMove(MouseEventArgs ^e) override;
		virtual void OnMouseDown(MouseEventArgs ^e) override;
		virtual void OnMouseUp(MouseEventArgs ^e) override;
		virtual void OnMouseWheel(MouseEventArgs ^e) override;
		void DrawGrid(int size, int step);
		void RenderGL();
		void BeginGL();
		void EndGL();
		void InitializeGL();
		void ResizeGL();			
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
