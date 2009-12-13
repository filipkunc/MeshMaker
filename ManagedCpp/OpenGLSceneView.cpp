// ManagedCpp.cpp : main project file.

#include "OpenGLSceneView.h"
#include "../PureCpp/OpenGLDrawing.h"

namespace ManagedCpp {

	const float perspectiveAngle = 45.0f;
	const float minDistance = 0.2f;
	const float maxDistance = 1000.0f;

	OpenGLSceneView::OpenGLSceneView()
	{
		InitializeComponent();

		displayed = nullptr;
		manipulated = nullptr;
		theDelegate = nullptr;

		selectionOffset = new Vector3D();
		isManipulating = NO;
		isSelecting = NO;

		camera = new Camera();
		camera->SetRadX(-45.0f * DEG_TO_RAD);
		camera->SetRadY(45.0f * DEG_TO_RAD);
		camera->SetZoom(20.0f);

		perspectiveRadians = new Vector3D(camera->GetRadians());

		lastPoint = PointF(0, 0);

		cameraMode = CameraMode::CameraModePerspective;
		viewMode = ViewMode::ViewModeSolid;

		defaultManipulator = gcnew Manipulator();
		defaultManipulator->AddWidget(gcnew ManipulatorWidget(AxisX, WidgetLine));
		defaultManipulator->AddWidget(gcnew ManipulatorWidget(AxisY, WidgetLine));
		defaultManipulator->AddWidget(gcnew ManipulatorWidget(AxisZ, WidgetLine));
		
		translationManipulator = gcnew Manipulator();
		translationManipulator->AddWidget(gcnew ManipulatorWidget(AxisX, WidgetArrow));
		translationManipulator->AddWidget(gcnew ManipulatorWidget(AxisY, WidgetArrow));
		translationManipulator->AddWidget(gcnew ManipulatorWidget(AxisZ, WidgetArrow));
		translationManipulator->AddWidget(gcnew ManipulatorWidget(AxisX, WidgetPlane));
		translationManipulator->AddWidget(gcnew ManipulatorWidget(AxisY, WidgetPlane));
		translationManipulator->AddWidget(gcnew ManipulatorWidget(AxisZ, WidgetPlane));

		rotationManipulator = gcnew Manipulator();
		rotationManipulator->AddWidget(gcnew ManipulatorWidget(AxisX, WidgetCircle));
		rotationManipulator->AddWidget(gcnew ManipulatorWidget(AxisY, WidgetCircle));
		rotationManipulator->AddWidget(gcnew ManipulatorWidget(AxisZ, WidgetCircle));
		
		scaleManipulator = gcnew Manipulator();
		scaleManipulator->AddWidget(gcnew ManipulatorWidget(Center, WidgetCube));
		scaleManipulator->AddWidget(gcnew ManipulatorWidget(AxisX, WidgetCube));
		scaleManipulator->AddWidget(gcnew ManipulatorWidget(AxisY, WidgetCube));
		scaleManipulator->AddWidget(gcnew ManipulatorWidget(AxisZ, WidgetCube));

		currentManipulator = defaultManipulator;
	}
	
	OpenGLSceneView::~OpenGLSceneView()
	{
		this->EndGL();
		delete perspectiveRadians;
		delete selectionOffset;
		delete camera;
		if (components)
		{
			delete components;
		}
	}

	OpenGLManipulating ^OpenGLSceneView::Displayed::get()
	{
		return displayed;
	}

	void OpenGLSceneView::Displayed::set(OpenGLManipulating ^value)
	{
		displayed = value;
	}

	OpenGLManipulating ^OpenGLSceneView::Manipulated::get()
	{
		return manipulated;
	}

	void OpenGLSceneView::Manipulated::set(OpenGLManipulating ^value)
	{
		manipulated = value;
	}

	OpenGLSceneViewDelegate ^OpenGLSceneView::TheDelegate::get()
	{
		return theDelegate;
	}

