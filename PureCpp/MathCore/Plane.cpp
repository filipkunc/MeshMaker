/*
 *  Plane.cpp
 *  OpenGLEditor
 *
 *  Created by Filip Kunc on 7/19/09.
 *  For license see LICENSE.TXT
 *
 */

#include "MathDeclaration.h"
#include "Plane.h"

Plane::Plane(const Vector3D & n, float p)
{
	this->n = n;
	this->p = p;
}

Plane::Plane(const Vector3D & n, const Vector3D & v)
{
	this->n = n;
	this->n.Normalize();
	p = -this->n.Dot(v);
}

float Plane::SignedDistance(const Vector3D & v)
{
	return this->n.Dot(v) + p;
}

float Plane::Distance(const Vector3D & v)
{
	return fabsf(SignedDistance(v));
}

