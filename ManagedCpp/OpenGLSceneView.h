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
#include "Manipulator.h"

namespace ManagedCpp
{
	public interface class OpenGLSceneViewDelegate
	{
		void ManipulationStarted();
		void ManipulationEnded();
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

		HDC deviceContext;
		HGLRC glRenderingContext;
		
		OpenGLManipulating ^displayed;
		OpenGLManipulating ^manipulated;
		OpenGLSceneViewDelegate ^theDelegate;
		
		Vector3D *selectionOffset;
		Camera *camera;
		Vector3D *perspectiveRadians;
		PointF lastPoint;
		PointF currentPoint;
		CocoaBool isManipulating;
		CocoaBool isSelecting;
		Manipulator ^defaultManipulator;
		Manipulator ^translationManipulator;
		Manipulator ^rotationManipulator;
		Manipulator ^scaleManipulator;
		Manipulator ^currentManipulator;
		
		CameraMode cameraMode;
		ViewMode viewMode;
	public:
		OpenGLSceneView();

		[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
		property ManipulatorType CurrentManipulator 
		{ 
			ManipulatorType get();
			void set(ManipulatorType value); 
		}
		
		[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
		property CameraMode CurrentCameraMode 
		{ 
			CameraMode get(); 
			void set(CameraMode value); 
		}
		
		[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
		property ViewMode CurrentViewMode 
		{ 
			ViewMode get(); 
			void set(ViewMode value); 
		}
		
		[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
		property OpenGLManipulating ^Displayed 
		{ 
			OpenGLManipulating ^get();
			void set(OpenGLManipulating ^value); 
		}
		
		[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
		property OpenGLManipulating ^Manipulated 
		{ 
			OpenGLManipulating ^get(); 
			void set(OpenGLManipulating ^value); 
		}

		[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
		property OpenGLSceneViewDelegate ^TheDelegate 
		{ 
			OpenGLSceneViewDelegate ^get();
			void set(OpenGLSceneViewDelegate ^value); 
		}
		
		[DesignerSerializationVisibility(DesignerSerializationVisibility::Hidden)]
		property RectangleF CurrentRect { RectangleF get(); }

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
		
		void SelectBox(
			RectangleF rect,
			OpenGLSelecting ^selecting,
			CocoaBool nearestOnly,
			OpenGLSelectionMode selectionMode);

		void SelectPoint(
			PointF point, 
			OpenGLSelecting ^selecting, 
			OpenGLSelectionMode selectionMode);

		void SelectRect(
			RectangleF rect,
			OpenGLSelecting ^selecting,
			OpenGLSelectionMode selectionMode);

		Vector3D GetPositionInSpace(PointF point);
		void DrawSelectionPlane(int index);
		Vector3D GetPositionOnAxis(Axis axis, PointF point);
		Vector3D GetPositionOnRotatedAxis(Axis axis, PointF point, Quaternion rotation);
		Vector3D GetPositionOnPlane(PlaneAxis plane, PointF point);

		Vector3D GetTranslation(PointF point);
		Vector3D GetScale(PointF point, Vector3D *lastPosition);
		Quaternion GetRotation(PointF point, Vector3D *lastPosition);
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
