/*
 *  OpenGLSelecting.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 10/25/09.
 *  For license see LICENSE.TXT
 *
 */
#pragma once

#include "../PureCpp/Enums.h"

namespace ManagedCpp
{
	public interface class OpenGLSelecting
	{
		property uint SelectableCount { uint get(); }
		void DrawForSelection(uint index);
		void SelectObject(uint index, OpenGLSelectionMode selectionMode);

		//@optional
		void WillSelect();
		void DidSelect();
	};
}