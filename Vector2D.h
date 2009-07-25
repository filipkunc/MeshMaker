/*
 *  Vector2D.h
 *  OpenGLWidgets
 *
 *  Created by Filip Kunc on 6/21/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "MathForwardDeclaration.h"

class Vector2D
{
public:
	float x, y;

	Vector2D();
	Vector2D(const float * v);
	Vector2D(const Vector2D & v);
	Vector2D(float x, float y);

	operator float * ();
	operator const float * () const;

	Vector2D & operator += (const Vector2D & v);
	Vector2D & operator -= (const Vector2D & v);
	Vector2D & operator *= (float s);
    Vector2D & operator /= (float s);

	Vector2D operator + () const;
    Vector2D operator - () const;

    Vector2D operator + (const Vector2D & v) const;
    Vector2D operator - (const Vector2D & v) const;
    Vector2D operator * (float s) const;
    Vector2D operator / (float s) const;

    friend Vector2D operator * (float s, const class Vector2D & v);

    bool operator == (const Vector2D & v) const;
    bool operator != (const Vector2D & v) const;

	float Dot(const Vector2D & v) const;
	float GetLength() const;
	float GetLengthSq() const;
	void SetLength(float length);
	void Normalize();
	float Distance(const Vector2D & v) const;
	float SqDistance(const Vector2D & v) const;
	Vector2D Lerp(float w, const Vector2D & v) const;
};