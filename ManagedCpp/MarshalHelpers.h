//
//  MarshalHelpers.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/23/09.
//  For license see LICENSE.TXT
//

#pragma once

#include <string>
using namespace std;

using namespace System;
using namespace System::Text;

namespace ManagedCpp
{
	string NativeString(String ^managedString);
}