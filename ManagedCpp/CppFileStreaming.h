/*
 *  CppFileStreaming.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 12/23/09.
 *  For license see LICENSE.TXT
 *
 */
#pragma once

#include "../PureCpp/Enums.h"
#include <fstream>
using namespace std;

namespace ManagedCpp
{
	public interface class CppFileStreaming
	{
		void Encode(ofstream *fout);
		void Decode(ifstream *fin);
	};
}