/*
 *  Plane.h
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 7/19/09.
 *  For license see LICENSE.TXT
 *
 */

#pragma once

#include "MathForwardDeclaration.h"

class Plane
{
public:
	// Hessian normal form
	// http://mathworld.wolfram.com/HessianNormalForm.html
	Vector3D n; // unit vector, length == 1
	float p;
	
	Plane(const Vector3D & n, float p);
	Plane(const Vector3D & n, const Vector3D & v);
	
	float SignedDistance(const Vector3D & v);
	float Distance(const Vector3D & v);
};