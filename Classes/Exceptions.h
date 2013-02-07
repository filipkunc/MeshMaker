//
//  Exceptions.h
//  MeshMaker
//
//  Created by Filip Kunc on 2/6/13.
//
//

#pragma once

#include <exception>

namespace MeshMaker
{
	struct UnsupportedImageFormatException : std::exception
	{
		virtual const char* what() const throw() { return "UnsupportedImageFormat"; }
	};

	struct IndexOutOfRangeException : std::exception
	{
		virtual const char* what() const throw() { return "IndexOutOfRangeException"; }
	};
}