	void OpenGLSceneView::TheDelegate::set(OpenGLSceneViewDelegate ^value)
	{
		theDelegate = value;
	}

	ManipulatorType OpenGLSceneView::CurrentManipulator::get()
	{
		if (currentManipulator == defaultManipulator)
			return ManipulatorType::ManipulatorTypeDefault;
		if (currentManipulator == translationManipulator)
			return ManipulatorType::ManipulatorTypeTranslation;
		if (currentManipulator == rotationManipulator)
			return ManipulatorType::ManipulatorTypeRotation;
		if (currentManipulator == scaleManipulator)
			return ManipulatorType::ManipulatorTypeScale;
		return ManipulatorType::ManipulatorTypeDefault;
	}

	void OpenGLSceneView::CurrentManipulator::set(ManipulatorType value)
	{
		switch (value)
		{
			case ManipulatorType::ManipulatorTypeDefault:
				currentManipulator = defaultManipulator;
				break;
			case ManipulatorType::ManipulatorTypeTranslation:
				currentManipulator = translationManipulator;
				break;
			case ManipulatorType::ManipulatorTypeRotation:
				currentManipulator = rotationManipulator;
				break;
			case ManipulatorType::ManipulatorTypeScale:
				currentManipulator = scaleManipulator;
				break;
			default:
				break;
		}
		this->Invalidate();
	}

	CameraMode OpenGLSceneView::CurrentCameraMode::get()
	{
		return cameraMode;
	}

	void OpenGLSceneView::CurrentCameraMode::set(CameraMode value)
	{	
		if (cameraMode == CameraMode::CameraModePerspective)
		{
			*perspectiveRadians = camera->GetRadians();
		}
		cameraMode = value;
		switch (cameraMode)
		{
			case CameraMode::CameraModePerspective:
				camera->SetRadians(*perspectiveRadians);
				break;
			case CameraMode::CameraModeTop:
				camera->SetRadians(Vector3D(-90.0f * DEG_TO_RAD, 0, 0));
				break;
			case CameraMode::CameraModeBottom:
				camera->SetRadians(Vector3D(90.0f * DEG_TO_RAD, 0, 0));
				break;
			case CameraMode::CameraModeLeft:
				camera->SetRadians(Vector3D(0, -90.0f * DEG_TO_RAD, 0));
				break;
			case CameraMode::CameraModeRight:
				camera->SetRadians(Vector3D(0, 90.0f * DEG_TO_RAD, 0));
				break;
			case CameraMode::CameraModeFront:
				camera->SetRadians(Vector3D(0, 0, 0));
				break;
			case CameraMode::CameraModeBack:
				camera->SetRadians(Vector3D(0, 180.0f * DEG_TO_RAD, 0));
				break;
			default:
				break;
		}
		this->Invalidate();
	}

	ViewMode OpenGLSceneView::CurrentViewMode::get()
	{
		return viewMode;
	}

	void OpenGLSceneView::CurrentViewMode::set(ViewMode value)
	{
		viewMode = value;
		this->Invalidate();
	}

	void OpenGLSceneView::DrawGrid(int size, int step)
	{
		glBegin(GL_LINES);
		for (int x = -size; x <= size; x += step)
		{
			glVertex3i(x, 0, -size);
			glVertex3i(x, 0, size);
		}
		for (int z = -size; z <= size; z += step)
		{
			glVertex3i(-size, 0, z);
			glVertex3i(size, 0, z);
		}
		glEnd();
	}

