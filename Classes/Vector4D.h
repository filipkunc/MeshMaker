//
//  Vector4D.h
//  OpenGLEditor
//
//  Created by Filip Kunc on 1/23/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#pragma once

#include "MathForwardDeclaration.h"

class Vector4D
{
public:
	float x, y, z, w;
    
	Vector4D();
	Vector4D(const float * v);
    Vector4D(const Vector3D & v);
	Vector4D(const Vector4D & v);
	Vector4D(float x, float y, float z, float w);
    
	operator float * ();
	operator const float * () const;
    
	Vector4D & operator += (const Vector4D & v);
	Vector4D & operator -= (const Vector4D & v);
	Vector4D & operator *= (float s);
    Vector4D & operator /= (float s);
    
	Vector4D operator + () const;
    Vector4D operator - () const;
    
    Vector4D operator + (const Vector4D & v) const;
    Vector4D operator - (const Vector4D & v) const;
    Vector4D operator * (float s) const;
    Vector4D operator / (float s) const;
    
    friend Vector4D operator * (float s, const class Vector4D & v);
    
    bool operator == (const Vector4D & v) const;
    bool operator != (const Vector4D & v) const;
    
	float Dot(const Vector4D & v) const;
	float GetLength() const;
	float GetLengthSq() const;
	void SetLength(float length);
	void Normalize();
};