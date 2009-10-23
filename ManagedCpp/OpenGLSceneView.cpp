// ManagedCpp.cpp : main project file.

#include "OpenGLSceneView.h"

namespace ManagedCpp {

	const float perspectiveAngle = 45.0f;
	const float minDistance = 0.2f;
	const float maxDistance = 1000.0f;
	
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
        glLoadIdentity();

		ResizeGL();

		// Set the viewpoint
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(camera->GetViewMatrix());

		glDisable(GL_LIGHTING);

		glColor3f(0.1f, 0.1f, 0.1f);
		DrawGrid(10, 2);

		glEnable(GL_LIGHTING);

        glFlush();
	}

	void OpenGLSceneView::ResizeGL()
	{
		float w_h = (float)Width / (float)Height;

		glViewport(0, 0, Width, Height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();			

		gluPerspective(perspectiveAngle, w_h, minDistance, maxDistance);
		glMatrixMode(GL_MODELVIEW);	
		glLoadIdentity();
	}

	#pragma region Mouse events

	void OpenGLSceneView::OnMouseMove(MouseEventArgs ^e)
	{
		UserControl::OnMouseMove(e);
	}

	void OpenGLSceneView::OnMouseDown(MouseEventArgs ^e)
	{
		UserControl::OnMouseDown(e);
	}

	void OpenGLSceneView::OnMouseUp(MouseEventArgs ^e)
	{
		UserControl::OnMouseUp(e);
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
        // CAUTION: Not doing the following WGL.wglSwapBuffers() on the DC will
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
        EndGL();
	}	

	#pragma endregion
}