	void OpenGLSceneView::RenderGL()
	{
		// Clear the background
		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		this->ResizeGL();
		
		// Set the viewpoint
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(camera->GetViewMatrix());
		
		glDisable(GL_LIGHTING);
		
		glColor3f(0.1f, 0.1f, 0.1f);
		this->DrawGrid(10, 2);
		
		glEnable(GL_LIGHTING);
		
		if (displayed != manipulated)
			displayed->Draw(viewMode);
			
		manipulated->Draw(viewMode);
		
		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		
		if (manipulated->SelectedCount > 0)
		{
			currentManipulator->Position = manipulated->SelectionCenter;
			
			if (this->cameraMode == CameraMode::CameraModePerspective)
				currentManipulator->Size = camera->GetPosition().Distance(currentManipulator->Position) * 0.15f;
			else
				currentManipulator->Size = camera->GetZoom() * 0.18f;

			scaleManipulator->Rotation = manipulated->SelectionRotation;
			currentManipulator->Draw(camera->GetAxisZ(), manipulated->SelectionCenter);
		}
			
		if (isSelecting)
		{
			this->BeginOrtho();
			glDisable(GL_TEXTURE_2D);
			float color[4] = { 0.2f, 0.4f, 1.0f, 0.0f };
			color[3] = 0.2f;
			glColor4fv(color);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glRectf(lastPoint.X, this->Height - lastPoint.Y, currentPoint.X, this->Height - currentPoint.Y);
			color[3] = 0.9f;
			glColor4fv(color);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glRectf(lastPoint.X, this->Height - lastPoint.Y, currentPoint.X, this->Height - currentPoint.Y);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			this->EndOrtho();
		}
		
		glEnable(GL_DEPTH_TEST);
		
		glFinish();
		
		glFlush();
	}

