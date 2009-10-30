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
#include "OpenGLManipulating.h"

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
		
		OpenGLManipulating ^displayed;
		OpenGLManipulating ^manipulated;
		//OpenGLSceneViewDelegate
		
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
		void ApplyProjection(RectangleF baseRect);
		void BeginOrtho();
		void EndOrtho();
		
		void Select(
			double x, 
			double y, 
			double width, 
			double height,
			OpenGLSelecting ^selecting,
			CocoaBool nearestOnly,
			OpenGLSelectionMode selectionMode);

		void Select(
			PointF point, 
			OpenGLSelecting ^selecting, 
			OpenGLSelectionMode selectionMode);

		void Select(
			RectangleF rect,
			OpenGLSelecting ^selecting,
			OpenGLSelectionMode selectionMode);

		Vector3D GetPositionInSpace(PointF point);
		void DrawSelectionPlane(int index);
		Vector3D GetPositionOnAxis(Axis axis, PointF point);
		Vector3D GetPositionOnRotatedAxis(Axis axis, PointF point, Quaternion rotation);
		Vector3D GetPositionOnPlane(PlaneAxis plane, PointF point);

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
