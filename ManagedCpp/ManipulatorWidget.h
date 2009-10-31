//
//  ManipulatorWidget.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 10/31/09.
//  For license see LICENSE.TXT
//

#pragma once

#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include "../PureCpp/Enums.h"
#include "../PureCpp/OpenGLDrawing.h"

namespace ManagedCpp
{
	public ref class ManipulatorWidget
	{
	private:
		Axis axis;
		Widget widget;
	public:
		property Axis ManipAxis { Axis get(); }
		property Widget ManipWidget { Widget get (); }
	
		ManipulatorWidget(Axis anAxis, Widget aWidget);
		void DrawCore(float size);
		void Draw(float size, CocoaBool isSelected, CocoaBool isGray);
	};
}