	void OpenGLSceneView::ResizeGL()
	{
		float w_h = (float)Width / (float)Height;

		glViewport(0, 0, Width, Height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();			

		ApplyProjection(this->ClientRectangle);
	}

	void OpenGLSceneView::ApplyProjection(RectangleF baseRect)
	{
		float w_h = baseRect.Width / baseRect.Height;
			
		if (cameraMode != CameraMode::CameraModePerspective)
		{
			float x = camera->GetZoom() * w_h;
			float y = camera->GetZoom(); 
			
			x /= 2.0f;
			y /= 2.0f;
			
			glOrtho(-x, x, -y, y, -maxDistance, maxDistance);
		}
		else 
		{
			gluPerspective(perspectiveAngle, w_h, minDistance, maxDistance);
		}
	}

	#pragma region Ortho

	void OpenGLSceneView::BeginOrtho()
	{
		RectangleF rect = this->ClientRectangle;
		glDepthMask(GL_FALSE);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();			
		gluOrtho2D(0, rect.Width, 0, rect.Height);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();			
		glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(1, 1, 1, 1);
	}

	void OpenGLSceneView::EndOrtho()
	{
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glPopMatrix();
		glPopMatrix();				
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();				
		glMatrixMode(GL_MODELVIEW);
		glDepthMask(GL_TRUE);
	}

	#pragma endregion
	
	#pragma region Mouse events

	void OpenGLSceneView::OnMouseDown(MouseEventArgs ^e)
	{
		UserControl::OnMouseDown(e);
		if (e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			lastPoint = PointF((float)e->X, (float)e->Y);
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			lastPoint = PointF((float)e->X, (float)e->Y);
	
			if (manipulated->SelectedCount > 0 && currentManipulator->SelectedIndex >= 0)
			{
				BeginGL();
				if (currentManipulator == translationManipulator)
				{
					*selectionOffset = this->GetTranslation(lastPoint);
					*selectionOffset -= manipulated->SelectionCenter;
					isManipulating = YES;
				}
				else if (currentManipulator == rotationManipulator)
				{
					this->GetRotation(lastPoint, selectionOffset);
					isManipulating = YES;
				}
				else if (currentManipulator == scaleManipulator)
				{
					this->GetScale(lastPoint, selectionOffset);
					isManipulating = YES;
				}

				EndGL();
				if (isManipulating)
				{
					if (theDelegate != nullptr)
						theDelegate->ManipulationStarted();
				}	
			}
			else
			{
				isSelecting = YES;
			}
		}
	}

	void OpenGLSceneView::OnMouseMove(MouseEventArgs ^e)
	{
		UserControl::OnMouseMove(e);
		if (e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			PointF point = PointF((float)e->X, (float)e->Y);
			float diffX = point.X - lastPoint.X;
			float diffY = point.Y - lastPoint.Y;
			
			if ((this->ModifierKeys & System::Windows::Forms::Keys::Alt) == System::Windows::Forms::Keys::Alt)
			{
				if (this->cameraMode == CameraMode::CameraModePerspective)
				{
					const float sensitivity = 0.005f;
					camera->RotateLeftRight(diffX * sensitivity);
					camera->RotateUpDown(diffY * sensitivity);
				}
			}
			else
			{
				RectangleF bounds = this->ClientRectangle;
				float w = bounds.Width;
				float h = bounds.Height;
				float sensitivity = (w + h) / 2.0f;
				sensitivity = 1.0f / sensitivity;
				camera->LeftRight(-diffX * camera->GetZoom() * sensitivity);
				camera->UpDown(-diffY * camera->GetZoom() * sensitivity);
			}
			
			lastPoint = point;
			this->Invalidate();
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::Left)
		{
			currentPoint = PointF((float)e->X, (float)e->Y);
			if (isManipulating)
			{
				BeginGL();
				lastPoint = currentPoint;
				if (currentManipulator == translationManipulator)
				{
					Vector3D move = this->GetTranslation(currentPoint);
					move -= *selectionOffset;
					move -= manipulated->SelectionCenter;
					manipulated->MoveSelectedBy(move);
				}
				else if (currentManipulator == rotationManipulator)
				{
					Quaternion rotation = this->GetRotation(currentPoint, selectionOffset);
					manipulated->RotateSelectedBy(rotation);
				}
				else if (currentManipulator == scaleManipulator)
				{
					Vector3D scale = this->GetScale(currentPoint, selectionOffset);
					manipulated->ScaleSelectedBy(scale);
				}

				EndGL();
				this->Invalidate();
			}
			else if (isSelecting)
			{
				this->Invalidate();
			}
		}
		else if (e->Button == System::Windows::Forms::MouseButtons::None)
		{
			currentPoint = PointF((float)e->X, (float)e->Y);
			if (manipulated->SelectedCount > 0)
			{
				if (!isManipulating)
				{
					currentManipulator->SelectedIndex = -1;
					currentManipulator->Position = manipulated->SelectionCenter;
					BeginGL();
					this->SelectPoint(currentPoint, currentManipulator, OpenGLSelectionModeAdd);
					EndGL();
					this->Invalidate();
				}
			}
		}
	}

	void OpenGLSceneView::OnMouseUp(MouseEventArgs ^e)
	{
		UserControl::OnMouseUp(e);
		currentPoint = PointF((float)e->X, (float)e->Y);
		if (isManipulating)
		{
			if (theDelegate != nullptr)
				theDelegate->ManipulationEnded();
			isManipulating = NO;
		}
		if (isSelecting)
		{
			isSelecting = NO;
			OpenGLSelectionMode selectionMode = OpenGLSelectionModeAdd;
			
			if ((this->ModifierKeys & System::Windows::Forms::Keys::Control) == System::Windows::Forms::Keys::Control)
				selectionMode = OpenGLSelectionModeInvert;
			else if ((this->ModifierKeys & System::Windows::Forms::Keys::Shift) == System::Windows::Forms::Keys::Shift)
				selectionMode = OpenGLSelectionModeAdd;
			else
				manipulated->ChangeSelection(NO);
			
			BeginGL();
			RectangleF rect = this->CurrentRect;
			if (rect.Width > 5.0f && rect.Height > 5.0f)
				this->SelectRect(rect, manipulated, selectionMode);
			else
				this->SelectPoint(currentPoint, manipulated, selectionMode);
			EndGL();
			this->Invalidate();
		}
	}

	void OpenGLSceneView::OnMouseWheel(MouseEventArgs ^e)
	{
		UserControl::OnMouseWheel(e);
		float zoom = e->Delta / 20.0f;
		float sensitivity = camera->GetZoom() * 0.02f;
		
		camera->Zoom(zoom * sensitivity);

		this->Invalidate();
	}

	RectangleF OpenGLSceneView::CurrentRect::get()
	{
		float minX = Min(lastPoint.X, currentPoint.X);
		float maxX = Max(lastPoint.X, currentPoint.X);
		float minY = Min(lastPoint.Y, currentPoint.Y);
		float maxY = Max(lastPoint.Y, currentPoint.Y);
		
		return RectangleF(minX, minY, maxX - minX, maxY - minY);
	}

	#pragma endregion

	#pragma region OpenGL setup

	void OpenGLSceneView::OnLoad(EventArgs ^e)
	{
		UserControl::OnLoad(e);
		InitializeGL();
	}

	void OpenGLSceneView::OnSizeChanged(EventArgs ^e)
	{
		UserControl::OnSizeChanged(e);
		Invalidate();
	}

	void OpenGLSceneView::OnPaintBackground(PaintEventArgs ^e)
	{
		if (this->DesignMode)
			UserControl::OnPaintBackground(e);
	}

	void OpenGLSceneView::OnPaint(PaintEventArgs ^e)
	{
		if (this->DesignMode)
			return;

		if (!deviceContext || !glRenderingContext)
			return;

		BeginGL();
		RenderGL();
		SwapBuffers(deviceContext);
		EndGL();
	}

	void OpenGLSceneView::BeginGL()
	{
		 wglMakeCurrent(deviceContext, glRenderingContext);
	}
	
	void OpenGLSceneView::EndGL()
	{
		wglMakeCurrent(NULL, NULL);
	}

	void OpenGLSceneView::InitializeGL()
	{
        if (this->DesignMode)
			return;
        
		deviceContext = GetDC((HWND)this->Handle.ToPointer());
        // CAUTION: Not doing the following SwapBuffers() on the DC will
        // result in a failure to subsequently create the RC.
		SwapBuffers(deviceContext);

        //Get the pixel format		
        PIXELFORMATDESCRIPTOR pfd;
		ZeroMemory(&pfd, sizeof(pfd));
		pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 32;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int pixelFormatIndex = ChoosePixelFormat(deviceContext, &pfd);
        if (pixelFormatIndex == 0)
        {
			Trace::WriteLine("Unable to retrieve pixel format");
            return;
        }

        if (SetPixelFormat(deviceContext, pixelFormatIndex, &pfd) == 0)
        {
			Trace::WriteLine("Unable to set pixel format");
            return;
        }
        //Create rendering context
        glRenderingContext = wglCreateContext(deviceContext);
        if (!glRenderingContext)
        {
			Trace::WriteLine("Unable to get rendering context");
            return;
        }
		if (wglMakeCurrent(deviceContext, glRenderingContext) == 0)
        {
			Trace::WriteLine("Unable to make rendering context current");
            return;
        }
        //Set up OpenGL related characteristics
        ResizeGL();
        BeginGL();
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);

		// Configure the view
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
		glShadeModel(GL_SMOOTH);
		glEnable(GL_LIGHTING);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHT0);
	}	

	#pragma endregion

	#pragma region Selection

	void OpenGLSceneView::SelectBox(
		RectangleF rect,
		OpenGLSelecting ^selecting,
		CocoaBool nearestOnly,
		OpenGLSelectionMode selectionMode)
	{
		if (selecting == nullptr || selecting->SelectableCount <= 0)
			return;

		rect.Y = this->Height - rect.Y;

		selecting->WillSelect(); // replace with HotChocolate GetDelegate

		/*id aSelecting = selecting;
		if ([aSelecting respondsToSelector:@selector(willSelect)])
		{
			[selecting willSelect];
		}*/
		
		int objectsFound = 0;
		int viewport[4];
		const unsigned int selectBufferSize = 65535;
		unsigned int selectBuffer[selectBufferSize];
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glSelectBuffer(selectBufferSize, selectBuffer);
		glGetIntegerv(GL_VIEWPORT, viewport);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glRenderMode(GL_SELECT);
		glLoadIdentity();
		gluPickMatrix(rect.X, rect.Y, rect.Width, rect.Height, viewport);
		RectangleF baseRect = this->ClientRectangle;
		this->ApplyProjection(baseRect);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(camera->GetViewMatrix());
		glInitNames();
		glPushName(0);
		glPushMatrix();
	    
		for (uint i = 0; i < selecting->SelectableCount; i++)
		{
			glLoadName(i + 1);
			glPushMatrix();
			selecting->DrawForSelection(i);
			glPopMatrix();
		}
	    
		glPopMatrix();
		glFinish();
		objectsFound = (int)glRenderMode(GL_RENDER);
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
			
		if (objectsFound <= 0)
			return;
		
		if (nearestOnly)
		{
			unsigned int lowestDepth = selectBuffer[1];
			int selectedIndex = (int)selectBuffer[3];
			for (int i = 1; i < objectsFound; i++)
			{
				if (selectBuffer[(i * 4) + 1] < lowestDepth)
				{
					lowestDepth = selectBuffer[(i * 4) + 1];
					selectedIndex = (int)selectBuffer[(i * 4) + 3];
				}
			}
			selectedIndex--;
			if (selectedIndex >= 0)
				selecting->SelectObject(selectedIndex, selectionMode);
		}
		else
		{
			for (int i = 0; i < objectsFound; i++)
			{
				int selectedIndex = (int)selectBuffer[(i * 4) + 3];
				selectedIndex--;
				if (selectedIndex >= 0)
					selecting->SelectObject(selectedIndex, selectionMode);
			}
		}

		selecting->DidSelect();

		/*if ([aSelecting respondsToSelector:@selector(didSelect)])
		{
			[selecting didSelect];
		}*/
	}

	void OpenGLSceneView::SelectPoint(
		PointF point, 
		OpenGLSelecting ^selecting, 
		OpenGLSelectionMode selectionMode)

	{
		this->SelectBox(RectangleF(point.X, point.Y, 10.0, 10.0),
			selecting, YES, selectionMode);
	}

	void OpenGLSceneView::SelectRect(
		RectangleF rect,
		OpenGLSelecting ^selecting,
		OpenGLSelectionMode selectionMode)
	{
		this->SelectBox(
			RectangleF(
				rect.X + rect.Width / 2,
				rect.Y + rect.Height / 2,
				rect.Width,
				rect.Height),
			selecting,
			NO,
			selectionMode);
	}

	#pragma endregion

	#pragma region Position retrieve

	Vector3D OpenGLSceneView::GetPositionInSpace(PointF point)
	{
		int viewport[4];
		double modelview[16];
		double projection[16];
		float winX, winY, winZ;
		double posX = 0.0, posY = 0.0, posZ = 0.0;
		
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);
		glGetIntegerv(GL_VIEWPORT, viewport);
		
		winX = point.X;
		winY = this->Height - point.Y;
		glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
		
		return Vector3D((float)posX, (float)posY, (float)posZ);
	}

	void OpenGLSceneView::DrawSelectionPlane(int index)
	{
		Vector3D position = manipulated->SelectionCenter;
			
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		::DrawSelectionPlane((PlaneAxis)(PlaneAxisX + index));
		glPopMatrix();
	}

	Vector3D OpenGLSceneView::GetPositionOnAxis(Axis axis, PointF point)
	{
		const float size = 4000.0f;
		DrawPlane(camera->GetAxisX(), camera->GetAxisY(), size);
		
		Vector3D position = this->GetPositionInSpace(point);
		Vector3D result = manipulated->SelectionCenter;
		result[axis] = position[axis];
		return result;
	}

	Vector3D OpenGLSceneView::GetPositionOnRotatedAxis(Axis axis, PointF point, Quaternion rotation)
	{
		const float size = 4000.0f;
		DrawPlane(camera->GetAxisX(), camera->GetAxisY(), size);
		
		Vector3D position = this->GetPositionInSpace(point);
		Vector3D result = manipulated->SelectionCenter;
		position.Transform(rotation.Conjugate());
		result[axis] = position[axis];
		return result;
	}

	Vector3D OpenGLSceneView::GetPositionOnPlane(PlaneAxis plane, PointF point)
	{
		int index = plane - PlaneAxisX;
		this->DrawSelectionPlane(index);
		Vector3D position = this->GetPositionInSpace(point);
		Vector3D result = position;
		result[index] = manipulated->SelectionCenter[index];
		return result;
	}

	#pragma endregion

	#pragma region Translation, Scale, Rotation

	Vector3D OpenGLSceneView::GetTranslation(PointF point)
	{	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadMatrixf(camera->GetViewMatrix());
		
		Vector3D position = manipulated->SelectionCenter;
		int selectedIndex = currentManipulator->SelectedIndex;
		
		if (selectedIndex >= AxisX && selectedIndex <= AxisZ)
			return this->GetPositionOnAxis((Axis)selectedIndex, point);
		if (selectedIndex >= PlaneAxisX && selectedIndex <= PlaneAxisZ)
			return this->GetPositionOnPlane((PlaneAxis)selectedIndex, point);
		
		return position;
	}

	Vector3D OpenGLSceneView::GetScale(PointF point, Vector3D *lastPosition)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadMatrixf(camera->GetViewMatrix());
		
		Vector3D position = manipulated->SelectionCenter;
		int selectedIndex = currentManipulator->SelectedIndex;
		
		Vector3D scale = Vector3D();
		
		if (selectedIndex >= 0)
		{
			ManipulatorWidget ^selectedWidget = currentManipulator->GetWidget(selectedIndex);
			Axis selectedAxis = selectedWidget->ManipAxis;
			if (selectedAxis >= AxisX && selectedAxis <= AxisZ)
			{
				position = this->GetPositionOnRotatedAxis(selectedAxis, point, manipulated->SelectionRotation);
				scale = position - *lastPosition;
			}
			else if (selectedAxis == Center)
			{
				position = this->GetPositionOnAxis(AxisY, point);
				scale = position - *lastPosition;
				scale.x = scale.y;
				scale.z = scale.y;
			}
			
			*lastPosition = position;
			scale *= 2.0f;
		}
				
		return scale;
	}

	Quaternion OpenGLSceneView::GetRotation(PointF point, Vector3D *lastPosition)
	{	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadMatrixf(camera->GetViewMatrix());
		
		Quaternion quaternion;
		Vector3D position;
		float angle;
		
		int selectedIndex = currentManipulator->SelectedIndex;
		
		position = this->GetPositionOnPlane((PlaneAxis)(selectedIndex + 3), point);
		position -= manipulated->SelectionCenter;
		
		switch(selectedIndex)
		{
			case AxisX:
				angle = atan2f(position.y, position.z) - atan2f(lastPosition->y, lastPosition->z);
				quaternion.FromAngleAxis(-angle, Vector3D(1, 0, 0));
				break;
			case AxisY:
				angle = atan2f(position.x, position.z) - atan2f(lastPosition->x, lastPosition->z);
				quaternion.FromAngleAxis(angle, Vector3D(0, 1, 0));
				break;
			case AxisZ:
				angle = atan2f(position.x, position.y) - atan2f(lastPosition->x, lastPosition->y);
				quaternion.FromAngleAxis(-angle, Vector3D(0, 0, 1));
				break;
			default: break;
		}
		
		*lastPosition = position;
		return quaternion;
	}

	#pragma endregion
}