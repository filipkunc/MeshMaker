//
//  MarshalHelpers.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 02/06/10.
//  For license see LICENSE.TXT
//

#include "MarshalHelpers.h"

namespace ManagedCpp
{
	// based on discussion at http://www.codeguru.com/forum/showthread.php?t=467464
	string NativeString(String ^managedString)
	{
		array<Byte> ^chars = Encoding::ASCII->GetBytes(managedString);
		pin_ptr<Byte> charsPointer = &(chars[0]);
		char *nativeCharsPointer = reinterpret_cast<char *>(static_cast<unsigned char *>(charsPointer));
		string native(nativeCharsPointer, chars->Length);
		return native;
	}
}

