//
//  MarshalHelpers.cpp
//  OpenGLEditor
//
//  Created by Filip Kunc on 12/23/09.
//  For license see LICENSE.TXT
//

#include <string>
using namespace std;

using namespace System;
using namespace System::Text;

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