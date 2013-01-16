//
//  MarshalHelpers.h
//  MeshMaker
//
//  Created by Filip Kunc on 12/23/09.
//  For license see LICENSE.TXT
//

#pragma once

#include <string>
using namespace std;

using namespace System;
using namespace System::Text;

namespace MeshMakerCppCLI
{
	ref class MarshalHelpers
	{
	public:
		// based on discussion at http://www.codeguru.com/forum/showthread.php?t=467464
		static string NativeString(String ^managedString)
		{
			array<Byte> ^chars = Encoding::ASCII->GetBytes(managedString);
			pin_ptr<Byte> charsPointer = &(chars[0]);
			char *nativeCharsPointer = reinterpret_cast<char *>(static_cast<unsigned char *>(charsPointer));
			string native(nativeCharsPointer, chars->Length);
			return native;
		}

		static String ^ManagedString(string nativeString)
		{
			return gcnew String(nativeString.c_str());
		}
	};
}