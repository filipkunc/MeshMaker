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
		//delegate = nullptr;

		selectionOffset = new Vector3D();
		isManipulating = NO;
		isSelecting = NO;

		camera = new Camera();
		camera->SetRadX(-45.0f * DEG_TO_RAD);
		camera->SetRadY(45.0f * DEG_TO_RAD);
		camera->SetZoom(20.0f);

		perspectiveRadians = new Vector3D(camera->GetRadians());

		lastPoint = PointF(0, 0);

		cameraMode = CameraModePerspective;
		viewMode = ViewModeSolid;

		testMesh = gcnew Mesh();
		testMesh->MakeCube();
	}
	
	OpenGLSceneView::~OpenGLSceneView()
	{
		delete perspectiveRadians;
		delete selectionOffset;
		delete camera;
		if (components)
		{
			delete components;
		}
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
			/*[currentManipulator setPosition:[manipulated selectionCenter]];
			[currentManipulator setSize:camera->GetPosition().Distance([currentManipulator position]) * 0.15f];
			[scaleManipulator setRotation:[manipulated selectionRotation]];
			[currentManipulator drawWithAxisZ:camera->GetAxisZ() center:[manipulated selectionCenter]];*/
		}
			
		if (isSelecting)
		{
			this->BeginOrtho();
			glDisable(GL_TEXTURE_2D);
			float color[4] = { 0.2f, 0.4f, 1.0f, 0.0f };
			color[3] = 0.2f;
			glColor4fv(color);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glRecti(lastPoint.X, lastPoint.Y, currentPoint.X, currentPoint.Y);
			color[3] = 0.9f;
			glColor4fv(color);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glRecti(lastPoint.X, lastPoint.Y, currentPoint.X, currentPoint.Y);
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
			
		if (cameraMode != CameraModePerspective)
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
			lastPoint = PointF(e->X, e->Y);
		}
	}

	void OpenGLSceneView::OnMouseMove(MouseEventArgs ^e)
	{
		UserControl::OnMouseMove(e);
		if (e->Button == System::Windows::Forms::MouseButtons::Middle)
		{
			PointF point = PointF(e->X, e->Y);
			float diffX = point.X - lastPoint.X;
			float diffY = point.Y - lastPoint.Y;
			
			if ((this->ModifierKeys & System::Windows::Forms::Keys::Alt) == System::Windows::Forms::Keys::Alt)
			{
				const float sensitivity = 0.005f;
				camera->RotateLeftRight(diffX * sensitivity);
				camera->RotateUpDown(diffY * sensitivity);
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
	}

	void OpenGLSceneView::OnMouseUp(MouseEventArgs ^e)
	{
		UserControl::OnMouseUp(e);
	}

	void OpenGLSceneView::OnMouseWheel(MouseEventArgs ^e)
	{
		UserControl::OnMouseWheel(e);
		float zoom = e->Delta / 20.0f;
		float sensitivity = camera->GetZoom() * 0.02f;
		
		camera->Zoom(zoom * sensitivity);

		this->Invalidate();
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

        EndGL();
	}	

	#pragma endregion

	#pragma region Selection

	void OpenGLSceneView::Select(
		double x, 
		double y, 
		double width, 
		double height,
		OpenGLSelecting ^selecting,
		CocoaBool nearestOnly,
		OpenGLSelectionMode selectionMode)
	{
		if (selecting == nullptr || selecting->SelectableCount <= 0)
			return;

		selecting->WillSelect(); // replace with HotChocolate GetDelegate

		/*id aSelecting = selecting;
		if ([aSelecting respondsToSelector:@selector(willSelect)])
		{
			[selecting willSelect];
		}*/
		
		int objectsFound = 0;
		int viewport[4];
		unsigned int selectBuffer[1024 * 1024];
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glSelectBuffer(1024 * 1024, selectBuffer);
		glGetIntegerv(GL_VIEWPORT, viewport);
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glRenderMode(GL_SELECT);
		glLoadIdentity();
		gluPickMatrix(x, y, width, height, viewport);
		RectangleF baseRect = this->ClientRectangle;
		this->ApplyProjection(baseRect);
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(camera->GetViewMatrix());
		glInitNames();
		glPushName(0);
		glPushMatrix();
	    
		for (int i = 0; i < selecting->SelectableCount; i++)
		{
			glLoadName((unsigned int)(i + 1));
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

	void OpenGLSceneView::Select(
		PointF point, 
		OpenGLSelecting ^selecting, 
		OpenGLSelectionMode selectionMode)

	{
		this->Select(point.X, point.Y, 10.0, 10.0,
			selecting, YES, selectionMode);
	}

	void OpenGLSceneView::Select(
		RectangleF rect,
		OpenGLSelecting ^selecting,
		OpenGLSelectionMode selectionMode)
	{
		this->Select(
			rect.X + rect.Width / 2,
			rect.Y + rect.Height / 2,
			rect.Width,
			rect.Height,
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
		winY = point.Y;
		glReadPixels((int)winX, (int)winY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
		
		return Vector3D((float)posX, (float)posY, (float)posZ);
	}

	void OpenGLSceneView::DrawSelectionPlane(int index)
	{
		Vector3D position = manipulated->SelectionCenter;
			
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);
		DrawSelectionPlane((PlaneAxis)(PlaneAxisX + index));
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

	/*- (Vector3D)translationFromPoint:(NSPoint)point
	{	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadMatrixf(camera->GetViewMatrix());
		
		Vector3D position = [manipulated selectionCenter];
		int selectedIndex = [currentManipulator selectedIndex];
		
		if (selectedIndex >= AxisX && selectedIndex <= AxisZ)
			return [self positionFromAxis:(Axis)selectedIndex point:point];
		if (selectedIndex >= PlaneAxisX && selectedIndex <= PlaneAxisZ)
			return [self positionFromPlaneAxis:(PlaneAxis)selectedIndex point:point];
		
		return position;
	}

	- (Vector3D)scaleFromPoint:(NSPoint)point lastPosition:(Vector3D *)lastPosition
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadMatrixf(camera->GetViewMatrix());
		
		Vector3D position = [manipulated selectionCenter];
		int selectedIndex = [currentManipulator selectedIndex];
		
		Vector3D scale = Vector3D();
		
		if (selectedIndex >= 0)
		{
			ManipulatorWidget *selectedWidget = [currentManipulator widgetAtIndex:selectedIndex];
			enum Axis selectedAxis = [selectedWidget axis];
			if (selectedAxis >= AxisX && selectedAxis <= AxisZ)
			{
				position = [self positionFromRotatedAxis:selectedAxis point:point rotation:[manipulated selectionRotation]];
				scale = position - *lastPosition;
			}
			else if (selectedAxis == Center)
			{
				position = [self positionFromAxis:AxisY point:point];
				scale = position - *lastPosition;
				scale.x = scale.y;
				scale.z = scale.y;
			}
			
			*lastPosition = position;
			scale *= 2.0f;
		}
				
		return scale;
	}

	- (Quaternion)rotationFromPoint:(NSPoint)point lastPosition:(Vector3D *)lastPosition
	{	
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadMatrixf(camera->GetViewMatrix());
		
		Quaternion quaternion;
		Vector3D position;
		float angle;
		
		int selectedIndex = [currentManipulator selectedIndex];
		
		position = [self positionFromPlaneAxis:(PlaneAxis)(selectedIndex + 3) point:point];
		position -= [manipulated selectionCenter];
		
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
	}*/

	#pragma endregion
}